
#ifndef OSM_GRAPH_LOADER_H
#define OSM_GRAPH_LOADER_H

#include <string>
#include <vector>

#include "Graph.h"

class OSMGraphLoader
{
public:
    static bool loadFromOSM(
        const std::string& filePath,
        Graph& graph
    );

    static bool loadFromFile(
        const std::string& filename,
        Graph& graph,
        std::vector<RoadFeature>& features
    );
};

#endif
