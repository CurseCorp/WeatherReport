#include "WeatherViewModel.h"
#include "CacheManager.h"
#include <QSettings>
#include <QDebug>

WeatherViewModel::WeatherViewModel(std::shared_ptr<WeatherApi> service, QObject *parent)
    : QObject(parent)
    , m_modelService(service)
{
    loadFavoritesFromConfig();
}

void WeatherViewModel::loadWeather(const QString& city) {
    if (city.isEmpty()) return;
    WeatherData cachedData = CacheManager::load(city, "forecast");

    if (cachedData.isValid) {
        qDebug() << "--- УСПЕШНО: Данные взяты из кэша для:" << city;
        updateUIData(cachedData);
    } else {
        qDebug() << "--- Кэш не найден или поврежден, делаем сетевые запросы...";

        WeatherData current = m_modelService->getCurrentWeather(city);
        QVector<DailyForecast> forecast = m_modelService->getForecast3Days(city);

        if (current.isValid && !forecast.isEmpty()) {
            current.dailyForecasts = forecast;
            CacheManager::save(city, "forecast", current);

            updateUIData(current);
        } else {
            qDebug() << "--- ОШИБКА: Не удалось получить данные из сети!";
        }
    }
}

void WeatherViewModel::updateUIData(const WeatherData &data) {
    if (data.dailyForecasts.isEmpty()) return;

    const auto &today = data.dailyForecasts[0];

    m_cityNameText = data.cityName;

    m_tempText = QString::number(data.temperatureCurrent, 'f', 1) + " °C";
    m_humidity = QString::number(data.humidity) + " %";
    m_description = data.description;
    m_windSpeed = QString::number(data.windSpeedMs) + " м/с";
    m_pressure = QString::number(data.pressure) + " мм рт.ст.";
    m_precipitation = QString::number(today.precipitationMm) + " мм";
    m_minTemp = QString::number(today.tempMin, 'f', 1) + " °C";
    m_maxTemp = QString::number(today.tempMax, 'f', 1) + " °C";

    emit weatherUpdated();

    QVariantList newModelData;
    for (const auto &day : data.dailyForecasts) {
        QVariantMap dayMap;
        dayMap["date"] = day.date;
        dayMap["tempMin"] = day.tempMin;
        dayMap["tempMax"] = day.tempMax;
        dayMap["humidity"] = day.humidity;
        dayMap["pressure"] = day.pressure;
        dayMap["description"] = day.description;

        QVariantList hourlyList;
        for (const auto &hour : day.hourly) {
            QVariantMap hourMap;
            hourMap["time"] = hour.time;
            hourMap["temp"] = hour.temp;
            hourMap["icon"] = hour.icon;
            hourlyList.append(hourMap);
        }
        dayMap["hourly"] = hourlyList;
        newModelData.append(dayMap);
    }

    m_forecastModel = newModelData;
    emit forecastModelChanged();
}
void WeatherViewModel::loadFavoritesFromConfig() {
    QSettings settings("CurseCorp", "WeatherReport");
    m_favoriteCities = settings.value("favorites").toStringList();
    emit favoriteCitiesChanged();
}

void WeatherViewModel::addCityToFavorites(const QString &city) {
    QString trimmedCity = city.trimmed();
    if (trimmedCity.isEmpty() || m_favoriteCities.contains(trimmedCity)) return;
    m_favoriteCities.append(trimmedCity);
    QSettings("CurseCorp", "WeatherReport").setValue("favorites", m_favoriteCities);
    emit favoriteCitiesChanged();
}

void WeatherViewModel::removeCityFromFavorites(const QString &city) {
    QString trimmedCity = city.trimmed();
    if (!m_favoriteCities.contains(trimmedCity)) return;
    m_favoriteCities.removeAll(trimmedCity);
    QSettings("CurseCorp", "WeatherReport").setValue("favorites", m_favoriteCities);
    emit favoriteCitiesChanged();
}
