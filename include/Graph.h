#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include "Road.h"
using namespace std;
class Graph 
{
private:
    vector<vector<Road>> adjList;
public:
    Graph(int totalLocations);
    void addRoad(Road road);
    void showGraph();
    vector<vector<Road>>& getAdjList();
    bool updateRoadStatus(int u, int v, int status);
bool getRoadDetails(int u, int v, double& time,
                    bool& blocked, double& cost, int& status);
};
#endif