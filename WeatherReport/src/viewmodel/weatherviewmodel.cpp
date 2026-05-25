
#include "WeatherViewModel.h"

WeatherViewModel::WeatherViewModel(std::shared_ptr<WeatherService> service, QObject *parent)
    : QObject(parent)
    , m_modelService(service)
{

}

void WeatherViewModel::loadWeather(const QString& city) {
    if (city.isEmpty()) return;

    WeatherData rawData = m_modelService->fetchCurrentWeather(city.toStdString());

    m_cityNameText = QString::fromStdString(rawData.cityName);
    m_tempText = QString::number(rawData.temperature, 'f', 1) + " °C";
    m_humidity = QString::number(rawData.humidity) + " %";
    m_description = QString::fromStdString(rawData.description);

    emit weatherUpdated();
}
