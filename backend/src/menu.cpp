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
    Graph graph(6);

    const char* uriEnv = std::getenv("MONGODB_URI");
    const char* dbEnv = std::getenv("MONGODB_DATABASE");

    bool dbConfigured = uriEnv && *uriEnv && dbEnv && *dbEnv;
    bool roadsLoaded = false;

    if (dbConfigured) {
        try {
            DatabaseManager db(uriEnv, dbEnv);

            if (db.ping()) {
                auto roads = db.getAllRoads();

                for (const auto& document : roads) {
                    auto view = document.view();

                    auto fromElement = view["fromLocation"];
                    auto toElement = view["toLocation"];
                    auto distanceElement = view["distance"];
                    auto statusElement = view["status"];

                    if (!fromElement || !toElement || !distanceElement || !statusElement) {
                        continue;
                    }

                    int u = (fromElement.type() == bsoncxx::type::k_int32) 
                        ? fromElement.get_int32().value 
                        : static_cast<int>(fromElement.get_double().value);
                        
                    int v = (toElement.type() == bsoncxx::type::k_int32) 
                        ? toElement.get_int32().value 
                        : static_cast<int>(toElement.get_double().value);
                        
                    double distance = (distanceElement.type() == bsoncxx::type::k_double) 
                        ? distanceElement.get_double().value 
                        : static_cast<double>(distanceElement.get_int32().value);
                        
                    int status = (statusElement.type() == bsoncxx::type::k_int32) 
                        ? statusElement.get_int32().value 
                        : static_cast<int>(statusElement.get_double().value);

                    if (u < 0 || u >= 6 || v < 0 || v >= 6 || u == v || status < 1 || status > 4 || distance <= 0) {
                        continue;
                    }

                    double hazard = 1.0;
                    if (status == 2) hazard = 2.0;
                    if (status == 4) hazard = 3.0;

                    Road road1(u, v, distance, hazard);
                    Road road2(v, u, distance, hazard); // Add reverse edge for 2-way road

                    if (status == 3) {
                        road1.setBlocked(true);
                        road2.setBlocked(true);
                    }

                    graph.addRoad(road1);
                    graph.addRoad(road2);
                }

                roadsLoaded = !roads.empty();
                if (roadsLoaded) {
                    cout << "Loaded road records from MongoDB.\n";
                }
            }
        } catch (const std::exception& e) {
            cerr << "Could not load roads from MongoDB: " << e.what() << '\n';
        }
    }

    if (!roadsLoaded) {
        cout << "Using the demonstration road network.\n";

        auto addTwoWay = [&](int u, int v, double time) {
            graph.addRoad(Road(u, v, time));
            graph.addRoad(Road(v, u, time));
        };

        addTwoWay(0, 1, 12.5);
        addTwoWay(0, 2, 8.0);
        addTwoWay(1, 3, 15.0);
        addTwoWay(2, 3, 6.5);
        addTwoWay(2, 4, 22.0);
        addTwoWay(3, 4, 10.0);
        addTwoWay(3, 5, 18.0);
    }

    RouteOptimizer optimizer;
    DisasterManager disasterManager;

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
                disasterManager.updateRoad(graph, u, v, status, db);
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