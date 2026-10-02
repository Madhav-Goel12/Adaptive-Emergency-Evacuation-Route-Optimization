#ifndef DISASTER_MANAGER_H
#define DISASTER_MANAGER_H

#include "Graph.h"
#include "Road.h"

class DisasterManager
{
public:

    void updateRoad(Graph &g,int u,int v,int st)
    {
        for(int i=0;i<g.roads.size();i++)
        {
            if((g.roads[i].src==u&&g.roads[i].des==v)||
               (g.roads[i].src==v&&g.roads[i].des==u))
            {
                g.roads[i].sts=st;

                if(st==1)
                {
                    g.roads[i].cost=g.roads[i].dis;
                }
                else if(st==2)
                {
                    g.roads[i].cost=g.roads[i].dis*2;
                }
                else if(st==3)
                {
                    g.roads[i].cost=9999;
                }
                else if(st==4)
                {
                    g.roads[i].cost=g.roads[i].dis*3;
                }

                return;
            }
        }
    }

    void showRoad(Graph &g)
    {
        for(int i=0;i<g.roads.size();i++)
        {
            cout<<g.roads[i].src<<" -> ";
            cout<<g.roads[i].des<<" ";

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
};

#endif
