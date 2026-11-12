
#include "Graph.h"
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
        // No live weather provider is connected yet.
        response.set_content(
            "{}",
            "application/json"
        );
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

        ostringstream details;

        details << "Route calculated using Dijkstra algorithm. "
                << path.size()
                << " graph nodes included.";

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
