#include "Graph.h"
#include <algorithm>
Graph::Graph(int totalLocations)
{
    if (totalLocations < 0)
        totalLocations = 0;

    adjList.resize(totalLocations);
    nodeCoordinates.resize(totalLocations, {0.0, 0.0});
}

// Store coordinates for an OSM node
void Graph::setCoordinates(
    int nodeId,
    double latitude,
    double longitude
)
{
    if (nodeId < 0)
        return;

    if (nodeId >= static_cast<int>(nodeCoordinates.size()))
    {
        nodeCoordinates.resize(nodeId + 1, {0.0, 0.0});
        adjList.resize(nodeId + 1);
    }

    nodeCoordinates[nodeId] = {latitude, longitude};
}

// Get coordinates for a graph node
Coordinates Graph::getCoordinates(int nodeId) const
{
    if (nodeId < 0 ||
        nodeId >= static_cast<int>(nodeCoordinates.size()))
    {
        return {0.0, 0.0};
    }

    return nodeCoordinates[nodeId];
}

// Number of graph nodes
int Graph::getNodeCount() const
{
    return static_cast<int>(adjList.size());
}

// Add road
void Graph::addRoad(Road road)
{
    int source = road.getSrc();
    int destination = road.getDest();

    if (source < 0 || destination < 0)
        return;

    int requiredSize = max(source, destination) + 1;

    if (requiredSize > static_cast<int>(adjList.size()))
    {
        adjList.resize(requiredSize);
        nodeCoordinates.resize(requiredSize, {0.0, 0.0});
    }

    adjList[source].push_back(road);
}

// Update road status
bool Graph::updateRoadStatus(int u, int v, int status)
{
    if (u < 0 ||
        v < 0 ||
        u >= static_cast<int>(adjList.size()) ||
        v >= static_cast<int>(adjList.size()))
    {
        return false;
    }

    if (status < 1 || status > 4)
        return false;

    bool found = false;

    auto update = [&](int src, int dest)
    {
        for (auto& road : adjList[src])
        {
            if (road.getDest() != dest)
                continue;

            if (status == 1)
            {
                road.setBlocked(false);
                road.setHazardLevel(1.0);
            }
            else if (status == 2)
            {
                road.setBlocked(false);
                road.setHazardLevel(2.0);
            }
            else if (status == 3)
            {
                road.setBlocked(true);
            }
            else if (status == 4)
            {
                road.setBlocked(false);
                road.setHazardLevel(3.0);
            }

            found = true;
        }
    };

    update(u, v);
    update(v, u);

    return found;
}

// Get road details
bool Graph::getRoadDetails(
    int u,
    int v,
    double& time,
    bool& blocked,
    double& cost,
    int& status
)
{
    if (u < 0 ||
        v < 0 ||
        u >= static_cast<int>(adjList.size()) ||
        v >= static_cast<int>(adjList.size()))
    {
        return false;
    }

    auto findRoad = [&](int src, int dest) -> bool
    {
        for (auto& road : adjList[src])
        {
            if (road.getDest() == dest)
            {
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

        return false;
    };

    if (findRoad(u, v))
        return true;

    return findRoad(v, u);
}

// Existing adjacency list
vector<vector<Road>>& Graph::getAdjList()
{
    return adjList;
}

// Display graph
void Graph::showGraph()
{
    cout << "\n     ADJACENCY LIST\n";

    for (int i = 0; i < static_cast<int>(adjList.size()); i++)
    {
        cout << "Location " << i << " : ";

        if (adjList[i].empty())
        {
            cout << "No outgoing roads";
        }
        else
        {
            for (int j = 0;
                 j < static_cast<int>(adjList[i].size());
                 j++)
            {
                cout << adjList[i][j].getDest();

                if (j != static_cast<int>(adjList[i].size()) - 1)
                    cout << " -> ";
            }
        }

        cout << endl;
    }
}
void Graph::addRoadFeature(const RoadFeature& feature)
{
    roadFeatures.push_back(feature);
}

const vector<RoadFeature>& Graph::getRoadFeatures() const
{
    return roadFeatures;
}