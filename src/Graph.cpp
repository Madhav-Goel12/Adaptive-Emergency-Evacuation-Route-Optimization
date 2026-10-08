#include "Graph.h"

Graph::Graph(int totalLocations) 
{
    adjList.resize(totalLocations);
}
void Graph::addRoad(Road road) 
{
    int source = road.getSrc();

    if (source >= 0 && source < adjList.size()) {
        adjList[source].push_back(road);
    }
}
void Graph::showGraph() 
{
    cout << "\n     ADJACENCY LIST      " << endl;

    for (int i = 0; i < adjList.size(); i++) 
    {
        cout << "Location " << i << " : ";
        if (adjList[i].empty()) 
        {
            cout << "   No outgoing roads   ";
        }
        else 
        {
            for (int j = 0; j < adjList[i].size(); j++) 
            {
                cout << adjList[i][j].getDest();
                if (j != adjList[i].size() - 1) {
                    cout << " -> ";
                }
            }
        }
        cout << endl;
    }
}