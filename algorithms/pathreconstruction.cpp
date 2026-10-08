#include "PathReconstruction.h"
#include <algorithm>
vector<int> getPath(vector<int> &par,int src,int des)
{
vector<int> path;
int cur=des;
while(cur!=-1)
{
path.push_back(cur);
if(cur==src)
break;
cur=par[cur];
}
if(path.back()!=src)
{
path.clear();
return path;
}
reverse(path.begin(),path.end());
return path;
}