#include "Road.h"
#include <iostream>

Road::Road()
    : u(-1), v(-1), travelTime(0.0),
      isBlocked(false), hazardLevel(1.0), name("") {}

Road::Road(int src, int dest, double time,
           double hazard, const string& roadName)
    : u(src), v(dest), travelTime(time),
      isBlocked(false), hazardLevel(hazard), name(roadName) {}

int Road::getSrc() const { return u; }
int Road::getDest() const { return v; }
double Road::getTime() const { return travelTime; }
bool Road::getBlockedStatus() const { return isBlocked; }

void Road::setBlocked(bool status) { isBlocked = status; }
void Road::setHazardLevel(double hazard) { hazardLevel = hazard; }

double Road::getCost() const {
    return isBlocked ? 999999.0 : travelTime * hazardLevel;
}

string Road::getName() const { return name; }
void Road::setName(const string& roadName) { name = roadName; }

void Road::printRoad() {
    cout << "Edge (" << u << " -> " << v << ")"
         << " | Time: " << travelTime
         << " mins | Blocked: "
         << (isBlocked ? "True" : "False")
         << " | Name: " << name
         << " | Dynamic Weight: " << getCost()
         << endl;
}