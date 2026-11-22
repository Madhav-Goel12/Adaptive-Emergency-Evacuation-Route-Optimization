
#include "Graph.h"
#include "apiclient.h"
#include "RouteOptimizer.h"
#include "httplib.h"

#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <functional>

using namespace std;

namespace
{
    // --------------------------------
    // JSON helpers
    // --------------------------------

    string escapeJson(const string& value)
    {
        string result;

        for (char c : value)
        {
            switch (c)
            {
                case '"':  result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default:   result += c; break;
            }
        }

        return result;
    }

    bool parseCoordinates(
        const string& body,
        const string& key,
        double& latitude,
        double& longitude
    )
    {
        string searchKey = "\"" + key + "\"";

        size_t position = body.find(searchKey);

        if (position == string::npos)
            return false;

        position = body.find('[', position);

        if (position == string::npos)
            return false;

        try
        {
            size_t firstEnd;
            latitude = stod(body.substr(position + 1), &firstEnd);

            size_t secondStart =
                body.find(',', position + 1);

            if (secondStart == string::npos)
                return false;

            size_t secondEnd;

            longitude = stod(
                body.substr(secondStart + 1),
                &secondEnd
            );
        }
        catch (...)
        {
            return false;
        }

        return latitude >= -90.0 &&
               latitude <= 90.0 &&
               longitude >= -180.0 &&
               longitude <= 180.0;
    }

    double distanceKm(
        double lat1,
        double lon1,
        double lat2,
        double lon2
    )
    {
        const double earthRadius = 6371.0;
        const double pi = 3.14159265358979323846;

        lat1 *= pi / 180.0;
        lon1 *= pi / 180.0;
        lat2 *= pi / 180.0;
        lon2 *= pi / 180.0;

        double dLat = lat2 - lat1;
        double dLon = lon2 - lon1;

        double a =
            sin(dLat / 2.0) * sin(dLat / 2.0) +
            cos(lat1) * cos(lat2) *
            sin(dLon / 2.0) * sin(dLon / 2.0);

        double c = 2.0 * atan2(
            sqrt(a),
            sqrt(1.0 - a)
        );

        return earthRadius * c;
    }

    int findNearestNode(
        Graph& graph,
        double latitude,
        double longitude
    )
    {
        int nearestNode = -1;
        double minimumDistance =
            numeric_limits<double>::max();

        for (int i = 0; i < graph.getNodeCount(); i++)
        {
            Coordinates point = graph.getCoordinates(i);

            if (point.latitude == 0.0 &&
                point.longitude == 0.0)
            {
                continue;
            }

            double distance = distanceKm(
                latitude,
                longitude,
                point.latitude,
                point.longitude
            );

            if (distance < minimumDistance)
            {
                minimumDistance = distance;
                nearestNode = i;
            }
        }

        // Avoid snapping locations too far from the road network.
        if (minimumDistance > 5.0)
            return -1;

        return nearestNode;
    }

    string getRoadStatus(const Road& road)
    {
        if (road.getBlockedStatus())
            return "blocked";

        double time = road.getTime();
        double cost = road.getCost();

        if (time <= 0.0)
            return "safe";

        if (cost >= time * 2.5)
            return "congested";

        if (cost >= time * 1.5)
            return "risky";

        return "safe";
    }

    // Shortest weighted distances from a node. When reverse=true, traverse
    // incoming edges so the result is the cost from each node to destination.
    vector<double> routeDistances(Graph& graph, int start, bool reverse)
    {
        const auto& adjacency = graph.getAdjList();
        const int count = static_cast<int>(adjacency.size());
        vector<vector<pair<int, double>>> edges(count);

        for (int u = 0; u < count; ++u)
        {
            for (const auto& road : adjacency[u])
            {
                if (road.getBlockedStatus()) continue;
                const int v = road.getDest();
                if (v < 0 || v >= count) continue;
                if (reverse) edges[v].push_back({u, road.getCost()});
                else edges[u].push_back({v, road.getCost()});
            }
        }

        vector<double> distances(count, numeric_limits<double>::infinity());
        using Entry = pair<double, int>;
        priority_queue<Entry, vector<Entry>, greater<Entry>> queue;
        if (start < 0 || start >= count) return distances;
        distances[start] = 0.0;
        queue.push({0.0, start});

        while (!queue.empty())
        {
            auto [cost, u] = queue.top();
            queue.pop();
            if (cost != distances[u]) continue;
            for (const auto& edge : edges[u])
            {
                const int v = edge.first;
                const double next = cost + edge.second;
                if (next < distances[v])
                {
                    distances[v] = next;
                    queue.push({next, v});
                }
            }
        }
        return distances;
    }

