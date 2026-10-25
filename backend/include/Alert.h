
#ifndef ALERT_H
#define ALERT_H
#include <string>
using namespace std;
enum class AlertType
{
    FLOOD,
    HEAVY_RAIN,
    ROAD_BLOCK,
    LANDSLIDE,
    OTHER
};
enum class AlertSeverity
{
    INFO,
    MODERATE,
    HIGH,
    CRITICAL
};

enum class AlertSource
{
    AUTOMATIC,
    MANUAL
};

class Alert
{
private:
    string id;
    string roadId;
    string message;
    AlertType type;
    AlertSeverity severity;
    AlertSource source;
    bool active;

public:
    Alert();

    Alert(string id, string roadId, string message,
          AlertType type, AlertSeverity severity,
          AlertSource source);

    void display() const;

    string getId() const;
    string getRoadId() const;
    string getMessage() const;

    AlertType getType() const;
    AlertSeverity getSeverity() const;
    AlertSource getSource() const;

    bool isActive() const;
    void resolve();
};
#endif
