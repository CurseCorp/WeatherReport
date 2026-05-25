// src/model/entities/WeatherData.h
#pragma once
#include <string>

struct WeatherData {
    std::string cityName;
    double temperature;
    int humidity;
    std::string description;
};
