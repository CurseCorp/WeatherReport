#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QSettings>
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
        if (file.open(QIODevice::ReadOnly)) {
            return deserialize(QJsonDocument::fromJson(file.readAll()).object());
        }
        return WeatherData();
    }

    static void saveHistory(const QString &city, const QDate &date, const WeatherData &data) {
        QString dirPath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
                          "/WeatherReportApp/" + city.trimmed().replace(" ", "_") + "/history";
        QDir().mkpath(dirPath);

        QJsonObject singleDayData;
        QString targetDateStr = date.toString("yyyy-MM-dd");

        for (const auto &day : data.dailyForecasts) {
            if (day.date == targetDateStr) {
                singleDayData = serializeSingleDay(day);
                break;
            }
        }

        if (singleDayData.isEmpty()) return;

        QDir dir(dirPath);
        QStringList files = dir.entryList({"*.json"}, QDir::Files, QDir::Name);
        while (files.size() >= 7) {
            dir.remove(files.first());
            files.removeFirst();
        }

        QString path = dirPath + "/" + targetDateStr + ".json";
        QFile file(path);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(QJsonDocument(singleDayData).toJson());
            file.close();
        }
    }
    static bool isCacheExpired(const QString &city) {
        QSettings settings("CurseCorp", "WeatherReport");
        QDateTime lastUpdate = settings.value(city + "/lastUpdate").toDateTime();
        if (!lastUpdate.isValid()) return true;
        return lastUpdate.secsTo(QDateTime::currentDateTime()) > 86400;
    }
    static void updateCacheTimestamp(const QString &city) {
        QSettings settings("CurseCorp", "WeatherReport");
        settings.setValue(city + "/lastUpdate", QDateTime::currentDateTime());
    }
    static WeatherData loadHistory(const QString &city, const QDate &date) {
        QString safeCity = city.trimmed().replace(" ", "_");
        QString path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
                       "/WeatherReportApp/" + safeCity + "/history/" + date.toString("yyyy-MM-dd") + ".json";

        QFile file(path);
        if (file.open(QIODevice::ReadOnly)) {
            QJsonObject json = QJsonDocument::fromJson(file.readAll()).object();
            WeatherData data;
            data.cityName = city;

            DailyForecast day;
            day.date = json["date"].toString();
            day.tempMin = json["tempMin"].toDouble();
            day.tempMax = json["tempMax"].toDouble();
            day.description = json["description"].toString();
            day.humidity = json["humidity"].toInt();
            day.windSpeedMs = json["windSpeed"].toDouble();
            day.pressure = json["pressure"].toInt();
            day.precipitationMm = json["precipitation"].toDouble();
            day.icon = json["iconcode"].toString();

            for (const QJsonValue &hVal : json["hourly"].toArray()) {
                QJsonObject h = hVal.toObject();
                day.hourly.append({h["time"].toString(), h["temp"].toDouble(), h["icon"].toString()});
            }

            data.dailyForecasts.append(day);
            data.isValid = true;
            return data;
        }
        return WeatherData();
    }

private:
    static QJsonObject serializeSingleDay(const DailyForecast &day) {
        QJsonObject obj;
        obj["date"] = day.date;
        obj["tempMin"] = day.tempMin;
        obj["tempMax"] = day.tempMax;
        obj["description"] = day.description;
        obj["humidity"] = day.humidity;
        obj["windSpeed"] = day.windSpeedMs;
        obj["pressure"] = day.pressure;
        obj["precipitation"] = day.precipitationMm;
        obj["iconcode"] = day.icon;

        QJsonArray hArr;
        for (const auto &h : day.hourly) {
            QJsonObject hObj;
            hObj["time"] = h.time;
            hObj["temp"] = h.temp;
            hObj["icon"] = h.icon;
            hArr.append(hObj);
        }
        obj["hourly"] = hArr;
        return obj;
    }

    static QString getFilePath(const QString &city, const QString &subFolder) {
        QString safeCity = city.trimmed().replace(" ", "_");
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
        root["iconcode"] = data.currentIcon;
        QJsonArray daysArray;
        for (const auto &day : data.dailyForecasts) {
            daysArray.append(serializeSingleDay(day));
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
        data.currentIcon = json["iconcode"].toString();
        for (const QJsonValue &dayVal : json["dailyForecasts"].toArray()) {
            QJsonObject d = dayVal.toObject();
            DailyForecast day;
            day.date = d["date"].toString();
            day.tempMin = d["tempMin"].toDouble();
            day.tempMax = d["tempMax"].toDouble();
            day.description = d["description"].toString();
            day.humidity = d["humidity"].toInt();
            day.windSpeedMs = d["windSpeed"].toDouble();
            day.pressure = d["pressure"].toInt();
            day.icon =d["iconcode"].toString();
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
