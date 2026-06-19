#include "openweathermapservice.h"
#include "weathericonmapper.h"
#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <cmath>

OpenWeatherMapService::OpenWeatherMapService()
    : m_networkManager(new QNetworkAccessManager(this))
{
    qDebug() << "OpenWeatherMapService создан";
}

OpenWeatherMapService::~OpenWeatherMapService()
{
}

void OpenWeatherMapService::setApiKey(const QString &key)
{
    m_apiKey = key;
    qDebug() << "OpenWeatherMapService: ключ установлен";
}

double OpenWeatherMapService::roundWindSpeed(double speed) const
{
    return std::round(speed * 10.0) / 10.0;
}

QByteArray OpenWeatherMapService::syncGet(const QUrl &url)
{
    qDebug() << "Запрос к OpenWeatherMap:" << url.toString();

    QNetworkRequest request(url);
    QNetworkReply *reply = m_networkManager->get(request);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QByteArray responseData;
    if (reply->error() == QNetworkReply::NoError) {
        responseData = reply->readAll();
        qDebug() << "Ответ OpenWeatherMap получен, размер:" << responseData.size();
    } else {
        qDebug() << "Ошибка OpenWeatherMap:" << reply->errorString();
    }

    reply->deleteLater();
    return responseData;
}

WeatherData OpenWeatherMapService::parseCurrentWeather(const QByteArray &rawData)
{
    WeatherData result;
    result.isValid = false;

    QJsonDocument doc = QJsonDocument::fromJson(rawData);
    if (!doc.isObject()) return result;

    QJsonObject obj = doc.object();
    if (obj.contains("cod") && obj["cod"].toString() != "200") {
        qDebug() << "Ошибка OpenWeatherMap:" << obj["message"].toString();
        return result;
    }

    result.cityId = obj["id"].toInt();
    result.cityName = obj["name"].toString();

    QJsonObject main = obj["main"].toObject();
    result.temperatureCurrent = main["temp"].toDouble();
    result.temperatureMin = main["temp_min"].toDouble();
    result.temperatureMax = main["temp_max"].toDouble();
    result.humidity = main["humidity"].toInt();
    result.pressure = main["pressure"].toInt();
    result.feelsLike = main["feels_like"].toDouble();

    QJsonObject wind = obj["wind"].toObject();
    result.windSpeedMs = roundWindSpeed(wind["speed"].toDouble());

    if (obj.contains("rain")) {
        QJsonObject rain = obj["rain"].toObject();
        result.precipitationMm = rain["1h"].toDouble();
    }

    QJsonArray weather = obj["weather"].toArray();
    if (!weather.isEmpty()) {
        QJsonObject w = weather[0].toObject();
        result.description = w["description"].toString();
        result.currentIcon = WeatherIconMapper::getIconNameForOpenWeatherMap(w["id"].toInt());
    }

    result.isValid = true;
    return result;
}

