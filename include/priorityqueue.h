#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H
#include <vector>
using namespace std;
struct PQNode
{
int node;
int cost;
};
class PriorityQueue
{
private:
vector<PQNode> q;
public:
void push(int node,int cost);
PQNode pop();
bool empty();
};
#endif