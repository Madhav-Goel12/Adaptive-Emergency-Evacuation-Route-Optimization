#include "priorityqueue.h"
void PriorityQueue::push(int node,int cost)
{
PQNode x;
x.node=node;
x.cost=cost;

q.push_back(x);

int i=q.size()-1;

while(i>0)
{
int p=(i-1)/2;

if(q[p].cost<=q[i].cost)
break;

swap(q[p],q[i]);
i=p;
}
}

PQNode PriorityQueue::pop()
{
PQNode x=q[0];

q[0]=q.back();
q.pop_back();

int i=0;

while(true)
{
int l=2*i+1;
int r=2*i+2;
int s=i;
if(l<q.size()&&q[l].cost<q[s].cost)
s=l;
if(r<q.size()&&q[r].cost<q[s].cost)
s=r;
if(s==i)
break;
swap(q[i],q[s]);
i=s;
}
return x;
}
bool PriorityQueue::empty()
{
return q.empty();
}