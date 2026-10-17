```cpp
#include "DatabaseManager.h"

#include <chrono>
#include <iostream>
#include <stdexcept>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/types.hpp>

#include <mongocxx/exception/exception.hpp>
#include <mongocxx/options/replace.hpp>
#include <mongocxx/options/find.hpp>

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

mongocxx::instance DatabaseManager::instance{};

DatabaseManager::DatabaseManager(
    const std::string& connectionString,
    const std::string& databaseName
)
    : client(mongocxx::uri{connectionString}),
      database(client[databaseName]) {
}

bool DatabaseManager::ping() {
    try {
        database.run_command(
            make_document(kvp("ping", 1))
        );
        std::cout << "MongoDB Atlas connected successfully.\n";
        return true;
    } catch (const mongocxx::exception& e) {
        std::cerr << "MongoDB ping failed: "
                  << e.what() << '\n';
        return false;
    }
}

bool DatabaseManager::saveLocation(
    int id,
    const std::string& name,
    double latitude,
    double longitude
) {
    try {
        auto collection = database["locations"];

        auto filter = make_document(kvp("id", id));
        auto replacement = make_document(
            kvp("id", id),
            kvp("name", name),
            kvp("latitude", latitude),
            kvp("longitude", longitude)
        );

        mongocxx::options::replace options;
        options.upsert(true);

        collection.replace_one(
            filter.view(),
            replacement.view(),
            options
        );

        return true;
    } catch (const mongocxx::exception& e) {
        std::cerr << "Saving location failed: "
                  << e.what() << '\n';
        return false;
    }
}

bool DatabaseManager::saveRoad(
    int roadId,
    int fromLocation,
    int toLocation,
    double distance,
    bool blocked
) {
    try {
        auto collection = database["roads"];

        auto filter = make_document(kvp("roadId", roadId));
        auto replacement = make_document(
            kvp("roadId", roadId),
            kvp("fromLocation", fromLocation),
            kvp("toLocation", toLocation),
            kvp("distance", distance),
            kvp("blocked", blocked)
        );

        mongocxx::options::replace options;
        options.upsert(true);

        collection.replace_one(
            filter.view(),
            replacement.view(),
            options
        );

        return true;
    } catch (const mongocxx::exception& e) {
        std::cerr << "Saving road failed: "
                  << e.what() << '\n';
        return false;
    }
}

bool DatabaseManager::saveEvacuationLog(
    const std::string& eventType,
    const std::string& details
) {
    try {
        auto collection = database["evacuation_logs"];

        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<
            std::chrono::milliseconds
        >(now.time_since_epoch()).count();

        auto document = make_document(
            kvp("eventType", eventType),
            kvp("details", details),
            kvp("timestamp", bsoncxx::types::b_int64{
                static_cast<std::int64_t>(timestamp)
            })
        );

        collection.insert_one(document.view());
        return true;
    } catch (const mongocxx::exception& e) {
        std::cerr << "Saving evacuation log failed: "
                  << e.what() << '\n';
        return false;
    }
}

std::vector<bsoncxx::document::value>
DatabaseManager::getAllLocations() {
    std::vector<bsoncxx::document::value> results;

    try {
        auto collection = database["locations"];

        for (const auto& document : collection.find({})) {
            results.emplace_back(document);
        }
    } catch (const mongocxx::exception& e) {
        std::cerr << "Reading locations failed: "
                  << e.what() << '\n';
    }

    return results;
}

std::vector<bsoncxx::document::value>
DatabaseManager::getAllRoads() {
    std::vector<bsoncxx::document::value> results;

    try {
        auto collection = database["roads"];

        for (const auto& document : collection.find({})) {
            results.emplace_back(document);
        }
    } catch (const mongocxx::exception& e) {
        std::cerr << "Reading roads failed: "
                  << e.what() << '\n';
    }

    return results;
}

bool DatabaseManager::deleteLocation(int id) {
    try {
        auto collection = database["locations"];
        auto filter = make_document(kvp("id", id));

        auto result = collection.delete_one(filter.view());
        return result && result->deleted_count() > 0;
    } catch (const mongocxx::exception& e) {
        std::cerr << "Deleting location failed: "
                  << e.what() << '\n';
        return false;
    }
}
```