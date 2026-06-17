#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include "../model/services/weatherapi.h"

class CacheManager {
public:
    static void save(const QString &city, const QString &subFolder, const WeatherData &data) {
        QString path = getFilePath(city, subFolder);
        QFile file(path);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(QJsonDocument(serialize(data)).toJson());
            file.close();
            qDebug() << "Кэш сохранен в:" << path;
        }
    }

    static WeatherData load(const QString &city, const QString &subFolder) {
        QString path = getFilePath(city, subFolder);
        QFile file(path);

        qDebug() << "Проверка кэша по пути:" << path; //

        if (file.open(QIODevice::ReadOnly)) {
            QByteArray data = file.readAll();
            file.close();
            return deserialize(QJsonDocument::fromJson(data).object());
        }
        return WeatherData();
    }

private:
    static QString getFilePath(const QString &city, const QString &subFolder) {
        QString safeCity = QString(city).trimmed().replace(" ", "_");
        QString dirPath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
                          "/WeatherReportApp/" + safeCity + "/" + subFolder;
        QDir().mkpath(dirPath);
        return dirPath + "/forecast.json";
    }
    static QJsonObject serialize(const WeatherData &data) {
        QJsonObject root;
        root["cityName"] = data.cityName;
        root["temp"] = data.temperatureCurrent;
        root["humidity"] = data.humidity;
        root["pressure"] = data.pressure;
        root["windSpeed"] = data.windSpeedMs;
        root["description"] = data.description;

        QJsonArray daysArray;
        for (const auto &day : data.dailyForecasts) {
            QJsonObject dayObj;
            dayObj["date"] = day.date;
            dayObj["tempMin"] = day.tempMin;
            dayObj["tempMax"] = day.tempMax;
            dayObj["description"] = day.description;
            dayObj["humidity"] = day.humidity;
            dayObj["windSpeed"] = day.windSpeedMs;
            dayObj["pressure"] = day.pressure;
            dayObj["precipitation"] = day.precipitationMm;

            QJsonArray hArr;
            for (const auto &h : day.hourly) {
                QJsonObject hObj;
                hObj["time"] = h.time;
                hObj["temp"] = h.temp;
                hObj["icon"] = h.icon;
                hArr.append(hObj);
            }
            dayObj["hourly"] = hArr;
            daysArray.append(dayObj);
        }
        root["dailyForecasts"] = daysArray;
        return root;
    }

    static WeatherData deserialize(const QJsonObject &json) {
        WeatherData data;
        data.cityName = json["cityName"].toString();
        data.temperatureCurrent = json["temp"].toDouble();
        data.humidity = json["humidity"].toInt();
        data.pressure = json["pressure"].toInt();
        data.windSpeedMs = json["windSpeed"].toDouble();
        data.description = json["description"].toString();

        QJsonArray daysArray = json["dailyForecasts"].toArray();
        for (const QJsonValue &dayVal : daysArray) {
            QJsonObject d = dayVal.toObject();
            DailyForecast day;
            day.date = d["date"].toString();
            day.tempMin = d["tempMin"].toDouble();
            day.tempMax = d["tempMax"].toDouble();
            day.description = d["description"].toString();
            day.humidity = d["humidity"].toInt();
            day.windSpeedMs = d["windSpeed"].toDouble();
            day.pressure = d["pressure"].toInt();
            day.precipitationMm = d["precipitation"].toDouble();

            for (const QJsonValue &hVal : d["hourly"].toArray()) {
                QJsonObject h = hVal.toObject();
                day.hourly.append({h["time"].toString(), h["temp"].toDouble(), h["icon"].toString()});
            }
            data.dailyForecasts.append(day);
        }
        data.isValid = true;
        return data;
    }
};

#endif // CACHEMANAGER_H
