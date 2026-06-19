#include "weatherapiservice.h"

WeatherData WeatherApiService::getCurrentWeather(double latitude, double longitude)
{
    QString coordinates = QString("%1,%2").arg(latitude, 0, 'f', 6).arg(longitude, 0, 'f', 6);
    return m_api.getCurrentWeather(coordinates);
}

QVector<DailyForecast> WeatherApiService::getForecast3Days(double latitude, double longitude)
{
    QString coordinates = QString("%1,%2").arg(latitude, 0, 'f', 6).arg(longitude, 0, 'f', 6);
    return m_api.getForecast3Days(coordinates);
}
