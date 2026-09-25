#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
using namespace std;
class Graph 
{
private:
    vector<vector<Road>> adjList;
public:
    // Constructor
    Graph(int totalLocations);

    // Add a road to the graph
    void addRoad(const Road& road);

    // Display complete adjacency list
    void showGraph() const;

    // Display roads going out from one location
    void showOutgoingRoads(int locationId) const;

    // Return number of locations
    int getNumberOfLocations() const;

    // Get roads connected to a location
    const vector<Road>& getNeighbors(int locationId) const;
};

#endif