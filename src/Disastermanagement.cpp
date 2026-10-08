#include "DisasterManager.h"
void DisasterManager::updateRoad(Graph &g,int u,int v,int st)
{
int n=g.roads.size();
for(int i=0;i<n;i++)
{
if(g.roads[i].src==u&&g.roads[i].des==v)
{
g.roads[i].sts=st;
if(st==1)
g.roads[i].cost=g.roads[i].dis;
else if(st==2)
g.roads[i].cost=g.roads[i].dis*2;
else if(st==3)
g.roads[i].cost=9999;
else if(st==4)
g.roads[i].cost=g.roads[i].dis*3;
return;
}
if(g.roads[i].src==v&&g.roads[i].des==u)
{
g.roads[i].sts=st;
if(st==1)
g.roads[i].cost=g.roads[i].dis;
else if(st==2)
g.roads[i].cost=g.roads[i].dis*2;
else if(st==3)
g.roads[i].cost=9999;
else if(st==4)
g.roads[i].cost=g.roads[i].dis*3;
return;
}
}
cout<<"Road not found"<<endl;
}
void DisasterManager::showRoad(Graph &g)
{
int n=g.roads.size();
for(int i=0;i<n;i++)
{
cout<<g.roads[i].src<<" -> "<<g.roads[i].des<<" ";
if(g.roads[i].sts==1)
cout<<"Safe";
else if(g.roads[i].sts==2)
cout<<"Risky";
else if(g.roads[i].sts==3)
cout<<"Blocked";
else if(g.roads[i].sts==4)
cout<<"Congested";
cout<<" Cost: "<<g.roads[i].cost<<endl;
}
}