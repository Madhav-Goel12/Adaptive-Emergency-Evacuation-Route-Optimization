#include "Road.h"

Road::Road() 
{
    u = -1;
    v = -1;
    travelTime = 0.0;
    isBlocked = false;
    hazardLevel = 1.0;
}
Road::Road(int src, int dest, double time, double hazard)
 {
    u = src;
    v = dest;
    travelTime = time;
    isBlocked = false;
    hazardLevel = hazard;
}
int Road::getSrc() 
{
    return u;
}
int Road::getDest() 
{
    return v;
}

double Road::getTime() 
{
    return travelTime;
}

bool Road::getBlockedStatus() 
{
    return isBlocked;
}

void Road::setBlocked(bool status) 
{
    isBlocked = status;
}

void Road::setHazardLevel(double hazard) 
{
    hazardLevel = hazard;
}


double Road::getCost()
 {
    if (isBlocked)
     {
        return 999999.0;
    }
    return travelTime * hazardLevel;
}

void Road::printRoad() 
{
    cout << "Edge (" << u << " -> " << v << ")"
         << " | Time: " << travelTime << " mins"
         << " | Blocked: " << (isBlocked ? "True" : "False")
         << " | Dynamic Weight: " << getCost()
         << endl;
}