    // --------------------------------
    // API: GET /api/health
    // --------------------------------

    void healthHandler(
        const httplib::Request&,
        httplib::Response& response
    )
    {
        response.set_content(
            R"({"status":"ok"})",
            "application/json"
        );
    }

    // --------------------------------
    // API: GET /api/roads
    // --------------------------------

    void roadsHandler(
        Graph& graph,
        const httplib::Request&,
        httplib::Response& response
    )
    {
        unordered_map<string, string> statuses;

        // Calculate status from actual graph edges.
        for (const auto& edges : graph.getAdjList())
        {
            for (const auto& road : edges)
            {
                string name = road.getName();

                if (name.empty())
                    name = "Unnamed Road";

                string status = getRoadStatus(road);

                auto it = statuses.find(name);

                if (it == statuses.end())
                {
                    statuses[name] = status;
                }
                else
                {
                    // More severe status takes priority.
                    if (status == "blocked" ||
                        (status == "congested" &&
                         it->second != "blocked") ||
                        (status == "risky" &&
                         it->second == "safe"))
                    {
                        it->second = status;
                    }
                }
            }
        }

        const auto& features = graph.getRoadFeatures();

        ostringstream json;

        json << "[";

        bool first = true;

        for (const auto& feature : features)
        {
            if (feature.coordinates.size() < 2)
                continue;

            if (!first)
                json << ",";

            first = false;

            string name = feature.name.empty()
                ? "Unnamed Road"
                : feature.name;

            string status = "safe";

            if (statuses.count(name))
                status = statuses[name];

            json << "{";

            json << "\"id\":\""
                 << escapeJson(feature.id) << "\",";

            json << "\"name\":\""
                 << escapeJson(name) << "\",";

            json << "\"status\":\""
                 << status << "\",";

            json << "\"coordinates\":[";

            for (
                size_t i = 0;
                i < feature.coordinates.size();
                i++
            )
            {
                if (i > 0)
                    json << ",";

                json << "["
                     << fixed << setprecision(7)
                     << feature.coordinates[i].latitude
                     << ","
                     << feature.coordinates[i].longitude
                     << "]";
            }

            json << "]}";
        }

        json << "]";

        response.set_content(
            json.str(),
            "application/json"
        );
    }

    // --------------------------------
    // API: GET /api/weather
    // --------------------------------

    void weatherHandler(
        const httplib::Request&,
        httplib::Response& response
    )
    {
        APIClient weather;
        if (!weather.getData(30.3165, 78.0322))
        {
            response.status = 502;
            response.set_content(
                R"({"error":"Unable to retrieve current Dehradun weather from Open-Meteo"})",
                "application/json"
            );
            return;
        }

        string condition = "Clear sky";
        if (weather.weatherCode == 1 || weather.weatherCode == 2) condition = "Mainly clear / partly cloudy";
        else if (weather.weatherCode == 3) condition = "Overcast";
        else if (weather.weatherCode >= 45 && weather.weatherCode <= 48) condition = "Fog";
        else if (weather.weatherCode >= 51 && weather.weatherCode <= 67) condition = "Drizzle or rain";
        else if (weather.weatherCode >= 71 && weather.weatherCode <= 77) condition = "Snow";
        else if (weather.weatherCode >= 80 && weather.weatherCode <= 82) condition = "Rain showers";
        else if (weather.weatherCode >= 95) condition = "Thunderstorm";

        ostringstream json;
        json << fixed << setprecision(1)
             << "{\"temperature\":" << weather.temp
             << ",\"humidity\":" << weather.humidity
             << ",\"precipitation\":" << weather.rain
             << ",\"windSpeed\":" << weather.wind
             << ",\"weatherCode\":" << weather.weatherCode
             << ",\"condition\":\"" << escapeJson(condition) << "\"}";
        response.set_content(json.str(), "application/json");
    }

    // --------------------------------
    // API: GET /api/alerts
    // --------------------------------

    void alertsHandler(
        const httplib::Request&,
        httplib::Response& response
    )
    {
        // No live alert provider is connected yet.
        response.set_content(
            "[]",
            "application/json"
        );
    }

    // --------------------------------
    // API: POST /api/route
    // --------------------------------

