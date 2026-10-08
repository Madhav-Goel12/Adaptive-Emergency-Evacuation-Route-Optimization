#include "RouteOptimizer.h"
#include "PriorityQueue.h"
#include "PathReconstruction.h"
#include <vector>
#include <limits>
using namespace std;

vector<int> RouteOptimizer::findRoute(Graph &g, int src, int des, double &totalCost)
{
    vector<vector<Road>>& adj = g.getAdjList();
    int n = adj.size();
    vector<double> dist(n, numeric_limits<double>::max());
    vector<int> parent(n, -1);
    PriorityQueue pq;
    dist[src] = 0;
    pq.push(src, 0);
    while(!pq.empty())
    {
        PQNode cur = pq.pop();
        int u = cur.node;
        if(cur.cost != dist[u])
            continue;
        if(u == des)
            break;
        for(int i = 0; i < adj[u].size(); i++)
        {
            Road &road = adj[u][i];
            if(road.getBlockedStatus())
            continue;
            int v = road.getDest();
            double cost = road.getCost();
            if(dist[u] + cost < dist[v])
            {
                dist[v] = dist[u] + cost;
                parent[v] = u;
                pq.push(v, dist[v]);
            }
        }
    }
    if(dist[des] == numeric_limits<double>::max())
    {
        totalCost = -1;
        return {};
    }
    totalCost = dist[des];
    vector<int> path;
    int cur = des;
    while(cur != -1)
    {
        path.push_back(cur);
        if(cur == src)
            break;
        cur = parent[cur];
    }
    if(path.back() != src)
    {
        totalCost = -1;
        return {};
    }
    reverse(path.begin(), path.end());
    return path;
}\
