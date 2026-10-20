#include "priorityqueue.h"
#include <algorithm>
#include <stdexcept>

void PriorityQueue::push(int node, double cost)
{
    q.push_back({node, cost});
    int i = static_cast<int>(q.size()) - 1;

    while (i > 0) {
        int parent = (i - 1) / 2;
        if (q[parent].cost <= q[i].cost)
            break;
        std::swap(q[parent], q[i]);
        i = parent;
    }
}

PQNode PriorityQueue::pop()
{
    if (q.empty())
        throw std::out_of_range("PriorityQueue::pop called on empty queue");

    PQNode result = q.front();
    q.front() = q.back();
    q.pop_back();

    int i = 0;
    const int n = static_cast<int>(q.size());
    while (true) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;

        if (left < n && q[left].cost < q[smallest].cost)
            smallest = left;
        if (right < n && q[right].cost < q[smallest].cost)
            smallest = right;
        if (smallest == i)
            break;

        std::swap(q[i], q[smallest]);
        i = smallest;
    }
    return result;
}

bool PriorityQueue::empty()
{
    return q.empty();
}
