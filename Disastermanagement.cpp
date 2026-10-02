#include <iostream>
using namespace std;88
void disUp(int g[20][20],int n)
{
int a,b,t;

cout<<"Enter road:";
cin>>a>>b;

cout<<"1. Safe"<<endl;
cout<<"2. Risky"<<endl;
cout<<"3. Blocked"<<endl;
cout<<"4. Congested"<<endl;
cout<<"Enter type: ";
cin>>t;

if(t==1)
{
cout<<"Road is safe"<<endl;
}
else if(t==2)
{
g[a][b]=g[a][b]*2;
g[b][a]=g[b][a]*2;
cout<<"Road is risky"<<endl;
}
else if(t==3)
{
g[a][b]=9999;
g[b][a]=9999;
cout<<"Road is blocked"<<endl;
}
else if(t==4)
{
g[a][b]=g[a][b]*3;
g[b][a]=g[b][a]*3;
cout<<"Road is congested"<<endl;
}
else
{
cout<<"Wrong input"<<endl;
}
}
void disShow(int g[20][20],int n)
{
cout<<endl;
cout<<"Updated Road Map:"<<endl;
for(int i=0;i<n;i++)
{
for(int j=0;j<n;j++)
{
if(g[i][j]!=0)
cout<<i<<" -> "<<j<<" : "<<g[i][j]<<endl;
}
}
}