#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include "../model/services/weatherapi.h"


class CacheManager {
public:
    static QString getCacheDir() {
        QString path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/WeatherReportApp";
        QDir dir(path);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        return path;
    }
    static QJsonObject serialize(const WeatherData &data) {
        QJsonObject obj;
        obj["cityName"] = data.cityName;
        obj["temp"] = data.temperatureCurrent;
        obj["tempMin"] = data.temperatureMin;
        obj["tempMax"] = data.temperatureMax;
        obj["humidity"] = data.humidity;
        obj["description"] = data.description;
        obj["precipitation"] = data.precipitationMm;
        obj["pressure"] = data.pressure;
        obj["windSpeed"] = data.windSpeedMs;
        QJsonArray hourlyArray;
        for (const auto &item : data.hourlyForecast) {
            QJsonObject hourObj;
            hourObj["time"] = item.time;
            hourObj["temp"] = item.temp;
            hourlyArray.append(hourObj);
        }
        obj["hourly"] = hourlyArray;
        return obj;
    }

    static WeatherData deserialize(const QJsonObject &obj) {
        WeatherData data;
        data.cityName = obj["cityName"].toString();
        data.temperatureCurrent = obj["temp"].toDouble();
        data.temperatureMin = obj["tempMin"].toDouble();
        data.temperatureMax = obj["tempMax"].toDouble();
        data.humidity = obj["humidity"].toInt();
        data.description = obj["description"].toString();
        data.precipitationMm = obj["precipitation"].toDouble();
        data.pressure = obj["pressure"].toDouble();
        data.windSpeedMs = obj["windSpeed"].toDouble();
        QJsonArray hourlyArray = obj["hourly"].toArray();
        for (const QJsonValue &value : hourlyArray) {
            QJsonObject hourObj = value.toObject();

            WeatherData::HourlyData hourItem;
            hourItem.time = hourObj["time"].toString();
            hourItem.temp = hourObj["temp"].toDouble();

            data.hourlyForecast.append(hourItem);
        }

        return data;
    }

    static void save(const QString &city, const WeatherData &data) {
       QString path = getCacheDir() + "/" + city.toLower() + ".json";
        QFile file(path);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(QJsonDocument(serialize(data)).toJson());
        }
    }

    static bool load(const QString &city, WeatherData &outData) {
        QString path = getCacheDir() + "/" + city.toLower() + ".json";
        QFile file(path);
        if (file.exists() && file.open(QIODevice::ReadOnly)) {
            outData = deserialize(QJsonDocument::fromJson(file.readAll()).object());
            return true;
        }
        return false;
    }
};
#endif
