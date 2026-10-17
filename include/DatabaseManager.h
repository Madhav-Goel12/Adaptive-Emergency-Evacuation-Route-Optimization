```cpp
#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>
#include <vector>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/collection.hpp>

#include <bsoncxx/document/value.hpp>

class DatabaseManager {
private:
    // The MongoDB driver instance must outlive the client.
    static mongocxx::instance instance;
    mongocxx::client client;
    mongocxx::database database;

public:
    DatabaseManager(
        const std::string& connectionString,
        const std::string& databaseName
    );

    bool ping();

    bool saveLocation(
        int id,
        const std::string& name,
        double latitude,
        double longitude
    );

    bool saveRoad(
        int roadId,
        int fromLocation,
        int toLocation,
        double distance,
        bool blocked
    );

    bool saveEvacuationLog(
        const std::string& eventType,
        const std::string& details
    );

    std::vector<bsoncxx::document::value> getAllLocations();

    std::vector<bsoncxx::document::value> getAllRoads();

    bool deleteLocation(int id);
};

#endif
```