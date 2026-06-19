#include "weatherservicemanager.h"
#include "openweathermapservice.h"
#include <QDebug>

WeatherServiceManager::WeatherServiceManager()
{
    m_service = WeatherServiceFactory::create(WeatherProvider::WeatherAPI);
    qDebug() << "WeatherServiceManager: WeatherAPI по умолчанию";
}

void WeatherServiceManager::setApiKey(const QString &apiKey)
{
    if (m_currentApiKey == apiKey) {
        return;
    }

    m_currentApiKey = apiKey;
    updateService();
}

void WeatherServiceManager::updateService()
{
    WeatherProvider newProvider;

    if (m_currentApiKey.isEmpty()) {
        newProvider = WeatherProvider::WeatherAPI;
        qDebug() << "WeatherServiceManager: ключ пуст → WeatherAPI";
    } else {
        newProvider = WeatherServiceFactory::detectProvider(m_currentApiKey);
        qDebug() << "WeatherServiceManager: определён провайдер по ключу";
    }

    if (m_currentProvider != newProvider) {
        m_currentProvider = newProvider;
        m_service = WeatherServiceFactory::create(newProvider);

        if (newProvider == WeatherProvider::OpenWeatherMap) {
            auto* owm = dynamic_cast<OpenWeatherMapService*>(m_service.get());
            if (owm) {
                owm->setApiKey(m_currentApiKey);
            }
        }

        qDebug() << "WeatherServiceManager: сервис обновлён на"
                 << (newProvider == WeatherProvider::WeatherAPI ? "WeatherAPI" : "OpenWeatherMap");
    }
}

IWeatherService* WeatherServiceManager::getService()
{
    return m_service.get();
}

WeatherProvider WeatherServiceManager::getCurrentProvider() const
{
    return m_currentProvider;
}
