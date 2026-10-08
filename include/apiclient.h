#ifndef APICLIENT_H
#define APICLIENT_H

#include <iostream>
#include <string>
using namespace std;

class APIClient
{
public:
    double temp;
    double rain;
    double wind;
    int weatherCode;

    APIClient();

    bool getData(double lat,double lon);
    int getStatus();
    void showData();
};

#endif