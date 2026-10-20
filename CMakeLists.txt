cmake_minimum_required(VERSION 3.15)
project(AdaptiveEmergencyEvacuationRouteOptimization LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(mongocxx REQUIRED)
find_package(bsoncxx REQUIRED)
find_package(CURL REQUIRED)

add_executable(evacuation_app
    src/menu.cpp
    src/Location.cpp
    src/Road.cpp
    src/Graph.cpp
    src/RouteOptimizer.cpp
    src/Disastermanagement.cpp
    src/DatabaseManager.cpp
    src/apiclient.cpp
    src/DataLoader.cpp
    algorithms/priorityqueue.cpp
    algorithms/pathreconstruction.cpp
)

target_include_directories(evacuation_app PRIVATE include algorithms)
target_link_libraries(evacuation_app PRIVATE
    mongo::mongocxx_shared
    mongo::bsoncxx_shared
    CURL::libcurl
)
