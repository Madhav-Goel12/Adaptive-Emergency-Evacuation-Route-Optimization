#include <iostream>
#include <string>
#include <vector>
#include "Graph.h"
#include "Road.h"
#include "DisasterManager.h"
#include "RouteOptimizer.h"
#include "apiclient.h"

using namespace std;

// Sample network setup class structure to match your project data
class Location {
private:
    int locationId;
    string locationName;
    bool isSafeZone;
public:
    Location(int id, string name, bool safe = false) {
        locationId = id;
        locationName = name;
        isSafeZone = safe;
    }
    int getId() { return locationId; }
    string getName() { return locationName; }
    bool checkSafeZone() { return isSafeZone; }
    void printInfo() {
        cout << "ID: " << locationId << " | Name: " << locationName << " | Safe Zone: " << (isSafeZone ? "YES" : "NO") << endl;
    }
};

void displayMenu() {
    cout << "\n=== Adaptive Emergency Evacuation Route Optimization ===" << endl;
    cout << "1. Load Sample Network Data (Dehradun Map)" << endl;
    cout << "2. Update Road Status (Disaster Event)" << endl;
    cout << "3. Show Current Roads" << endl;
    cout << "4. Find Optimal Evacuation Route" << endl;
    cout << "5. Fetch Live Weather Status (API)" << endl;
    cout << "0. Exit" << endl;
    cout << "======================================================" << endl;
    cout << "Enter your choice: ";
}

int main() {
    int totalNodes = 6;
    Graph cityGraph(totalNodes); 
    DisasterManager disasterMgr;
    RouteOptimizer optimizer;

    // Store sample locations and edges
    vector<Location> locations = {
        Location(0, "Clock Tower", false),
        Location(1, "ISBT Junction", false),
        Location(2, "Rajpur Road", false),
        Location(3, "Ballupur Flyover", false),
        Location(4, "Military Shelter A", true),
        Location(5, "City General Hospital", true)
    };

    bool dataLoaded = false;

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
            case 1: {
                // Load sample edges into graph
                cityGraph.addRoad(Road(0, 1, 12.5));
                cityGraph.addRoad(Road(0, 2, 8.0));
                cityGraph.addRoad(Road(1, 3, 15.0));
                cityGraph.addRoad(Road(2, 3, 6.5));
                cityGraph.addRoad(Road(2, 4, 22.0));
                cityGraph.addRoad(Road(3, 4, 10.0));
                cityGraph.addRoad(Road(3, 5, 18.0));
                
                // Add return paths for two-way traversal
                cityGraph.addRoad(Road(1, 0, 12.5));
                cityGraph.addRoad(Road(2, 0, 8.0));
                cityGraph.addRoad(Road(3, 1, 15.0));
                cityGraph.addRoad(Road(3, 2, 6.5));
                cityGraph.addRoad(Road(4, 2, 22.0));
                cityGraph.addRoad(Road(4, 3, 10.0));
                cityGraph.addRoad(Road(5, 3, 18.0));

                dataLoaded = true;
                cout << "\n[Success] Sample network and locations loaded successfully!" << endl;
                break;
            }

            case 2: {
                if (!dataLoaded) {
                    cout << "Please load data first (Option 1)." << endl;
                    break;
                }
                int u, v, status;
                cout << "Enter start node (u): ";
                cin >> u;
                cout << "Enter end node (v): ";
                cin >> v;
                cout << "Enter status (1: Safe, 2: Risky, 3: Blocked, 4: Congested): ";
                cin >> status;
                
                disasterMgr.updateRoad(cityGraph, u, v, status);
                cout << "Road status updated." << endl;
                break;
            }

            case 3:
                if (!dataLoaded) {
                    cout << "Please load data first (Option 1)." << endl;
                    break;
                }
                cout << "\nCurrent Road Status:" << endl;
                disasterMgr.showRoad(cityGraph);
                break;

            case 4: {
                if (!dataLoaded) {
                    cout << "Please load data first (Option 1)." << endl;
                    break;
                }
                int startNode, destNode;
                cout << "Enter starting location ID (0-5): ";
                cin >> startNode;
                cout << "Enter destination safe zone ID (4 or 5): ";
                cin >> destNode;
                
                double totalCost = 0;
                vector<int> route = optimizer.findRoute(cityGraph, startNode, destNode, totalCost);
                
                if (totalCost == -1 || route.empty()) {
                    cout << "❌ No safe route found to destination!" << endl;
                } else {
                    cout << "\n✅ Optimal Evacuation Route Found:\nPath: ";
                    for (size_t i = 0; i < route.size(); i++) {
                        cout << route[i];
                        if (i != route.size() - 1) cout << " -> ";
                    }
                    cout << "\nTotal Route Cost/Time: " << totalCost << " mins" << endl;
                }
                break;
            }

            case 5: {
                APIClient client;
                // Default coordinates for Dehradun
                if (client.getData(30.3165, 78.0322)) {
                    client.showData();
                } else {
                    cout << "Failed to fetch live weather data." << endl;
                }
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