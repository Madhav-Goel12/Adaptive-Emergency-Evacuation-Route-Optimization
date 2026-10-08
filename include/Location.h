#ifndef LOCATION_H
#define LOCATION_H

#include <iostream>
#include <string>
using namespace std;

class Location {
private:
    int locationId;
    string locationName;
    bool isSafeZone;

public:
    Location();
    Location(int id, string name, bool safe = false);

    int getId();
    string getName();
    bool checkSafeZone();
    void setSafeZone(bool status);
    void printInfo();
};

#endif