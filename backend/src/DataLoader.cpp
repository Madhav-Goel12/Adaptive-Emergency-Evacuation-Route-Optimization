#include "Graph.h"
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <stdexcept>

using namespace std;

Graph loadGraphFromJSON(const string& filename)
{
    ifstream file(filename);

    if (!file.is_open())
    {
        throw runtime_error("Cannot open JSON file: " + filename);
    }

    string content;
    string line;

    while (getline(file, line))
    {
        content += line + "\n";
    }

    // Read total number of locations
    regex locationsPattern(
        R"("totalLocations"\s*:\s*(\d+))"
    );

    smatch match;

    if (!regex_search(content, match, locationsPattern))
    {
        throw runtime_error("totalLocations not found");
    }

    int totalLocations = stoi(match[1].str());

    if (totalLocations <= 0)
    {
        throw runtime_error("Invalid number of locations");
    }

    Graph graph(totalLocations);

    // Find the roads array
    size_t roadsStart = content.find("\"roads\"");

    if (roadsStart == string::npos)
    {
        throw runtime_error("Roads array not found");
    }

    size_t arrayStart = content.find('[', roadsStart);
    size_t arrayEnd = content.find(']', arrayStart);

    if (arrayStart == string::npos ||
        arrayEnd == string::npos)
    {
        throw runtime_error("Invalid roads array");
    }

    string roadsData =
        content.substr(arrayStart + 1,
                       arrayEnd - arrayStart - 1);

    // Read each road object
    regex roadPattern(R"(\{([^{}]*)\})");

    auto begin = sregex_iterator(
        roadsData.begin(), roadsData.end(), roadPattern
    );

    auto end = sregex_iterator();

    regex srcPattern(R"("src"\s*:\s*(\d+))");
    regex destPattern(R"("dest"\s*:\s*(\d+))");
    regex timePattern(R"("time"\s*:\s*([0-9]+(?:\.[0-9]+)?))");
    regex hazardPattern(R"("hazard"\s*:\s*([0-9]+(?:\.[0-9]+)?))");
    regex blockedPattern(R"("blocked"\s*:\s*(true|false))");

    for (auto it = begin; it != end; ++it)
    {
        string roadData = (*it)[1].str();

        smatch srcMatch, destMatch, timeMatch;
        smatch hazardMatch, blockedMatch;

        if (!regex_search(roadData, srcMatch, srcPattern) ||
            !regex_search(roadData, destMatch, destPattern) ||
            !regex_search(roadData, timeMatch, timePattern))
        {
            throw runtime_error("Invalid road data");
        }

        int src = stoi(srcMatch[1].str());
        int dest = stoi(destMatch[1].str());

        double time = stod(timeMatch[1].str());
        double hazard = 1.0;
        bool blocked = false;

        if (regex_search(roadData, hazardMatch, hazardPattern))
            hazard = stod(hazardMatch[1].str());

        if (regex_search(roadData, blockedMatch, blockedPattern))
            blocked = (blockedMatch[1].str() == "true");

        if (src < 0 || src >= totalLocations ||
            dest < 0 || dest >= totalLocations ||
            time < 0 || hazard < 0)
        {
            throw runtime_error("Invalid road values");
        }

        Road road(src, dest, time, hazard);
        road.setBlocked(blocked);

        graph.addRoad(road);
    }

    return graph;
}

