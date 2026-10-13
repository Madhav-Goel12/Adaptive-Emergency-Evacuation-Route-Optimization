#include <iostream>
#include <string>
#include "Graph.h"
#include "Road.h"
#include "DisasterManager.h"
#include "RouteOptimizer.h"
using namespace std;

void displayMenu() {
    cout << "\n=== Adaptive Emergency Evacuation Route Optimization ===" << endl;
    cout << "1. Load Data (Graph & Locations)" << endl;
    cout << "2. Update Road Status (Disaster Event)" << endl;
    cout << "3. Show Current Roads" << endl;
    cout << "4. Find Optimal Evacuation Route" << endl;
    cout << "0. Exit" << endl;
    cout << "======================================================" << endl;
    cout << "Enter your choice: ";
}

int main() {
    // FIX: Passing an argument for the number of vertices to satisfy the Graph constructor.
    // Change '100' to the actual maximum number of nodes/locations your project uses.
    int totalNodes = 100;
    Graph cityGraph(totalNodes); 
    
    DisasterManager disasterMgr;
    // RouteOptimizer optimizer; // Uncomment and initialize when ready

    int choice;
    bool running = true;

    while (running) {
        displayMenu();
        if (!(cin >> choice)) {
             cout << "Invalid input. Please enter a number." << endl;
             cin.clear();
             cin.ignore(10000, '\n');
             continue;
        }

        switch (choice) {
            case 1:
                cout << "Loading data... (Functionality to be implemented)" << endl;
                // Example usage once DataLoader is ready:
                // DataLoader::loadGraph(cityGraph, "data.txt");
                break;

            case 2: {
                int u, v, status;
                cout << "Enter start node (u): ";
                cin >> u;
                cout << "Enter end node (v): ";
                cin >> v;
                cout << "Enter new status (e.g., 0 for normal, 1 for blocked/damaged): ";
                cin >> status;
                
                disasterMgr.updateRoad(cityGraph, u, v, status);
                cout << "Road status updated." << endl;
                break;
            }

            case 3:
                cout << "Current Road Status:" << endl;
                disasterMgr.showRoad(cityGraph);
                break;

            case 4: {
                int startNode, destNode;
                cout << "Enter starting location ID: ";
                cin >> startNode;
                cout << "Enter destination safe zone ID: ";
                cin >> destNode;
                
                cout << "Calculating optimal route... (Functionality to be linked)" << endl;
                // Example usage once RouteOptimizer is fully linked:
                // vector<int> route = optimizer.findRoute(cityGraph, startNode, destNode);
                // Print route here...
                break;
            }

            case 0:
                cout << "Exiting system. Stay safe!" << endl;
                running = false;
                break;

            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
}