    void routeHandler(
        Graph& graph,
        const httplib::Request& request,
        httplib::Response& response
    )
    {
        double startLat, startLon;
        double destLat, destLon;

        bool validStart = parseCoordinates(
            request.body,
            "startCoordinates",
            startLat,
            startLon
        );

        bool validDestination = parseCoordinates(
            request.body,
            "destinationCoordinates",
            destLat,
            destLon
        );

        if (!validStart || !validDestination)
        {
            response.status = 400;

            response.set_content(
                R"({"error":"Valid startCoordinates and destinationCoordinates are required"})",
                "application/json"
            );

            return;
        }

        int source = findNearestNode(
            graph,
            startLat,
            startLon
        );

        int destination = findNearestNode(
            graph,
            destLat,
            destLon
        );

        if (source < 0 || destination < 0)
        {
            response.status = 404;

            response.set_content(
                R"({"error":"No nearby road found within 5 km of the selected location"})",
                "application/json"
            );

            return;
        }

        RouteOptimizer optimizer;

        double totalCost = 0.0;

        vector<int> path = optimizer.findRoute(
            graph,
            source,
            destination,
            totalCost
        );

        if (path.empty())
        {
            response.status = 404;

            response.set_content(
                R"({"error":"No route found between selected locations"})",
                "application/json"
            );

            return;
        }

        double totalDistance = 0.0;
        double totalDuration = 0.0;

        vector<Coordinates> routeCoordinates;

        // Include the exact selected starting location.
        routeCoordinates.push_back({
            startLat,
            startLon
        });

        auto& adjacency = graph.getAdjList();

        // Build actual route geometry and calculate travel time.
        for (size_t i = 0; i < path.size(); i++)
        {
            Coordinates point = graph.getCoordinates(path[i]);

            if (point.latitude != 0.0 ||
                point.longitude != 0.0)
            {
                routeCoordinates.push_back(point);
            }

            if (i + 1 < path.size())
            {
                int u = path[i];
                int v = path[i + 1];

                bool edgeFound = false;

                for (const auto& road : adjacency[u])
                {
                    if (road.getDest() == v)
                    {
                        if (road.getBlockedStatus())
                        {
                            response.status = 409;

                            response.set_content(
                                R"({"error":"Selected route contains a blocked road"})",
                                "application/json"
                            );

                            return;
                        }

                        totalDuration += road.getTime();

                        edgeFound = true;
                        break;
                    }
                }

                if (!edgeFound)
                {
                    response.status = 500;

                    response.set_content(
                        R"({"error":"Route edge could not be verified"})",
                        "application/json"
                    );

                    return;
                }
            }
        }

        // Include the exact selected destination.
        routeCoordinates.push_back({
            destLat,
            destLon
        });

        // Calculate actual route distance.
        for (
            size_t i = 1;
            i < routeCoordinates.size();
            i++
        )
        {
            totalDistance += distanceKm(
                routeCoordinates[i - 1].latitude,
                routeCoordinates[i - 1].longitude,
                routeCoordinates[i].latitude,
                routeCoordinates[i].longitude
            );
        }

        // Find risky/blocked road segments that are relevant to this
        // origin-destination corridor, rather than listing network-wide risks.
        const vector<double> fromStart = routeDistances(graph, source, false);
        const vector<double> toDestination = routeDistances(graph, destination, true);
        unordered_set<long long> selectedEdges;
        const long long nodeCount = static_cast<long long>(adjacency.size());
        for (size_t i = 0; i + 1 < path.size(); ++i)
        {
            const int a = min(path[i], path[i + 1]);
            const int b = max(path[i], path[i + 1]);
            selectedEdges.insert(static_cast<long long>(a) * nodeCount + b);
        }

        struct AvoidedRoad
        {
            int from;
            int to;
            string name;
            string status;
            string reason;
            double score;
            double hazardFactor;
        };
        vector<AvoidedRoad> avoided;
        unordered_set<string> reportedRoads;
       // Only consider alternatives reasonably close to the selected journey.
const double relevanceLimit = totalCost * 1.40 + 2.0;

        for (int u = 0; u < static_cast<int>(adjacency.size()); ++u)
        {
            for (const auto& road : adjacency[u])
            {
                const int v = road.getDest();
                if (v < 0 || v >= static_cast<int>(adjacency.size())) continue;
                const int edgeA = min(u, v);
                const int edgeB = max(u, v);
                if (selectedEdges.count(static_cast<long long>(edgeA) * nodeCount + edgeB)) continue;

                const string status = getRoadStatus(road);
                if (status == "safe") continue;
                if (!isfinite(fromStart[u]) || !isfinite(toDestination[v])) continue;

                const double factor = road.getTime() > 0.0
    ? road.getCost() / road.getTime() : 1.0;

// Estimate the complete origin-to-destination alternative.
// For blocked roads, use their base travel time for comparison;
// never treat a blocked edge as a usable zero-cost shortcut.
const double comparisonCost = (status == "blocked")
    ? max(road.getTime(), road.getCost())
    : road.getCost();

const double candidateScore =
    fromStart[u] + comparisonCost + toDestination[v];

// Exclude roads that are already part of an equally good or
// better journey, and ignore alternatives that are too expensive.
if (candidateScore <= totalCost + 0.01) continue;
if (candidateScore > relevanceLimit) continue;

                string name = road.getName();
                if (name.empty()) name = "Unnamed road segment " + to_string(edgeA) + "-" + to_string(edgeB);
                // Identify each road segment by its unique pair of graph nodes.
// Different segments with the same road name must remain separate.
const long long reportKey =
    static_cast<long long>(edgeA) * nodeCount + edgeB;

if (reportedRoads.count(to_string(reportKey))) continue;
reportedRoads.insert(to_string(reportKey));

                string reason;
                if (status == "blocked")
                    reason = "Excluded because this road segment is marked blocked in the road network.";
                else if (status == "congested")
                    reason = "Not selected because its hazard-adjusted travel cost is " + to_string(factor).substr(0, 4) + " times its base travel time; a lower-cost alternative was found.";
                else
                    reason = "Not selected because its hazard-adjusted travel cost is " + to_string(factor).substr(0, 4) + " times its base travel time; a lower-cost alternative was found.";

                avoided.push_back({u, v, name, status, reason, candidateScore, factor});
            }
        }

        sort(avoided.begin(), avoided.end(), [](const AvoidedRoad& a, const AvoidedRoad& b) {
            const auto severity = [](const string& status) {
                if (status == "blocked") return 0;
                if (status == "congested") return 1;
                return 2;
            };
            if (severity(a.status) != severity(b.status)) return severity(a.status) < severity(b.status);
            return a.score < b.score;
        });
        if (avoided.size() > 6) avoided.resize(6);

        ostringstream details;
        details << "Route calculated using Dijkstra algorithm. "
                << path.size() << " graph nodes included. "
                << avoided.size() << " relevant risky or blocked alternative road segments identified.";

        ostringstream json;

        json << fixed << setprecision(6);

        json << "{";

        json << "\"distanceKm\":"
             << totalDistance << ",";

        json << "\"durationMinutes\":"
             << totalDuration << ",";

        json << "\"coordinates\":[";

        for (
            size_t i = 0;
            i < routeCoordinates.size();
            i++
        )
        {
            if (i > 0)
                json << ",";

            json << "["
                 << routeCoordinates[i].latitude
                 << ","
                 << routeCoordinates[i].longitude
                 << "]";
        }

        json << "],";

        json << "\"avoidedRoads\":[";
        for (size_t i = 0; i < avoided.size(); ++i)
        {
            if (i) json << ",";
            const auto& item = avoided[i];
            json << "{\"id\":\"" << item.from << "-" << item.to
                 << "\",\"name\":\"" << escapeJson(item.name)
                 << "\",\"status\":\"" << item.status
                 << "\",\"reason\":\"" << escapeJson(item.reason)
                 << "\",\"hazardFactor\":" << item.hazardFactor
                 << ",\"coordinates\":[["
                 << graph.getCoordinates(item.from).latitude << ","
                 << graph.getCoordinates(item.from).longitude << "],["
                 << graph.getCoordinates(item.to).latitude << ","
                 << graph.getCoordinates(item.to).longitude << "]]" << "}";
        }
        json << "],";

        json << "\"details\":\""
             << escapeJson(details.str())
             << "\"";

        json << "}";

        response.set_content(
            json.str(),
            "application/json"
        );
    }
}

