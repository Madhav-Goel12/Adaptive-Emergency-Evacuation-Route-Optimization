#include "Location.h"

Location::Location() {
    locationId = -1;
    locationName = "";
    isSafeZone = false;
}

Location::Location(int id, string name, bool safe) {
    locationId = id;
    locationName = name;
    isSafeZone = safe;
}

int Location::getId() {
    return locationId;
}

string Location::getName() {
    return locationName;
}

bool Location::checkSafeZone() {
    return isSafeZone;
}

void Location::setSafeZone(bool status) {
    isSafeZone = status;
}

void Location::printInfo() {
    cout << "ID: " << locationId
         << " | Name: " << locationName
         << " | Safe Zone: " << (isSafeZone ? "YES" : "NO")
         << endl;
}