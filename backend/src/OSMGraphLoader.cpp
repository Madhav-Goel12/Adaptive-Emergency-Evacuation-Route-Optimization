
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
        int64_t osmId;
        double lat;
        double lon;
        int graphId;
    };

    struct OSMWay
    {
        int64_t osmId = 0;
        vector<int64_t> nodeIds;

        bool oneWay = false;
        bool reverseOneWay = false;

        string name;
        string highwayType;
    };

    double haversineDistance(
        double lat1,
        double lon1,
        double lat2,
        double lon2
    )
    {
        const double earthRadius = 6371000.0;
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

        a = max(0.0, min(1.0, a));

        double c = 2.0 * atan2(
            sqrt(a),
            sqrt(1.0 - a)
        );

        return earthRadius * c;
    }

    bool isAllowedHighway(const string& highway)
    {
        static const unordered_set<string> allowed = {
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
}

bool OSMGraphLoader::loadFromOSM(
    const string& filePath,
    Graph& graph
)
{
    XMLDocument doc;

    XMLError error = doc.LoadFile(filePath.c_str());

    if (error != XML_SUCCESS)
    {
        cerr << "OSM XML load failed: "
             << doc.ErrorStr() << endl;

        return false;
    }

    XMLElement* root = doc.FirstChildElement("osm");

    if (!root)
    {
        cerr << "Invalid OSM file: missing <osm> root"
             << endl;

        return false;
    }

    unordered_map<int64_t, OSMNode> nodes;
    vector<OSMWay> ways;

    // --------------------------------
    // 1. Read OSM nodes
    // --------------------------------

    for (
        XMLElement* node = root->FirstChildElement("node");
        node;
        node = node->NextSiblingElement("node")
    )
    {
        int64_t id;
        double lat;
        double lon;

        if (node->QueryInt64Attribute("id", &id) != XML_SUCCESS)
            continue;

        if (node->QueryDoubleAttribute("lat", &lat) != XML_SUCCESS)
            continue;

        if (node->QueryDoubleAttribute("lon", &lon) != XML_SUCCESS)
            continue;

        nodes[id] = {
            id,
            lat,
            lon,
            -1
        };
    }

    // --------------------------------
    // 2. Read OSM road ways and tags
    // --------------------------------

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

        // Read node references
        for (
            XMLElement* nd = way->FirstChildElement("nd");
            nd;
            nd = nd->NextSiblingElement("nd")
        )
        {
            int64_t ref;

            if (
                nd->QueryInt64Attribute("ref", &ref)
                == XML_SUCCESS
            )
            {
                osmWay.nodeIds.push_back(ref);
            }
        }

        string oneWayValue;
        string junctionValue;
        string accessValue;

        // Read OSM tags
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
                osmWay.highwayType = v;

            else if (k == "name")
                osmWay.name = v;

            else if (k == "oneway")
                oneWayValue = v;

            else if (k == "junction")
                junctionValue = v;

            else if (k == "access")
                accessValue = v;
        }

        if (!isAllowedHighway(osmWay.highwayType))
            continue;

        if (
            accessValue == "no" ||
            accessValue == "private"
        )
        {
            continue;
        }

        if (osmWay.nodeIds.size() < 2)
            continue;

        osmWay.reverseOneWay = oneWayValue == "-1";

        osmWay.oneWay =
            oneWayValue == "yes" ||
            oneWayValue == "1" ||
            oneWayValue == "true" ||
            osmWay.reverseOneWay ||
            junctionValue == "roundabout";

        if (osmWay.name.empty())
            osmWay.name = "Unnamed Road";

        ways.push_back(osmWay);
    }

    // --------------------------------
    // 3. Collect nodes used by roads
    // --------------------------------

    unordered_set<int64_t> usedNodeIds;

    for (const auto& way : ways)
    {
        for (int64_t nodeId : way.nodeIds)
        {
            if (nodes.count(nodeId))
                usedNodeIds.insert(nodeId);
        }
    }

    // --------------------------------
    // 4. Assign compact graph IDs
    // --------------------------------

    int nextGraphId = 0;

    for (int64_t osmId : usedNodeIds)
    {
        nodes[osmId].graphId = nextGraphId++;
    }

    // --------------------------------
    // 5. Store graph node coordinates
    // --------------------------------

    for (const auto& entry : nodes)
    {
        const OSMNode& node = entry.second;

        if (node.graphId >= 0)
        {
            graph.setCoordinates(
                node.graphId,
                node.lat,
                node.lon
            );
        }
    }

    // --------------------------------
    // 6. Build graph edges and features
    // --------------------------------

    int roadsAdded = 0;
    int featuresAdded = 0;

    for (const auto& way : ways)
    {
        RoadFeature feature;

        feature.id =
            "osm-way-" + to_string(way.osmId);

        feature.name = way.name;

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

            // Preserve actual OSM road geometry
            if (feature.coordinates.empty())
            {
                feature.coordinates.push_back({
                    from.lat,
                    from.lon
                });
            }

            feature.coordinates.push_back({
                to.lat,
                to.lon
            });

            double distanceMeters = haversineDistance(
                from.lat,
                from.lon,
                to.lat,
                to.lon
            );

            if (distanceMeters <= 0.0)
                continue;

            // Initial approximate road speed
            double speedKmh = 30.0;

            double travelMinutes =
                (distanceMeters / 1000.0) /
                speedKmh * 60.0;

            if (travelMinutes <= 0.0)
                travelMinutes = 0.01;

            int src = from.graphId;
            int dest = to.graphId;

            // Reverse-direction one-way road
            if (way.reverseOneWay)
            {
                swap(src, dest);
            }

            graph.addRoad(
                Road(
                    src,
                    dest,
                    travelMinutes,
                    1.0,
                    way.name
                )
            );

            roadsAdded++;

            // Add reverse edge for two-way roads
            if (!way.oneWay)
            {
                graph.addRoad(
                    Road(
                        dest,
                        src,
                        travelMinutes,
                        1.0,
                        way.name
                    )
                );

                roadsAdded++;
            }
        }

        // Store complete road feature
        if (feature.coordinates.size() >= 2)
        {
            graph.addRoadFeature(feature);
            featuresAdded++;
        }
    }

    // --------------------------------
    // 7. Display loading information
    // --------------------------------

    cout << "\nOSM graph loaded successfully\n";

    cout << "OSM nodes: "
         << usedNodeIds.size() << endl;

    cout << "OSM ways: "
         << ways.size() << endl;

    cout << "Road features: "
         << featuresAdded << endl;

    cout << "Directed roads: "
         << roadsAdded << endl;

    return roadsAdded > 0;
}
