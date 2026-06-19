#ifndef WEATHERAPISERVICE_H
#define WEATHERAPISERVICE_H

#include "IWeatherService.h"
#include "src/model/services/weatherapi.h"

class WeatherApiService : public IWeatherService
{
public:
    WeatherData getCurrentWeather(double latitude, double longitude) override;
    QVector<DailyForecast> getForecast3Days(double latitude, double longitude) override;

private:
    WeatherApi m_api;
};

#endif // WEATHERAPISERVICE_H
