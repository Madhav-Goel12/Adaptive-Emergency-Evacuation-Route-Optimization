
#include "Alert.h"
#include <iostream>

using namespace std;

Alert::Alert()
{
    id = "";
    roadId = "";
    message = "";
    type = AlertType::OTHER;
    severity = AlertSeverity::INFO;
    source = AlertSource::MANUAL;
    active = true;
}

Alert::Alert(string id, string roadId, string message,
             AlertType type, AlertSeverity severity,
             AlertSource source)
{
    this->id = id;
    this->roadId = roadId;
    this->message = message;
    this->type = type;
    this->severity = severity;
    this->source = source;
    active = true;
}

void Alert::display() const
{
    cout << "\nAlert ID: " << id;
    cout << "\nRoad ID: " << roadId;
    cout << "\nMessage: " << message;
    cout << "\nActive: " << (active ? "Yes" : "No") << endl;
}

string Alert::getId() const { return id; }
string Alert::getRoadId() const { return roadId; }
string Alert::getMessage() const { return message; }

AlertType Alert::getType() const { return type; }
AlertSeverity Alert::getSeverity() const { return severity; }
AlertSource Alert::getSource() const { return source; }

bool Alert::isActive() const { return active; }

void Alert::resolve()
{
    active = false;
}
