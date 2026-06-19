#include "WeatherViewModel.h"
#include "CacheManager.h"
#include "../model/services/weatherservicemanager.h"
#include "../model/entities/City.h"
#include "../model/entities/WeatherData.h"
#include <QSettings>
#include <QDebug>
#include <QDate>

WeatherViewModel::WeatherViewModel(std::shared_ptr<WeatherApi> service,
                                   std::shared_ptr<GeocodingService> geoService,
                                   QObject *parent)
    : QObject(parent)
    , m_geoService(geoService)
    , m_pendingCity("")
{
    m_modelService = std::shared_ptr<IWeatherService>(
        m_serviceManager.getService(),
        [](IWeatherService*){}
        );

    if (m_geoService) {
        connect(m_geoService.get(), &GeocodingService::searchFinished,
                this, &WeatherViewModel::onSearchFinished);
    }

    loadFavoritesFromConfig();
    loadFavoriteTemps();
    loadWeather("Москва");
}

void WeatherViewModel::processWeatherData(const QString &city, double lat, double lon)
{
    bool isExpired = CacheManager::isCacheExpired(city);
    WeatherData cachedData = CacheManager::load(city, "forecast");

    if (cachedData.isValid && !isExpired) {
        qDebug() << "--- УСПЕШНО: Данные взяты из свежего кэша для:" << city;
        updateUIData(cachedData);
        loadFavoriteTemps();
        return;
    }

    qDebug() << (isExpired ? "--- Кэш устарел, обновляем из сети..."
                           : "--- Кэш не найден, делаем сетевые запросы...");

    WeatherData current = m_modelService->getCurrentWeather(lat, lon);
    QVector<DailyForecast> forecast = m_modelService->getForecast3Days(lat, lon);

    if (current.isValid && !forecast.isEmpty()) {
        current.dailyForecasts = forecast;
        current.cityName = city;
        CacheManager::save(city, "forecast", current);
        CacheManager::updateCacheTimestamp(city);
        updateUIData(current);
        loadFavoriteTemps();
    } else {
        if (cachedData.isValid) {
            qDebug() << "--- ОШИБКА СЕТИ: Используем старый кэш в качестве резерва.";
            updateUIData(cachedData);
            loadFavoriteTemps();
        } else {
            qDebug() << "--- ОШИБКА: Не удалось получить данные ни из сети, ни из кэша!";
        }
    }
}

void WeatherViewModel::loadWeather(const QString& city) {
    if (city.isEmpty()) {
        qDebug() << "loadWeather: пустое название города";
        return;
    }

    qDebug() << "--- Загрузка погоды для города:" << city;

    m_pendingCity = city;

    if (m_geoService) {
        m_geoService->searchCities(city);
    } else {
        qDebug() << "Ошибка: GeocodingService не инициализирован!";
    }
}

void WeatherViewModel::onSearchFinished(const QVector<CityData> &results)
{
    QVariantList newResults;
    for (const auto &city : results) {
        QVariantMap map;
        map["id"] = city.id;
        map["name"] = city.name;
        map["displayName"] = city.displayName;
        map["country"] = city.country;
        map["lat"] = city.latitude;
        map["lon"] = city.longitude;
        newResults.append(map);
    }
    m_searchResults = newResults;
    emit searchResultsChanged();

    if (m_pendingCity.isEmpty()) {
        return;
    }

    QString city = m_pendingCity;
    m_pendingCity.clear();

    if (results.isEmpty()) {
        qDebug() << "Город не найден:" << city;
        return;
    }

    double lat = results.first().latitude;
    double lon = results.first().longitude;
    qDebug() << "Город найден:" << city << "координаты:" << lat << lon;

    processWeatherData(city, lat, lon);
}

void WeatherViewModel::updateUIData(const WeatherData &data) {
    if (data.dailyForecasts.isEmpty()) {
        qDebug() << "updateUIData: нет данных dailyForecasts";
        return;
    }

    const auto &today = data.dailyForecasts[0];

    m_cityNameText = data.cityName;
    CacheManager::saveHistory(data.cityName, QDate::currentDate(), data);
    m_tempText = QString::number(data.temperatureCurrent, 'f', 1);
    m_humidity = QString::number(data.humidity) + " %";
    m_description = data.description;
    m_windSpeed = QString::number(data.windSpeedMs, 'f', 1);
    m_pressure = QString::number(data.pressure) + " мм рт.ст.";
    m_precipitation = QString::number(today.precipitationMm, 'f', 1) + " мм";
    m_minTemp = QString::number(today.tempMin, 'f', 1);
    m_maxTemp = QString::number(today.tempMax, 'f', 1);
    m_iconCode = data.currentIcon;

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
        dayMap["iconcode"] = day.icon;

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
            qDebug() << "Нет данных в кэше для:" << city;
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

    m_pendingCity = city;
    if (m_geoService) {
        m_geoService->searchCities(city);
    } else {
        qDebug() << "Ошибка: GeocodingService не инициализирован!";
    }
}

void WeatherViewModel::saveApiKey(const QString &apikey) {
    qDebug() << "--- Сохранение API ключа:" << (apikey.isEmpty() ? "пустой" : "***");

    m_serviceManager.setApiKey(apikey);

    m_modelService = std::shared_ptr<IWeatherService>(
        m_serviceManager.getService(),
        [](IWeatherService*){}
        );

    emit apiChanged();

    if (!m_cityNameText.isEmpty() && m_cityNameText != "—") {
        refreshWeather(m_cityNameText);
    } else {
        loadWeather("Москва");
    }
}

void WeatherViewModel::searchCities(const QString &query)
{
    if (m_geoService) {
        m_geoService->searchCities(query);
    } else {
        qDebug() << "Ошибка: GeocodingService не инициализирован!";
    }
}
