
#ifndef DISASTERMANAGER_H
#define DISASTERMANAGER_H

#include "Alert.h"
#include <vector>
#include <string>

using namespace std;

class DisasterManager
{
private:
    vector<Alert> alerts;
    int nextId;

public:
    DisasterManager();

    void addAlert(string roadId, string message,
                  AlertType type, AlertSeverity severity,
                  AlertSource source);

    void showAlerts() const;

    bool resolveAlert(string alertId);

    vector<Alert> getActiveAlerts() const;
};

#endif
