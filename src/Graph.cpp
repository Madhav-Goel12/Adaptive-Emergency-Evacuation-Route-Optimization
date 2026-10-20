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
bool Graph::updateRoadStatus(int u, int v, int status)
{
    if (u < 0 || u >= static_cast<int>(adjList.size()) ||
        v < 0 || v >= static_cast<int>(adjList.size()))
        return false;

    bool found = false;

    auto update = [&](int src, int dest) {
        for (auto &road : adjList[src]) {
            if (road.getDest() == dest) {
                if (status == 1) {
                    road.setBlocked(false);
                    road.setHazardLevel(1.0);
                } else if (status == 2) {
                    road.setBlocked(false);
                    road.setHazardLevel(2.0);
                } else if (status == 3) {
                    road.setBlocked(true);
                } else if (status == 4) {
                    road.setBlocked(false);
                    road.setHazardLevel(3.0);
                } else {
                    return;
                }
                found = true;
            }
        }
    };

    if (status < 1 || status > 4)
        return false;

    update(u, v);
    update(v, u);

    return found;
}

bool Graph::getRoadDetails(
    int u, int v, double& time, bool& blocked, double& cost, int& status)
{
    if (u < 0 || u >= static_cast<int>(adjList.size()))
        return false;

    for (auto &road : adjList[u]) {
        if (road.getDest() == v) {
            time = road.getTime();
            blocked = road.getBlockedStatus();
            cost = road.getCost();

            if (blocked)
                status = 3;
            else if (time != 0.0 && cost >= time * 2.5)
                status = 4;
            else if (time != 0.0 && cost >= time * 1.5)
                status = 2;
            else
                status = 1;

            return true;
        }
    }
    if (v >= 0 && v < static_cast<int>(adjList.size())) {
    for (auto &road : adjList[v]) {
        if (road.getDest() == u) {
            time = road.getTime();
            blocked = road.getBlockedStatus();
            cost = road.getCost();

            if (blocked)
                status = 3;
            else if (time != 0.0 && cost >= time * 2.5)
                status = 4;
            else if (time != 0.0 && cost >= time * 1.5)
                status = 2;
            else
                status = 1;

            return true;
        }
    }
}
    return false;
}
vector<vector<Road>>& Graph::getAdjList()
{
    return adjList;
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

