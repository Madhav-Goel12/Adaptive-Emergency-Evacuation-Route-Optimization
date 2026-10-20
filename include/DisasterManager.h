#ifndef DISASTER_MANAGER_H
#define DISASTER_MANAGER_H

#include "Graph.h"
#include "Road.h"
#include "DatabaseManager.h"

class DisasterManager
{
public:
    void updateRoad(Graph &g, int u, int v, int status,
                    DatabaseManager &db);

    void showRoad(Graph &g);
};

#endif
