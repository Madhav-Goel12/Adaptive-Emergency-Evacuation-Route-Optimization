#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include "apiclient.h"
#include "DatabaseManager.h"
#include "DisasterManager.h"
#include "Graph.h"
#include "RouteOptimizer.h"
#include "Road.h"
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/types.hpp>
#include "HttpServer.h"
#include "OSMGraphLoader.h"

using namespace std;

static void displayMenu()
{
    cout << "\n=== ADAPTIVE EMERGENCY EVACUATION SYSTEM ===\n"
         << "1. Display road network\n"
         << "2. Find evacuation route\n"
         << "3. Update road condition (and save to MongoDB)\n"
         << "4. Test MongoDB connection\n"
         << "5. Check live weather API\n"
         << "6. Exit\n"
         << "Enter choice: ";
}

static void clearInput()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main()
{
    Graph graph(0);

bool osmLoaded = OSMGraphLoader::loadFromOSM(
    "backend/data/dehradun-roads.osm",
    graph
);

if (!osmLoaded)
{
    cerr << "Failed to load Dehradun OSM graph!" << endl;
    return 1;
}

cout << "Dehradun OSM graph ready!" << endl;
cout << "Total graph nodes: "
     << graph.getNodeCount() << endl;

    const char* uriEnv = std::getenv("MONGODB_URI");
    const char* dbEnv = std::getenv("MONGODB_DATABASE");

    bool dbConfigured = uriEnv && *uriEnv && dbEnv && *dbEnv;
    bool roadsLoaded = true;

    RouteOptimizer optimizer;
    DisasterManager disasterManager;

    runHttpServer(graph);

    int choice = 0;
    do {
        displayMenu();
        if (!(cin >> choice)) {
            clearInput();
            cout << "Please enter a number from 1 to 6.\n";
            continue;
        }

        switch (choice) {
        case 1:
            graph.showGraph();
            break;

        case 2: {
            int src, dest;
            cout << "Enter source and destination location IDs (0-5): ";
            if (!(cin >> src >> dest)) {
                clearInput();
                cout << "Invalid input.\n";
                break;
            }
            double totalCost = -1;
            vector<int> path = optimizer.findRoute(graph, src, dest, totalCost);
            if (path.empty()) {
                cout << "No route found for those locations.\n";
            } else {
                cout << "Best route: ";
                for (size_t i = 0; i < path.size(); ++i) {
                    if (i) cout << " -> ";
                    cout << path[i];
                }
                cout << "\nTotal cost: " << totalCost << '\n';
            }
            break;
        }

        case 3: {
            if (!dbConfigured) {
                cout << "MongoDB is not configured. Set MONGODB_URI and "
                        "MONGODB_DATABASE in the environment first.\n";
                break;
            }
            int u, v, status;
            cout << "Enter road endpoints (u v) and status "
                    "(1=Safe, 2=Risky, 3=Blocked, 4=Congested): ";
            if (!(cin >> u >> v >> status)) {
                clearInput();
                cout << "Invalid input.\n";
                break;
            }
            try {
                DatabaseManager db(uriEnv, dbEnv);
                if (!db.ping()) {
                    cout << "MongoDB connection failed; road not saved.\n";
                    break;
                }
                graph.updateRoadStatus(u, v, status);
            } catch (const std::exception& e) {
                cout << "MongoDB error: " << e.what() << '\n';
            }
            break;
        }

        case 4: {
            if (!dbConfigured) {
                cout << "Set MONGODB_URI and MONGODB_DATABASE first.\n";
                break;
            }
            try {
                DatabaseManager db(uriEnv, dbEnv);
                db.ping();
            } catch (const std::exception& e) {
                cout << "MongoDB error: " << e.what() << '\n';
            }
            break;
        }

        case 5: {
            double lat, lon;

            cout << "Enter latitude: ";
            if (!(cin >> lat)) {
                clearInput();
                cout << "Invalid input for latitude.\n";
                break;
            }

            cout << "Enter longitude: ";
            if (!(cin >> lon)) {
                clearInput();
                cout << "Invalid input for longitude.\n";
                break;
            }

            APIClient api;
            if (api.getData(lat, lon)) {
                api.showData();
            } else {
                cout << "Could not fetch weather data.\n";
            }
            break;
        }

        case 6:
            cout << "Thank you for using the system!\n";
            break;

        default:
            cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 6);

    return 0;
}  