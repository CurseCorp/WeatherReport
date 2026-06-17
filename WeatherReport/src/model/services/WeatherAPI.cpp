#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include "src/model/services/weatherapi.h"

WeatherApi::WeatherApi(QObject *parent)
    : QObject(parent)
{
    m_networkManager = new QNetworkAccessManager(this);
    m_apiKey = "bd8adf28f050476fb36183854262505";
    qDebug() << "WeatherApi создан";
}

WeatherApi::~WeatherApi()
{
}

QByteArray WeatherApi::syncGet(const QString &cityName, int days)
{
    QUrl url("http://api.weatherapi.com/v1/forecast.json");
    QUrlQuery query;
    query.addQueryItem("key", m_apiKey);
    query.addQueryItem("q", cityName);
    query.addQueryItem("days", QString::number(days));
    query.addQueryItem("aqi", "no");
    query.addQueryItem("alerts", "no");
    url.setQuery(query);

    qDebug() << "Запрос к WeatherAPI:" << url.toString();

    QNetworkRequest request(url);
    QNetworkReply *reply = m_networkManager->get(request);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QByteArray responseData;
    if (reply->error() == QNetworkReply::NoError) {
        responseData = reply->readAll();
        qDebug() << "Ответ API получен, размер:" << responseData.size();
    } else {
        qDebug() << "Ошибка API:" << reply->errorString();
        emit errorOccurred(reply->errorString());
    }

    reply->deleteLater();
    return responseData;
}

WeatherData WeatherApi::parseCurrentWeatherJson(const QByteArray &rawData)
{
    WeatherData result;
    result.isValid = false;

    QJsonDocument doc = QJsonDocument::fromJson(rawData);
    if (!doc.isObject()) return result;

    QJsonObject obj = doc.object();
    if (obj.contains("error")) return result;

    QJsonObject location = obj["location"].toObject();
    result.cityName = location["name"].toString();
    result.cityId = location["loc_id"].toInt();

    QJsonObject current = obj["current"].toObject();
    result.temperatureCurrent = current["temp_c"].toDouble();
    result.feelsLike = current["feelslike_c"].toDouble();
    result.humidity = current["humidity"].toInt();
    result.pressure = current["pressure_mb"].toInt();
    result.windSpeedMs = current["wind_kph"].toDouble() / 3.6;
    result.description = current["condition"].toObject()["text"].toString();

    result.isValid = true;
    qDebug() << "Текущая погода для города:" << result.cityName;
    qDebug() << "  Температура:" << result.temperatureCurrent << "°C";

    return result;
}

QVector<DailyForecast> WeatherApi::parseForecastJson(const QByteArray &rawData)
{
    QVector<DailyForecast> forecasts;

    QJsonDocument doc = QJsonDocument::fromJson(rawData);
    if (!doc.isObject()) return forecasts;

    QJsonObject obj = doc.object();
    if (obj.contains("error")) return forecasts;

    if (!obj.contains("forecast")) return forecasts;

    QJsonArray forecastDay = obj["forecast"].toObject()["forecastday"].toArray();
    for (const auto &dayItem : forecastDay) {
        QJsonObject dayObj = dayItem.toObject();
        QJsonObject dayData = dayObj["day"].toObject();

        DailyForecast daily;
        daily.date = dayObj["date"].toString();
        daily.tempMin = dayData["mintemp_c"].toDouble();
        daily.tempMax = dayData["maxtemp_c"].toDouble();
        daily.precipitationMm = dayData["totalprecip_mm"].toDouble();
        daily.humidity = dayData["avghumidity"].toInt();
        daily.pressure = dayData["avgvis_km"].toInt();
        daily.windSpeedMs = dayData["maxwind_kph"].toDouble() / 3.6;
        daily.description = dayData["condition"].toObject()["text"].toString();

        for (const auto &hourItem : dayObj["hour"].toArray()) {
            QJsonObject hObj = hourItem.toObject();
            HourlyData hourly;
            hourly.time = hObj["time"].toString().split(" ").last();
            hourly.temp = hObj["temp_c"].toDouble();
            hourly.icon = hObj["condition"].toObject()["icon"].toString();
            daily.hourly.append(hourly);
        }

        forecasts.append(daily);
    }

    qDebug() << "Спарсено" << forecasts.size() << "дней прогноза";
    return forecasts;
}

WeatherData WeatherApi::getCurrentWeather(const QString &cityName)
{
    WeatherData result;
    result.isValid = false;

    if (cityName.trimmed().isEmpty()) {
        qDebug() << "Название города пустое";
        emit errorOccurred("Название города не может быть пустым");
        return result;
    }

    qDebug() << "Запрос текущей погоды для города:" << cityName;

    QByteArray rawData = syncGet(cityName, 1);

    if (rawData.isEmpty()) {
        qDebug() << "Ответ API пустой";
        emit errorOccurred("Пустой ответ от API");
        return result;
    }

    return parseCurrentWeatherJson(rawData);
}

QVector<DailyForecast> WeatherApi::getForecast3Days(const QString &cityName)
{
    QVector<DailyForecast> result;

    if (cityName.trimmed().isEmpty()) {
        qDebug() << "Название города пустое";
        emit errorOccurred("Название города не может быть пустым");
        return result;
    }

    qDebug() << "Запрос прогноза на 3 дня для города:" << cityName;

    QByteArray rawData = syncGet(cityName, 3);

    if (rawData.isEmpty()) {
        qDebug() << "Ответ API пустой";
        emit errorOccurred("Пустой ответ от API");
        return result;
    }

    return parseForecastJson(rawData);
}
