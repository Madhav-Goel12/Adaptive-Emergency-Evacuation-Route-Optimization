#ifndef ROAD_H
#define ROAD_H

#include <string>
using namespace std;

class Road {
private:
    int u;
    int v;
    double travelTime;
    bool isBlocked;
    double hazardLevel;
    string name;

public:
    Road();
    Road(int src, int dest, double time,
         double hazard = 1.0,
         const string& roadName = "");

    int getSrc() const;
    int getDest() const;
    double getTime() const;
    bool getBlockedStatus() const;
    void setBlocked(bool status);
    void setHazardLevel(double hazard);
    double getCost() const;
    void printRoad();

    string getName() const;
    void setName(const string& roadName);
};

#endif