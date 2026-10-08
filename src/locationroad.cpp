#include <iostream>
#include <string>
#include <vector>
using namespace std;
class Location {
private:
    int locationId;
    string locationName;
    bool isSafeZone;
public:
    Location() {
        locationId = -1;
        locationName = "";
        isSafeZone = false;
    }
    Location(int id, string name, bool safe = false) {
        locationId = id;
        locationName = name;
        isSafeZone = safe;
    }
    int getId() { 
        return locationId; 
    }
    string getName() { 
        return locationName; 
    }
    bool checkSafeZone() { 
        return isSafeZone; 
    }
    void setSafeZone(bool status) { 
        isSafeZone = status; 
    }
    void printInfo() {
        cout << "ID: " << locationId 
             << " | Name: " << locationName 
             << " | Safe Zone: " << (isSafeZone ? "YES" : "NO") 
             << endl;
    }
};
class Road {
private:
    int u;            
    int v;            
    double travelTime;
    bool isBlocked;
    double hazardLevel; 
public:
    Road() {
        u = -1;
        v = -1;
        travelTime = 0.0;
        isBlocked = false;
        hazardLevel = 1.0;
    }
    Road(int src, int dest, double time, double hazard = 1.0) {
        u = src;
        v = dest;
        travelTime = time;
        isBlocked = false;
        hazardLevel = hazard;
    }
    int getSrc() { return u; }
    int getDest() { return v; }
    double getTime() { return travelTime; }
    bool getBlockedStatus() { return isBlocked; }
    void setBlocked(bool status) { 
        isBlocked = status; 
    }
    void setHazardLevel(double hazard) { 
        hazardLevel = hazard; 
    }
    double getCost() {
        if (isBlocked) {
            return 999999.0; 
        }
        return travelTime * hazardLevel;
    }
    void printRoad() {
        cout << "Edge (" << u << " -> " << v << ")"
             << " | Time: " << travelTime << " mins"
             << " | Blocked: " << (isBlocked ? "True" : "False")
             << " | Dynamic Weight: " << getCost() 
             << endl;
    }
};

class Graph 
{
private:

    vector<vector<Road>> adjList;

public:

    Graph(int totalLocations)
    {
        adjList.resize(totalLocations);
    }

    void addRoad(Road road)
    {
        int source = road.getSrc();

        if (source >= 0 && source < adjList.size()) {
            adjList[source].push_back(road);
        }
    }

    void showGraph() {
        cout << "\n     ADJACENCY LIST      " << endl;

        for (int i = 0; i < adjList.size(); i++) {
            cout << "Location " << i << " : ";

            if (adjList[i].empty()) {
                cout << "   No outgoing roads   ";
            }
            else {
                for (int j = 0; j < adjList[i].size(); j++) {
                    cout << adjList[i][j].getDest();

                    if (j != adjList[i].size() - 1) {
                        cout << " -> ";
                    }
                }
            }

            cout << endl;
        }
    }
};

class NetworkSetup {
public:
    vector<Location> nodes;
    vector<Road> edges;

    void buildSampleGraph() {
        nodes.push_back(Location(0, "Clock Tower", false));
        nodes.push_back(Location(1, "ISBT Junction", false));
        nodes.push_back(Location(2, "Rajpur Road", false));
        nodes.push_back(Location(3, "Ballupur Flyover", false));
        nodes.push_back(Location(4, "Military Shelter A", true));
        nodes.push_back(Location(5, "City General Hospital", true));
        edges.push_back(Road(0, 1, 12.5));
        edges.push_back(Road(0, 2, 8.0));
        edges.push_back(Road(1, 3, 15.0));
        edges.push_back(Road(2, 3, 6.5));
        edges.push_back(Road(2, 4, 22.0));
        edges.push_back(Road(3, 4, 10.0));
        edges.push_back(Road(3, 5, 18.0));
    }
    void displayNetwork() {
        cout << "\n=== LOCATIONS LIST ===" << endl;
        for (size_t i = 0; i < nodes.size(); i++) {
            nodes[i].printInfo();
        }

        cout << "\n=== ROADS LIST ===" << endl;
        for (size_t i = 0; i < edges.size(); i++) {
            edges[i].printRoad();
        }
    }
};
int main() 
{
    NetworkSetup demoMap;
    demoMap.buildSampleGraph();
    Graph roadGraph(demoMap.nodes.size());
    for (int i = 0; i < demoMap.edges.size(); i++)
    {
        roadGraph.addRoad(demoMap.edges[i]);
    }
    demoMap.displayNetwork();
    roadGraph.showGraph();
    cout << "\n     Updating Road Condition     " << endl;
    cout << "   Simulating road block at Edge (3 -> 4)  " << endl;
    demoMap.edges[5].setBlocked(true);
    demoMap.edges[5].printRoad();
    return 0;
}