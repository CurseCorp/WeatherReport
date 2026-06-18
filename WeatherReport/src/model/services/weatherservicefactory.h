#ifndef WEATHERSERVICEFACTORY_H
#define WEATHERSERVICEFACTORY_H

#include <memory>
#include <QString>
#include "IWeatherService.h"

enum class WeatherProvider {
    WeatherAPI,
    OpenWeatherMap
};

class WeatherServiceFactory
{
public:
    static std::unique_ptr<IWeatherService> create(WeatherProvider provider);
    static WeatherProvider detectProvider(const QString &apiKey);
};

#endif // WEATHERSERVICEFACTORY_H
