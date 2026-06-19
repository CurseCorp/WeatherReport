#ifndef WEATHERSERVICEMANAGER_H
#define WEATHERSERVICEMANAGER_H

#include <memory>
#include <QString>
#include "IWeatherService.h"
#include "weatherservicefactory.h"

class WeatherServiceManager
{
public:
    WeatherServiceManager();

    void setApiKey(const QString &apiKey);
    IWeatherService* getService();
    WeatherProvider getCurrentProvider() const;

private:
    std::unique_ptr<IWeatherService> m_service;
    WeatherProvider m_currentProvider = WeatherProvider::WeatherAPI;
    QString m_currentApiKey;

    void updateService();
};

#endif // WEATHERSERVICEMANAGER_H
