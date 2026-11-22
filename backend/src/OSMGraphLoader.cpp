
#include "OSMGraphLoader.h"

#include <tinyxml2.h>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <cmath>
#include <iostream>
#include <algorithm>

using namespace tinyxml2;
using namespace std;

namespace
{
    struct OSMNode
    {
        int64_t osmId = 0;
        double lat = 0.0;
        double lon = 0.0;
        int graphId = -1;
    };

    struct OSMWay
    {
        int64_t osmId = 0;
        vector<int64_t> nodeIds;

        bool oneWay = false;
        bool reverseOneWay = false;

        string name;
        string highwayType;
        string surface;
        string maxSpeed;
    };

    constexpr double PI = 3.14159265358979323846;

    double haversineDistance(
        double lat1,
        double lon1,
        double lat2,
        double lon2
    )
    {
        const double earthRadius = 6371000.0;

        double dLat = (lat2 - lat1) * PI / 180.0;
        double dLon = (lon2 - lon1) * PI / 180.0;

        double a =
            sin(dLat / 2.0) * sin(dLat / 2.0) +
            cos(lat1 * PI / 180.0) *
            cos(lat2 * PI / 180.0) *
            sin(dLon / 2.0) * sin(dLon / 2.0);

        a = max(0.0, min(1.0, a));

        double c = 2.0 * atan2(
            sqrt(a),
            sqrt(1.0 - a)
        );

        return earthRadius * c;
    }

    bool isAllowedHighway(const string& highway)
    {
        static const unordered_set<string> allowed =
        {
            "motorway",
            "motorway_link",
            "trunk",
            "trunk_link",
            "primary",
            "primary_link",
            "secondary",
            "secondary_link",
            "tertiary",
            "tertiary_link",
            "unclassified",
            "residential",
            "living_street",
            "service"
        };

        return allowed.count(highway) > 0;
    }

    double getHighwaySpeed(const string& highway)
    {
        if (highway == "motorway") return 80.0;
        if (highway == "motorway_link") return 40.0;

        if (highway == "trunk") return 60.0;
        if (highway == "trunk_link") return 40.0;

        if (highway == "primary") return 50.0;
        if (highway == "primary_link") return 35.0;

        if (highway == "secondary") return 40.0;
        if (highway == "secondary_link") return 30.0;

        if (highway == "tertiary") return 30.0;
        if (highway == "tertiary_link") return 25.0;

        if (highway == "unclassified") return 25.0;
        if (highway == "residential") return 25.0;

        if (highway == "living_street") return 15.0;
        if (highway == "service") return 15.0;

        return 20.0;
    }

    double getHighwayHazard(const string& highway)
    {
        if (
            highway == "tertiary" ||
            highway == "tertiary_link"
        )
            return 1.2;

        if (
            highway == "unclassified" ||
            highway == "residential"
        )
            return 1.3;

        if (
            highway == "living_street" ||
            highway == "service"
        )
            return 1.2;

        return 1.0;
    }

    double getSurfaceHazard(const string& surface)
    {
        if (surface == "unpaved" || surface == "gravel")
            return 1.5;

        if (
            surface == "dirt" ||
            surface == "ground" ||
            surface == "mud"
        )
            return 1.8;

        if (surface == "wood" || surface == "pebblestone")
            return 1.4;

        return 1.0;
    }

    double getEffectiveSpeed(
        const string& highway,
        const string& maxSpeed
    )
    {
        double speed = getHighwaySpeed(highway);

        if (maxSpeed.empty())
            return speed;

        try
        {
            size_t parsedChars = 0;
            double limit = stod(maxSpeed, &parsedChars);

            if (maxSpeed.find("mph") != string::npos)
                limit *= 1.60934;

            if (limit > 0.0)
                speed = min(speed, limit);
        }
        catch (...)
        {
            // Unrecognized maxspeed values use estimated speed.
        }

        return speed > 0.0
            ? speed
            : getHighwaySpeed(highway);
    }
}

