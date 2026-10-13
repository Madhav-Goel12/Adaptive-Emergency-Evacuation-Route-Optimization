
#ifndef DISASTER_MANAGER_H
#define DISASTER_MANAGER_H

#include "Graph.h"
#include "Road.h"

class DisasterManager
{
public:
    void updateRoad(Graph &g, int u, int v, int st);
    void showRoad(Graph &g);
};

#endif