#ifndef ROUTE_OPTIMIZER_H
#define ROUTE_OPTIMIZER_H

#include "Graph.h"
#include <vector>
using namespace std;

class RouteOptimizer
{
public:
    vector<int> findRoute(Graph &g, int src, int des, double &totalCost);
};

#endif
