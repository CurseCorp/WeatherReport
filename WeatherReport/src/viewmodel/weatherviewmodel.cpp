#include "WeatherViewModel.h"
#include "CacheManager.h"
#include <QSettings>
#include <QDebug>

WeatherViewModel::WeatherViewModel(std::shared_ptr<WeatherApi> service, QObject *parent)
    : QObject(parent)
    , m_modelService(service)
{
    loadFavoritesFromConfig();
    loadFavoriteTemps();
    loadWeather("Москва");
}

void WeatherViewModel::loadWeather(const QString& city) {
    if (city.isEmpty()) return;
    bool isExpired = CacheManager::isCacheExpired(city);
    WeatherData cachedData = CacheManager::load(city, "forecast");

    if (cachedData.isValid && !isExpired) {
        qDebug() << "--- УСПЕШНО: Данные взяты из свежего кэша для:" << city;
        updateUIData(cachedData);
        return;
    }
    qDebug() << (isExpired ? "--- Кэш устарел, обновляем из сети..."
                           : "--- Кэш не найден, делаем сетевые запросы...");

    WeatherData current = m_modelService->getCurrentWeather(city);
    QVector<DailyForecast> forecast = m_modelService->getForecast3Days(city);

    if (current.isValid && !forecast.isEmpty()) {
        current.dailyForecasts = forecast;

        CacheManager::save(city, "forecast", current);
        CacheManager::updateCacheTimestamp(city);

        updateUIData(current);
    } else {
        if (cachedData.isValid) {
            qDebug() << "--- ОШИБКА СЕТИ: Используем старый кэш в качестве резерва.";
            updateUIData(cachedData);
        } else {
            qDebug() << "--- ОШИБКА: Не удалось получить данные ни из сети, ни из кэша!";
        }
    }
}

void WeatherViewModel::updateUIData(const WeatherData &data) {
    if (data.dailyForecasts.isEmpty()) return;

    const auto &today = data.dailyForecasts[0];

    m_cityNameText = data.cityName;
CacheManager::saveHistory(data.cityName, QDate::currentDate(), data);
    m_tempText = QString::number(data.temperatureCurrent, 'f', 1);
    m_humidity = QString::number(data.humidity) + " %";
    m_description = data.description;
    m_windSpeed = QString::number(data.windSpeedMs);
    m_pressure = QString::number(data.pressure) + " мм рт.ст.";
    m_precipitation = QString::number(today.precipitationMm) + " мм";
    m_minTemp = QString::number(today.tempMin, 'f', 1);
    m_maxTemp = QString::number(today.tempMax, 'f', 1);

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
    loadWeather(city);
    loadFavoriteTemps();
    emit favoriteCitiesChanged();
}

void WeatherViewModel::removeCityFromFavorites(const QString &city) {
    QString trimmedCity = city.trimmed();
    if (!m_favoriteCities.contains(trimmedCity)) return;
    m_favoriteCities.removeAll(trimmedCity);
    QSettings("CurseCorp", "WeatherReport").setValue("favorites", m_favoriteCities);
    emit favoriteCitiesChanged();
}
void WeatherViewModel::loadFavoriteTemps() {
    QSettings settings("CurseCorp", "WeatherReport");
    m_favoriteCities = settings.value("favorites").toStringList();
    m_favoriteCityTemps.clear();

    for (const QString &city : m_favoriteCities) {
        WeatherData cached = CacheManager::load(city, "forecast");

        if (cached.isValid) {
            m_favoriteCityTemps[city] = QString::number(cached.temperatureCurrent, 'f', 1);
        } else {

            m_favoriteCityTemps[city] = "--";

            WeatherData data = m_modelService->getCurrentWeather(city);
            if (data.isValid) {
                m_favoriteCityTemps[city] = QString::number(data.temperatureCurrent, 'f', 1);
                CacheManager::save(city, "forecast", data);
            }
        }
    }

    emit favoriteCityTempsChanged();
}
void WeatherViewModel::loadHistory(const QString &city) {
    m_historyData.clear();
    QDate today = QDate::currentDate();

    for (int i = 1; i <= 7; ++i) {
        QDate targetDate = today.addDays(-i);
        WeatherData data = CacheManager::loadHistory(city, targetDate);

        if (data.isValid) {
            QVariantMap map;
            map["date"] = targetDate.toString("dd.MM");
            if (!data.dailyForecasts.isEmpty()) {
                const auto &day = data.dailyForecasts[0];
                map["tempMax"] = day.tempMax;
                map["tempMin"] = day.tempMin;
                map["description"] = day.description;
            }

            m_historyData.append(map);
        }
    }
    emit historyDataChanged();
}
void WeatherViewModel::refreshWeather(const QString &city) {
    qDebug() << "--- Принудительное обновление для:" << city;
    WeatherData current = m_modelService->getCurrentWeather(city);
    QVector<DailyForecast> forecast = m_modelService->getForecast3Days(city);

    if (current.isValid && !forecast.isEmpty()) {
        current.dailyForecasts = forecast;
        CacheManager::save(city, "forecast", current);
        updateUIData(current);
        qDebug() << "--- УСПЕШНО: Данные обновлены из сети.";
    } else {
        qDebug() << "--- ОШИБКА: Сеть недоступна, берем данные из кэша...";
        loadWeather(city);
    }
}