QVector<DailyForecast> OpenWeatherMapService::parseForecast(const QByteArray &rawData)
{
    QVector<DailyForecast> forecasts;

    QJsonDocument doc = QJsonDocument::fromJson(rawData);
    if (!doc.isObject()) return forecasts;

    QJsonObject obj = doc.object();
    if (obj.contains("cod") && obj["cod"].toString() != "200") {
        qDebug() << "Ошибка OpenWeatherMap прогноз:" << obj["message"].toString();
        return forecasts;
    }

    QJsonArray list = obj["list"].toArray();
    QDate currentDate = QDate::currentDate();

    for (const auto &item : list) {
        QJsonObject forecast = item.toObject();
        QString dtTxt = forecast["dt_txt"].toString();
        QDateTime dt = QDateTime::fromString(dtTxt, "yyyy-MM-dd hh:mm:ss");

        if (dt.date() < currentDate) continue;
        if (dt.date() > currentDate.addDays(2)) break;

        DailyForecast daily;
        daily.date = dt.date().toString("yyyy-MM-dd");

        QJsonObject main = forecast["main"].toObject();
        daily.tempMin = main["temp_min"].toDouble();
        daily.tempMax = main["temp_max"].toDouble();
        daily.humidity = main["humidity"].toInt();
        daily.pressure = main["pressure"].toInt();
        daily.feelsLike = main["feels_like"].toDouble();

        QJsonObject wind = forecast["wind"].toObject();
        daily.windSpeedMs = roundWindSpeed(wind["speed"].toDouble());

        if (forecast.contains("rain")) {
            QJsonObject rain = forecast["rain"].toObject();
            daily.precipitationMm = rain["3h"].toDouble();
        }

        QJsonArray weather = forecast["weather"].toArray();
        if (!weather.isEmpty()) {
            QJsonObject w = weather[0].toObject();
            daily.description = w["description"].toString();
            daily.icon = WeatherIconMapper::getIconNameForOpenWeatherMap(w["id"].toInt());
        }

        HourlyData hour;
        hour.time = dt.toString("hh:00");
        hour.temp = main["temp"].toDouble();
        hour.icon = daily.icon;
        daily.hourly.append(hour);

        forecasts.append(daily);
    }

    QMap<QString, DailyForecast> groupedForecasts;
    for (const auto &daily : forecasts) {
        if (!groupedForecasts.contains(daily.date)) {
            DailyForecast newDay;
            newDay.date = daily.date;
            newDay.tempMin = 9999;
            newDay.tempMax = -9999;
            newDay.humidity = daily.humidity;
            newDay.pressure = daily.pressure;
            newDay.windSpeedMs = daily.windSpeedMs;
            newDay.precipitationMm = daily.precipitationMm;
            newDay.description = daily.description;
            newDay.icon = daily.icon;
            newDay.feelsLike = daily.feelsLike;
            groupedForecasts[daily.date] = newDay;
        }

        if (daily.tempMin < groupedForecasts[daily.date].tempMin) {
            groupedForecasts[daily.date].tempMin = daily.tempMin;
        }
        if (daily.tempMax > groupedForecasts[daily.date].tempMax) {
            groupedForecasts[daily.date].tempMax = daily.tempMax;
        }

        groupedForecasts[daily.date].hourly.append(daily.hourly);
    }

    QVector<DailyForecast> interpolatedForecasts;

    for (auto it = groupedForecasts.begin(); it != groupedForecasts.end(); ++it) {
        DailyForecast interpolated = it.value();
        interpolated.hourly.clear();

        const QVector<HourlyData> &threeHourData = it.value().hourly;

        for (int i = 0; i < threeHourData.size(); ++i) {
            const HourlyData &hour = threeHourData[i];

            int hourInt = hour.time.left(2).toInt();

            for (int offset = 0; offset < 3; ++offset) {
                int newHour = hourInt + offset;
                if (newHour >= 24) break;

                if (newHour > 23) break;

                HourlyData newHourData;
                newHourData.time = QString("%1:00").arg(newHour, 2, 10, QChar('0'));
                newHourData.temp = hour.temp;
                newHourData.icon = hour.icon;
                interpolated.hourly.append(newHourData);
            }
        }

        if (interpolated.hourly.size() > 24) {
            interpolated.hourly.resize(24);
        }

        interpolatedForecasts.append(interpolated);
    }

    qDebug() << "OpenWeatherMap: интерполировано" << interpolatedForecasts.size() << "дней";
    for (const auto &day : interpolatedForecasts) {
        qDebug() << "  День" << day.date << "часов:" << day.hourly.size();
    }

    return interpolatedForecasts;
}


WeatherData OpenWeatherMapService::getCurrentWeather(double latitude, double longitude)
{
    WeatherData result;
    result.isValid = false;

    if (m_apiKey.isEmpty()) {
        qDebug() << "OpenWeatherMapService: ключ не установлен";
        return result;
    }

    QUrl url("https://api.openweathermap.org/data/2.5/weather");
    QUrlQuery query;
    query.addQueryItem("lat", QString::number(latitude, 'f', 6));
    query.addQueryItem("lon", QString::number(longitude, 'f', 6));
    query.addQueryItem("appid", m_apiKey);
    query.addQueryItem("units", "metric");
    url.setQuery(query);

    QByteArray rawData = syncGet(url);
    if (rawData.isEmpty()) return result;

    return parseCurrentWeather(rawData);
}

QVector<DailyForecast> OpenWeatherMapService::getForecast3Days(double latitude, double longitude)
{
    QVector<DailyForecast> result;

    if (m_apiKey.isEmpty()) {
        qDebug() << "OpenWeatherMapService: ключ не установлен";
        return result;
    }

    QUrl url("https://api.openweathermap.org/data/2.5/forecast");
    QUrlQuery query;
    query.addQueryItem("lat", QString::number(latitude, 'f', 6));
    query.addQueryItem("lon", QString::number(longitude, 'f', 6));
    query.addQueryItem("appid", m_apiKey);
    query.addQueryItem("units", "metric");

    url.setQuery(query);

    QByteArray rawData = syncGet(url);
    if (rawData.isEmpty()) return result;

    return parseForecast(rawData);
}
