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

QByteArray WeatherApi::syncGet(const QString &cityName)
{
    QUrl url("http://api.weatherapi.com/v1/forecast.json");
    QUrlQuery query;
    query.addQueryItem("key", m_apiKey);
    query.addQueryItem("q", cityName);
    query.addQueryItem("days", "3");
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

WeatherData WeatherApi::parseWeatherJson(const QByteArray &rawData)
{
    WeatherData result;
    result.isValid = false;

    QJsonDocument doc = QJsonDocument::fromJson(rawData);

    if (!doc.isObject()) {
        qDebug() << "Ошибка: ожидался объект JSON";
        emit errorOccurred("Неверный формат ответа API");
        return result;
    }

    QJsonObject obj = doc.object();

    if (obj.contains("error")) {
        QString errorMsg = obj["error"].toObject()["message"].toString();
        qDebug() << "Ошибка API:" << errorMsg;
        emit errorOccurred(errorMsg);
        return result;
    }

    QJsonObject location = obj["location"].toObject();
    result.cityId = location["loc_id"].toInt();
    result.cityName = location["name"].toString();

    QJsonObject current = obj["current"].toObject();
    result.temperatureCurrent = current["temp_c"].toDouble();
    result.feelsLike = current["feelslike_c"].toDouble();
    result.humidity = current["humidity"].toInt();
    result.pressure = current["pressure_mb"].toInt();

    double windKph = current["wind_kph"].toDouble();
    result.windSpeedMs = windKph / 3.6;

    QJsonObject condition = current["condition"].toObject();
    result.description = condition["text"].toString();


    result.dailyForecasts.clear();
    result.hourlyForecast.clear();

    if (obj.contains("forecast")) {
        QJsonObject forecast = obj["forecast"].toObject();
        QJsonArray forecastDay = forecast["forecastday"].toArray();

        if (!forecastDay.isEmpty()) {

            QJsonObject firstDay = forecastDay[0].toObject();
            QJsonObject firstDayData = firstDay["day"].toObject();
            result.temperatureMin = firstDayData["mintemp_c"].toDouble();
            result.temperatureMax = firstDayData["maxtemp_c"].toDouble();
            result.precipitationMm = firstDayData["totalprecip_mm"].toDouble();

            for (const auto &dayItem : forecastDay) {
                QJsonObject dayObj = dayItem.toObject();
                QJsonObject dayData = dayObj["day"].toObject();

                DailyForecast daily;
                daily.date = dayObj["date"].toString();

                QJsonArray hourArray = dayObj["hour"].toArray();
                for (const auto &hourItem : hourArray) {
                    QJsonObject hourObj = hourItem.toObject();

                    HourlyData hourly;
                    hourly.time = hourObj["time"].toString();
                    hourly.temp = hourObj["temp_c"].toDouble();
                    hourly.icon = hourObj["condition"].toObject()["icon"].toString();

                    daily.hourly.append(hourly);
                    result.hourlyForecast.append(hourly);
                }

                result.dailyForecasts.append(daily);
            }

            qDebug() << "  Дней в прогнозе:" << result.dailyForecasts.size();
            qDebug() << "  Всего почасовых записей:" << result.hourlyForecast.size();
        }
    }

    result.isValid = true;

    qDebug() << "Погода для города:" << result.cityName;
    qDebug() << "  Текущая:" << result.temperatureCurrent << "°C";
    qDebug() << "  Ощущается как:" << result.feelsLike << "°C";
    qDebug() << "  Мин/Макс:" << result.temperatureMin << "/" << result.temperatureMax << "°C";
    qDebug() << "  Описание:" << result.description;
    qDebug() << "  Влажность:" << result.humidity << "%";
    qDebug() << "  Ветер:" << result.windSpeedMs << "м/с";
    qDebug() << "  Давление:" << result.pressure << "hPa";
    qDebug() << "  Осадки:" << result.precipitationMm << "мм";
    qDebug() << "  Количество дней в прогнозе:" << result.dailyForecasts.size();
    qDebug() << "  Всего часов в прогнозе:" << result.hourlyForecast.size();

    return result;
}

WeatherData WeatherApi::getWeatherByCityName(const QString &cityName)
{
    WeatherData result;
    result.isValid = false;

    if (cityName.trimmed().isEmpty()) {
        qDebug() << "Название города пустое";
        emit errorOccurred("Название города не может быть пустым");
        return result;
    }

    qDebug() << "Запрос погоды для города:" << cityName;

    QByteArray rawData = syncGet(cityName);

    if (rawData.isEmpty()) {
        qDebug() << "Ответ API пустой";
        emit errorOccurred("Пустой ответ от API");
        return result;
    }

    return parseWeatherJson(rawData);
}