// --------------------------------
// Start HTTP server
// --------------------------------

void runHttpServer(Graph& graph)
{
    httplib::Server server;

    // CORS configuration
    server.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });

    server.Options(
        ".*",
        [](const httplib::Request&,
           httplib::Response& response)
        {
            response.status = 204;
        }
    );

    server.Get(
        "/api/health",
        healthHandler
    );

    server.Get(
        "/api/roads",
        [&graph](
            const httplib::Request& request,
            httplib::Response& response)
        {
            roadsHandler(graph, request, response);
        }
    );

    server.Get(
        "/api/weather",
        weatherHandler
    );

    server.Get(
        "/api/alerts",
        alertsHandler
    );

    server.Post(
        "/api/route",
        [&graph](
            const httplib::Request& request,
            httplib::Response& response)
        {
            routeHandler(graph, request, response);
        }
    );

    cout << "\n====================================\n";
    cout << " Emergency Evacuation API Server\n";
    cout << "====================================\n";
    cout << "Health:  /api/health\n";
    cout << "Roads:   /api/roads\n";
    cout << "Weather: /api/weather\n";
    cout << "Alerts:  /api/alerts\n";
    cout << "Route:   /api/route\n";
    cout << "Server listening on port 8080\n";
    cout << "====================================\n";

    server.listen("0.0.0.0", 8080);
}
