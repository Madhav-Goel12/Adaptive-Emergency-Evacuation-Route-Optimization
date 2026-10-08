#ifndef DIJKSTRA_H
#define DIJKSTRA_H
#include "Graph.h"
#include "priorityqueue.h"
#include <vector>
using namespace std;
vector<int> dijkstra(Graph &g,int src,int des,int &total);
#endif