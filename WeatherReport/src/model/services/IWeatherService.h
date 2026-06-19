#ifndef IWEATHERSERVICE_H
#define IWEATHERSERVICE_H

#include <QVector>
#include "src/model/entities/WeatherData.h"

class IWeatherService
{
public:
    virtual ~IWeatherService() = default;

    virtual WeatherData getCurrentWeather(double latitude, double longitude) = 0;
    virtual QVector<DailyForecast> getForecast3Days(double latitude, double longitude) = 0;
};

#endif // IWEATHERSERVICE_H
