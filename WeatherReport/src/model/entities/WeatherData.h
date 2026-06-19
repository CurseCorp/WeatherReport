#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include <QString>
#include <QDateTime>
#include <QVector>

struct HourlyData {
    QString time;
    double temp;
    QString icon;
};

struct DailyForecast {
    QString date;
    double tempMin;
    double tempMax;
    QString description;
    int humidity;
    double windSpeedMs;
    int pressure;
    double precipitationMm;
    QVector<HourlyData> hourly;
    QString icon;
};

struct WeatherData {
    int cityId = 0;
    QString cityName;
    double temperatureCurrent = 0.0;
    double feelsLike = 0.0;
    QString description;
    int humidity = 0;
    double windSpeedMs = 0.0;
    int pressure = 0;
    QString currentIcon;
    QDateTime createdAt = QDateTime::currentDateTime();
    QVector<DailyForecast> dailyForecasts;
    bool isValid = false;
};

#endif
