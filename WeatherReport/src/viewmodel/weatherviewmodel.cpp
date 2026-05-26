
#include "WeatherViewModel.h"
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

   WeatherData rawData = m_modelService->getWeatherByCityName(city); //();

    m_cityNameText = rawData.cityName;
    m_tempText = QString::number(rawData.temperatureCurrent, 'f', 1) + " °C";
    m_humidity = QString::number(rawData.humidity) + " %";
    m_description =rawData.description;
    m_windSpeed = QString::number(rawData.windSpeedMs) + "М/С";
    m_pressure = QString::number(rawData.pressure) + "мм рт.ст.";


    emit weatherUpdated();
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
