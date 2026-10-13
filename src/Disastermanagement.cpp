#include "DisasterManager.h"
#include <iostream>
#include <vector>

using namespace std;

void DisasterManager::updateRoad(Graph &g, int u, int v, int st) {
    vector<vector<Road>>& adjList = g.getAdjList();
    bool found = false;

    // 1. Update the road from u to v
    if (u >= 0 && u < adjList.size()) {
        for (size_t i = 0; i < adjList[u].size(); i++) {
            if (adjList[u][i].getDest() == v) { // Used getDest() instead of .des
                if (st == 1) { // Safe
                    adjList[u][i].setBlocked(false);
                    adjList[u][i].setHazardLevel(1.0);
                } else if (st == 2) { // Risky
                    adjList[u][i].setBlocked(false);
                    adjList[u][i].setHazardLevel(2.0); // Cost will be multiplied by 2
                } else if (st == 3) { // Blocked
                    adjList[u][i].setBlocked(true);
                    adjList[u][i].setHazardLevel(9999.0);
                } else if (st == 4) { // Congested
                    adjList[u][i].setBlocked(false);
                    adjList[u][i].setHazardLevel(3.0); // Cost will be multiplied by 3
                }
                found = true;
                break;
            }
        }
    }

    // 2. Update the road from v to u (for two-way streets)
    if (v >= 0 && v < adjList.size()) {
        for (size_t i = 0; i < adjList[v].size(); i++) {
            if (adjList[v][i].getDest() == u) { // Used getDest() instead of .des
                if (st == 1) { 
                    adjList[v][i].setBlocked(false);
                    adjList[v][i].setHazardLevel(1.0);
                } else if (st == 2) { 
                    adjList[v][i].setBlocked(false);
                    adjList[v][i].setHazardLevel(2.0); 
                } else if (st == 3) { 
                    adjList[v][i].setBlocked(true);
                    adjList[v][i].setHazardLevel(9999.0);
                } else if (st == 4) { 
                    adjList[v][i].setBlocked(false);
                    adjList[v][i].setHazardLevel(3.0); 
                }
                found = true;
                break;
            }
        }
    }

    if (!found) {
        cout << "Road not found" << endl;
    }
}

void DisasterManager::showRoad(Graph &g) {
    vector<vector<Road>>& adjList = g.getAdjList();
    
    for (size_t i = 0; i < adjList.size(); i++) {
        for (size_t j = 0; j < adjList[i].size(); j++) {
            // Using getSrc() and getDest() instead of .src and .des
            cout << adjList[i][j].getSrc() << " -> " << adjList[i][j].getDest() << " Status: ";
            
            // Check status based on the methods available in Road.h
            if (adjList[i][j].getBlockedStatus()) {
                cout << "Blocked";
            } else {
                // If not blocked, we determine status by comparing the base time vs the calculated cost
                double baseTime = adjList[i][j].getTime();
                double totalCost = adjList[i][j].getCost();
                
                if (totalCost == baseTime) cout << "Safe";
                else if (totalCost == baseTime * 2) cout << "Risky";
                else if (totalCost == baseTime * 3) cout << "Congested";
                else cout << "Unknown";
            }
            
            // Output final cost
            cout << " | Cost: " << adjList[i][j].getCost() << endl;
        }
    }
}