bool OSMGraphLoader::loadFromOSM(
    const string& filePath,
    Graph& graph
)
{
    XMLDocument doc;

    if (doc.LoadFile(filePath.c_str()) != XML_SUCCESS)
    {
        cerr << "Failed to load OSM file: "
             << filePath << endl;
        return false;
    }

    XMLElement* root = doc.FirstChildElement("osm");

    if (!root)
    {
        cerr << "Invalid OSM file: missing <osm> root element."
             << endl;
        return false;
    }

    unordered_map<int64_t, OSMNode> nodes;
    vector<OSMWay> ways;

    // Read all OSM nodes.
    for (
        XMLElement* node = root->FirstChildElement("node");
        node;
        node = node->NextSiblingElement("node")
    )
    {
        OSMNode n;

        if (
            node->QueryInt64Attribute("id", &n.osmId) != XML_SUCCESS ||
            node->QueryDoubleAttribute("lat", &n.lat) != XML_SUCCESS ||
            node->QueryDoubleAttribute("lon", &n.lon) != XML_SUCCESS
        )
        {
            continue;
        }

        nodes[n.osmId] = n;
    }

    // Read allowed OSM road ways and their tags.
    for (
        XMLElement* way = root->FirstChildElement("way");
        way;
        way = way->NextSiblingElement("way")
    )
    {
        OSMWay osmWay;

        if (
            way->QueryInt64Attribute(
                "id",
                &osmWay.osmId
            ) != XML_SUCCESS
        )
        {
            continue;
        }

        for (
            XMLElement* nd = way->FirstChildElement("nd");
            nd;
            nd = nd->NextSiblingElement("nd")
        )
        {
            int64_t ref = 0;

            if (
                nd->QueryInt64Attribute("ref", &ref)
                == XML_SUCCESS
            )
            {
                osmWay.nodeIds.push_back(ref);
            }
        }

        bool allowed = false;
        bool privateAccess = false;

        for (
            XMLElement* tag = way->FirstChildElement("tag");
            tag;
            tag = tag->NextSiblingElement("tag")
        )
        {
            const char* key = tag->Attribute("k");
            const char* value = tag->Attribute("v");

            if (!key || !value)
                continue;

            string k = key;
            string v = value;

            if (k == "highway")
            {
                osmWay.highwayType = v;
                allowed = isAllowedHighway(v);
            }
            else if (k == "name")
            {
                osmWay.name = v;
            }
            else if (k == "oneway")
            {
                if (
                    v == "yes" ||
                    v == "1" ||
                    v == "true"
                )
                {
                    osmWay.oneWay = true;
                }
                else if (v == "-1")
                {
                    osmWay.reverseOneWay = true;
                }
            }
            else if (
                k == "junction" &&
                v == "roundabout"
            )
            {
                osmWay.oneWay = true;
            }
            else if (k == "access")
            {
                if (v == "no" || v == "private")
                    privateAccess = true;
            }
            else if (k == "surface")
            {
                osmWay.surface = v;
            }
            else if (k == "maxspeed")
            {
                osmWay.maxSpeed = v;
            }
        }

        if (
            !allowed ||
            privateAccess ||
            osmWay.nodeIds.size() < 2
        )
        {
            continue;
        }

        if (osmWay.name.empty())
            osmWay.name = "Unnamed Road";

        ways.push_back(osmWay);
    }

    // Keep only nodes referenced by accepted roads.
    unordered_set<int64_t> usedNodes;

    for (const OSMWay& way : ways)
    {
        for (int64_t id : way.nodeIds)
        {
            if (nodes.count(id))
                usedNodes.insert(id);
        }
    }

    // Assign compact graph IDs and store coordinates.
    int nextId = 0;

    for (auto& entry : nodes)
    {
        if (usedNodes.count(entry.first))
        {
            entry.second.graphId = nextId++;

            graph.setCoordinates(
                entry.second.graphId,
                entry.second.lat,
                entry.second.lon
            );
        }
    }

    // Convert OSM ways into map features and graph edges.
    int roadCount = 0;
    int featureCount = 0;

    for (const OSMWay& way : ways)
    {
        double speedKmh = getEffectiveSpeed(
            way.highwayType,
            way.maxSpeed
        );

        double hazard =
            getHighwayHazard(way.highwayType) *
            getSurfaceHazard(way.surface);

        // -----------------------------------------
        // NEW: CREATE ROAD GEOMETRY FOR FRONTEND
        // -----------------------------------------

        RoadFeature feature;

        feature.id = to_string(way.osmId);
        feature.name = way.name;

        for (int64_t nodeId : way.nodeIds)
        {
            if (!nodes.count(nodeId))
                continue;

            const OSMNode& node = nodes.at(nodeId);

            if (node.graphId < 0)
                continue;

            feature.coordinates.push_back(
                Coordinates{
                    node.lat,
                    node.lon
                }
            );
        }

        if (feature.coordinates.size() >= 2)
        {
            graph.addRoadFeature(feature);
            ++featureCount;
        }

        // -----------------------------------------
        // EXISTING: DIJKSTRA GRAPH EDGE CREATION
        // -----------------------------------------

        for (
            size_t i = 0;
            i + 1 < way.nodeIds.size();
            ++i
        )
        {
            int64_t osmFrom = way.nodeIds[i];
            int64_t osmTo = way.nodeIds[i + 1];

            if (
                !nodes.count(osmFrom) ||
                !nodes.count(osmTo)
            )
            {
                continue;
            }

            const OSMNode& from = nodes.at(osmFrom);
            const OSMNode& to = nodes.at(osmTo);

            if (
                from.graphId < 0 ||
                to.graphId < 0
            )
            {
                continue;
            }

            double distanceMeters = haversineDistance(
                from.lat,
                from.lon,
                to.lat,
                to.lon
            );

            if (distanceMeters <= 0.0)
                continue;

            double travelMinutes =
                (distanceMeters / 1000.0) /
                speedKmh * 60.0;

            if (travelMinutes <= 0.0)
                travelMinutes = 0.01;

            // Normal OSM direction.
            if (!way.reverseOneWay)
            {
                graph.addRoad(
                    Road(
                        from.graphId,
                        to.graphId,
                        travelMinutes,
                        hazard,
                        way.name
                    )
                );

                ++roadCount;
            }

            // Bidirectional road.
            if (
                !way.oneWay &&
                !way.reverseOneWay
            )
            {
                graph.addRoad(
                    Road(
                        to.graphId,
                        from.graphId,
                        travelMinutes,
                        hazard,
                        way.name
                    )
                );

                ++roadCount;
            }

            // Reverse one-way road.
            if (way.reverseOneWay)
            {
                graph.addRoad(
                    Road(
                        to.graphId,
                        from.graphId,
                        travelMinutes,
                        hazard,
                        way.name
                    )
                );

                ++roadCount;
            }
        }
    }

    cout << "\nOSM DATA LOADED SUCCESSFULLY\n";
    cout << "Nodes: " << nextId << endl;
    cout << "Ways: " << ways.size() << endl;
    cout << "Map Road Features: " << featureCount << endl;
    cout << "Directed Road Segments: " << roadCount << endl;

    return true;
}
