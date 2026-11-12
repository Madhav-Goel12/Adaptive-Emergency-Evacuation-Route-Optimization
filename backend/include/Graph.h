
#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <string>
#include "Road.h"

using namespace std;

struct Coordinates
{
    double latitude;
    double longitude;
};

struct RoadFeature
{
    string id;
    string name;
    vector<Coordinates> coordinates;
};

class Graph
{
private:
    vector<vector<Road>> adjList;
    vector<Coordinates> nodeCoordinates;

    // Stores complete OSM road geometry and metadata
    vector<RoadFeature> roadFeatures;

public:
    Graph(int totalLocations);

    // OSM node coordinates
    void setCoordinates(int nodeId, double latitude, double longitude);
    Coordinates getCoordinates(int nodeId) const;
    int getNodeCount() const;

    // Road feature storage
    void addRoadFeature(const RoadFeature& feature);
    const vector<RoadFeature>& getRoadFeatures() const;

    // Existing graph functions
    void addRoad(Road road);
    void showGraph();

    vector<vector<Road>>& getAdjList();

    bool updateRoadStatus(int u, int v, int status);

    bool getRoadDetails(
        int u,
        int v,
        double& time,
        bool& blocked,
        double& cost,
        int& status
    );
};

#endif
