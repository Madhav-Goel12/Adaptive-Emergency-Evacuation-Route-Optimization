#include "APIClient.h"
#include <curl/curl.h>
#include <cstdlib>

using namespace std;

APIClient::APIClient()
{
    temp=0;
    rain=0;
    wind=0;
    weatherCode=0;
}

size_t writeData(void *ptr,size_t size,size_t nmemb,void *data)
{
    string *str=(string*)data;
    str->append((char*)ptr,size*nmemb);
    return size*nmemb;
}
bool APIClient::getData(double lat,double lon)
{
    CURL *curl;
    CURLcode res;
    string data;
    curl=curl_easy_init();
    if(curl==NULL)
    {
        cout<<"Curl initialization failed"<<endl;
        return false;
    }
    string url=
    "https://api.open-meteo.com/v1/forecast?latitude="+
    to_string(lat)+
    "&longitude="+
    to_string(lon)+
    "&current=temperature_2m,precipitation,wind_speed_10m,weather_code";
    curl_easy_setopt(curl,CURLOPT_URL,url.c_str());
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,writeData);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,&data);
    curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);
    curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER,1L);
    res=curl_easy_perform(curl);
    if(res!=CURLE_OK)
    {
        cout<<"API request failed: "
            <<curl_easy_strerror(res)<<endl;

        curl_easy_cleanup(curl);
        return false;
    }
    curl_easy_cleanup(curl);
    size_t p;
    p=data.find("\"temperature_2m\":");
    if(p!=string::npos)
    {
        p=data.find(":",p);
        temp=atof(data.substr(p+1).c_str());
    }
    p=data.find("\"precipitation\":");
    if(p!=string::npos)
    {
        p=data.find(":",p);
        rain=atof(data.substr(p+1).c_str());
    }
    p=data.find("\"wind_speed_10m\":");
    if(p!=string::npos)
    {
        p=data.find(":",p);
        wind=atof(data.substr(p+1).c_str());
    }
    p=data.find("\"weather_code\":");
    if(p!=string::npos)
    {
        p=data.find(":",p);
        weatherCode=atoi(data.substr(p+1).c_str());
   }
    return true;
}
int APIClient::getStatus()
{
    if(rain>=10 || wind>=60)
    return 3;
    if(rain>=5 || wind>=40)
        return 4;
    if(rain>=2 || wind>=25)
        return 2;

    return 1;
}
void APIClient::showData()
{
    cout<<"Temperature: "<<temp<<" C"<<endl;
    cout<<"Rainfall: "<<rain<<" mm"<<endl;
    cout<<"Wind Speed: "<<wind<<" km/h"<<endl;
    cout<<"Weather Code: "<<weatherCode<<endl;
    cout<<"Current Status: ";
    int st=getStatus();
    if(st==1)
        cout<<"Safe"<<endl;
    else if(st==2)
        cout<<"Risky"<<endl;
    else if(st==3)
        cout<<"Blocked"<<endl;
    else if(st==4)
        cout<<"Congested"<<endl;
}