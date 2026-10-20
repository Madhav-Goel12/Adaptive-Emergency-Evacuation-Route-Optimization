#include "RouteOptimizer.h"
#include "priorityqueue.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

using namespace std;

vector<int> RouteOptimizer::findRoute(Graph &g, int src, int des, double &totalCost)
{
    vector<vector<Road>>& adj = g.getAdjList();
    const int n = static_cast<int>(adj.size());

    totalCost = -1;
    if (src < 0 || des < 0 || src >= n || des >= n)
        return {};

    vector<double> dist(n, numeric_limits<double>::infinity());
    vector<int> parent(n, -1);
    PriorityQueue pq;

    dist[src] = 0.0;
    pq.push(src, 0.0);

    while (!pq.empty()) {
        PQNode cur = pq.pop();
        int u = cur.node;

        if (u < 0 || u >= n)
            continue;
        if (cur.cost != dist[u])
            continue;
        if (u == des)
            break;

        for (auto &road : adj[u]) {
            if (road.getBlockedStatus())
                continue;

            int v = road.getDest();
            if (v < 0 || v >= n)
                continue;

            double edgeCost = road.getCost();
            if (edgeCost < 0)
                continue;

            if (dist[u] + edgeCost < dist[v]) {
                dist[v] = dist[u] + edgeCost;
                parent[v] = u;
                pq.push(v, dist[v]);
            }
        }
    }

    if (!std::isfinite(dist[des]))
        return {};

    vector<int> path;
    for (int cur = des; cur != -1; cur = parent[cur]) {
        path.push_back(cur);
        if (cur == src)
            break;
    }

    if (path.empty() || path.back() != src)
        return {};

    reverse(path.begin(), path.end());
    totalCost = dist[des];
    return path;
}