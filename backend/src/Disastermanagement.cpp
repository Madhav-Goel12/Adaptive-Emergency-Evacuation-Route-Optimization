
#include "DisasterManager.h"
#include <iostream>

using namespace std;

DisasterManager::DisasterManager()
{
    nextId = 1;
}

void DisasterManager::addAlert(string roadId, string message,
                               AlertType type, AlertSeverity severity,
                               AlertSource source)
{
    string id = "ALT" + to_string(nextId++);

    Alert a(id, roadId, message, type, severity, source);

    alerts.push_back(a);

    cout << "\nAlert created successfully!";
    cout << "\nAlert ID: " << id << endl;
}

void DisasterManager::showAlerts() const
{
    if (alerts.empty())
    {
        cout << "\nNo alerts available.\n";
        return;
    }

    for (const Alert &a : alerts)
    {
        a.display();
        cout << "----------------------\n";
    }
}

bool DisasterManager::resolveAlert(string alertId)
{
    for (Alert &a : alerts)
    {
        if (a.getId() == alertId && a.isActive())
        {
            a.resolve();
            cout << "\nAlert resolved successfully.\n";
            return true;
        }
    }

    cout << "\nActive alert not found.\n";
    return false;
}

vector<Alert> DisasterManager::getActiveAlerts() const
{
    vector<Alert> activeAlerts;

    for (const Alert &a : alerts)
    {
        if (a.isActive())
        {
            activeAlerts.push_back(a);
        }
    }

    return activeAlerts;
}
