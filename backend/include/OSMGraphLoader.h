#ifndef OSM_GRAPH_LOADER_H
#define OSM_GRAPH_LOADER_H

#include <string>
#include "Graph.h"

class OSMGraphLoader
{
public:
    static bool loadFromOSM(
        const std::string& filePath,
        Graph& graph
    );
};

#endif