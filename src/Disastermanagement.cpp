#include "DisasterManager.h"

#include <iostream>
#include <algorithm>

using namespace std;

void DisasterManager::updateRoad(
    Graph &g, int u, int v, int status, DatabaseManager &db)
{
    if (status < 1 || status > 4) {
        cout << "Invalid road status." << endl;
        return;
    }

    if (!g.updateRoadStatus(u, v, status)) {
        cout << "Road not found or could not be updated." << endl;
        return;
    }

    double time = 0.0;
    double cost = 0.0;
    bool blocked = false;
    int actualStatus = status;

    if (!g.getRoadDetails(u, v, time, blocked, cost, actualStatus)) {
        cout << "Road updated in graph, but details could not be read."
             << endl;
        return;
    }

    int roadId = min(u, v) * 100000 + max(u, v);

    if (db.saveRoad(roadId, u, v, time, actualStatus, cost)) {
        cout << "Road updated and saved to MongoDB." << endl;
    } else {
        cout << "Road updated in graph, but MongoDB save failed." << endl;
    }
}

void DisasterManager::showRoad(Graph &g)
{
    cout << "Use Graph::showGraph() to display the road network."
         << endl;
    g.showGraph();
}


