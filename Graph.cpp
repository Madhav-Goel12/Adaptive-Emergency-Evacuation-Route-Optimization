#include "Graph.h"
#include <iostream>

using namespace std;

// Constructor
Graph::Graph(int totalLocations) 
{
    adjList.resize(totalLocations);
}
// Add a road to the adjacency list
void Graph::addRoad(const Road& road) 
{
    int source = road.getSrc();
    if (source >= 0 &&
        source < static_cast<int>(adjList.size())) {

        adjList[source].push_back(road);
    }
}
// Display the complete adjacency list
void Graph::showGraph() const 
{
    cout << "\n=============== ADJACENCY LIST ===============" << endl;
    for (int i = 0;i < static_cast<int>(adjList.size());i++) 
         {
        cout << "Location " << i << " : ";
        if (adjList[i].empty()) {
            cout << "No outgoing roads";
        }
        else {
            for (int j = 0;
                 j < static_cast<int>(adjList[i].size());
                 j++) {

                cout << adjList[i][j].getDest();

                if (j != static_cast<int>(adjList[i].size()) - 1) {
                    cout << " -> ";
                }
            }
        }

        cout << endl;
    }
}
// Display all roads going out from one location
void Graph::showOutgoingRoads(int locationId) const 
{
    if (locationId < 0 ||
        locationId >= static_cast<int>(adjList.size())) 
        {
        cout << "Invalid location ID." << endl;
        return;
    }
    cout << "\nRoads from Location "
         << locationId << ":" << endl;
    if (adjList[locationId].empty()) {
        cout << "No outgoing roads." << endl;
        return;
    }
    for (const Road& road : adjList[locationId]) {
        road.printRoad();
    }
}
// Return number of locations
int Graph::getNumberOfLocations() const
 {

    return static_cast<int>(adjList.size());
}
// Return roads connected to a location
const vector<Road>& Graph::getNeighbors(int locationId) const
 {

    return adjList[locationId];
}