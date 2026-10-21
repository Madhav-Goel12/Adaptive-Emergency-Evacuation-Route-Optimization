#include "apiclient.h"

#include <curl/curl.h>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

static size_t writeData(void* ptr, size_t size, size_t nmemb, void* data)
{
    string* response = static_cast<string*>(data);
    response->append(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

APIClient::APIClient()
    : temp(0), rain(0), wind(0), weatherCode(0)
{
}

bool APIClient::getData(double lat, double lon)
{
    CURL* curl = curl_easy_init();

    if (!curl) {
        cerr << "Curl initialization failed.\n";
        return false;
    }

    string response;

    string url =
        "https://api.open-meteo.com/v1/forecast?latitude=" +
        to_string(lat) +
        "&longitude=" + to_string(lon) +
        "&current=temperature_2m,precipitation,wind_speed_10m,weather_code";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);

    CURLcode result = curl_easy_perform(curl);

    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curl);

    if (result != CURLE_OK || httpCode != 200) {
        cerr << "Weather API request failed: "
             << (result != CURLE_OK
                     ? curl_easy_strerror(result)
                     : "HTTP " + to_string(httpCode))
             << '\n';
        return false;
    }

    // Extract fields specifically from the "current" JSON object.
    size_t currentPos = response.find("\"current\"");
    if (currentPos == string::npos) {
        cerr << "API response has no current weather data.\n";
        return false;
    }

    size_t objectStart = response.find('{', currentPos);
    size_t objectEnd = response.find('}', objectStart);

    if (objectStart == string::npos || objectEnd == string::npos) {
        cerr << "Invalid current weather JSON object.\n";
        return false;
    }

    string current = response.substr(
        objectStart, objectEnd - objectStart + 1);

    auto readNumber = [&](const string& key, double& value) -> bool {
        size_t pos = current.find("\"" + key + "\"");
        if (pos == string::npos) return false;

        pos = current.find(':', pos);
        if (pos == string::npos) return false;

        const char* start = current.c_str() + pos + 1;
        while (*start == ' ' || *start == '\n' || *start == '\t')
            ++start;

        if (*start == '\0' || *start == 'n') return false;

        char* end = nullptr;
        double parsed = strtod(start, &end);

        if (end == start) return false;

        value = parsed;
        return true;
    };

    double newTemp, newRain, newWind, newCode;

    if (!readNumber("temperature_2m", newTemp) ||
        !readNumber("precipitation", newRain) ||
        !readNumber("wind_speed_10m", newWind) ||
        !readNumber("weather_code", newCode)) {
        cerr << "Could not parse all weather fields from API response.\n";
        return false;
    }

    temp = newTemp;
    rain = newRain;
    wind = newWind;
    weatherCode = static_cast<int>(newCode);

    return true;
}

int APIClient::getStatus()
{
    if (rain >= 10 || wind >= 60)
        return 3;
    if (rain >= 5 || wind >= 40)
        return 4;
    if (rain >= 2 || wind >= 25)
        return 2;

    return 1;
}

void APIClient::showData()
{
    cout << "Temperature: " << temp << " C\n";
    cout << "Rainfall: " << rain << " mm\n";
    cout << "Wind Speed: " << wind << " km/h\n";
    cout << "Weather Code: " << weatherCode << '\n';

    cout << "Current Status: ";
    switch (getStatus()) {
        case 1: cout << "Safe\n"; break;
        case 2: cout << "Risky\n"; break;
        case 3: cout << "Blocked\n"; break;
        case 4: cout << "Congested\n"; break;
    }
}