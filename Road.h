#ifndef ROAD_H
#define ROAD_H

#include <iostream>
using namespace std;

class Road {
private:
    int u;
    int v;
    double travelTime;
    bool isBlocked;
    double hazardLevel;

public:
    Road();
    Road(int src, int dest, double time, double hazard = 1.0);

    int getSrc();
    int getDest();
    double getTime();
    bool getBlockedStatus();

    void setBlocked(bool status);
    void setHazardLevel(double hazard);

    double getCost();
    void printRoad();
};

#endif