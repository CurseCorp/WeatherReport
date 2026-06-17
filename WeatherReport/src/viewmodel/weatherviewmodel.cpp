#include "WeatherViewModel.h"
#include "CacheManager.h"
#include <QSettings>
WeatherViewModel::WeatherViewModel(std::shared_ptr<WeatherApi> service, QObject *parent)
    : QObject(parent)
    , m_modelService(service)
{
    loadFavoritesFromConfig();
}
namespace {
const QString ORGANIZATION = "CurseCorp";
const QString APPLICATION  = "WeatherReport";
const QString KEY_FAVORITES = "favorites";
}
void WeatherViewModel::loadWeather(const QString& city) {
    if (city.isEmpty()) return;

    WeatherData data;

    if (CacheManager::load(city, data)) {
        updateUIData(data);
        return;
    }

    data = m_modelService->getWeatherByCityName(city);

    CacheManager::save(city, data);
    updateUIData(data);

}
void WeatherViewModel::loadFavoritesFromConfig()
{
    QSettings settings(ORGANIZATION, APPLICATION);
    m_favoriteCities = settings.value(KEY_FAVORITES).toStringList();
    emit favoriteCitiesChanged();
}

void WeatherViewModel::addCityToFavorites(const QString &city)
{
    QString trimmedCity = city.trimmed();
    if (trimmedCity.isEmpty() || m_favoriteCities.contains(trimmedCity)) {
        return;
    }
    m_favoriteCities.append(trimmedCity);
    QSettings settings(ORGANIZATION, APPLICATION);
    settings.setValue(KEY_FAVORITES, m_favoriteCities);

    emit favoriteCitiesChanged();
}


void WeatherViewModel::removeCityFromFavorites(const QString &city)
{
    QString trimmedCity = city.trimmed();
    if (!m_favoriteCities.contains(trimmedCity)) return;

    m_favoriteCities.removeAll(trimmedCity);
    QSettings settings(ORGANIZATION, APPLICATION);
    settings.setValue(KEY_FAVORITES, m_favoriteCities);

    emit favoriteCitiesChanged();
}

void WeatherViewModel::updateUIData(const WeatherData &data) {
    m_cityNameText = data.cityName;
    m_tempText = QString::number(data.temperatureCurrent, 'f', 1) + " °C";
    m_humidity = QString::number(data.humidity) + " %";
    m_description = data.description;
    m_windSpeed = QString::number(data.windSpeedMs) + "М/С";
    m_pressure = QString::number(data.pressure) + "мм рт.ст.";
    m_precipitation = QString::number(data.precipitationMm) + "ММ";
    m_minTemp = QString::number(data.temperatureMin, 'f', 1) + " °C";
    m_maxTemp = QString::number(data.temperatureMax, 'f', 1) + " °C";
    emit weatherUpdated();
}
