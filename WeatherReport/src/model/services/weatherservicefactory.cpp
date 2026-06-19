#include "weatherservicefactory.h"
#include "weatherapiservice.h"
#include "openweathermapservice.h"
#include <QRegularExpression>

std::unique_ptr<IWeatherService> WeatherServiceFactory::create(WeatherProvider provider)
{
    switch (provider) {
    case WeatherProvider::WeatherAPI:
        return std::make_unique<WeatherApiService>();
    case WeatherProvider::OpenWeatherMap:
        return std::make_unique<OpenWeatherMapService>();
    default:
        return std::make_unique<WeatherApiService>();
    }
}

WeatherProvider WeatherServiceFactory::detectProvider(const QString &apiKey)
{
    if (apiKey.isEmpty()) {
        return WeatherProvider::WeatherAPI;
    }


    if (apiKey.length() == 32 && apiKey.contains(QRegularExpression("^[a-fA-F0-9]{32}$"))) {
        return WeatherProvider::OpenWeatherMap;
    }

    if (apiKey.startsWith("ow_") || apiKey.startsWith("openweather_")) {
        return WeatherProvider::OpenWeatherMap;
    }

    return WeatherProvider::WeatherAPI;
}
