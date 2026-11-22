#ifndef APICLIENT_H
#define APICLIENT_H

#include <string>

class APIClient {
public:
    double temp;
    double rain;
    double wind;
    int weatherCode;
    double humidity;
    std::string observationTime;

    APIClient();
    bool getData(double lat, double lon);
    int getStatus();
    void showData();
};

#endif
