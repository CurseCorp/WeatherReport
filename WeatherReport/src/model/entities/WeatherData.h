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
    QVector<HourlyData> hourly;

    double tempMin = 0.0;
    double tempMax = 0.0;
    double precipitationMm = 0.0;
    int humidity = 0;
    int pressure = 0;
    double windSpeedMs = 0.0;
    QString description;
    QString icon;
    double feelsLike = 0.0;
};

struct WeatherData
{
    int cityId = 0;
    QString cityName;

    double temperatureCurrent = 0.0;
    double temperatureMin = 0.0;
    double temperatureMax = 0.0;

    QString description;
    QString currentIcon;

    int humidity = 0;
    double windSpeedMs = 0.0;
    int pressure = 0;
    double precipitationMm = 0.0;
    double feelsLike = 0.0;

    QDateTime createdAt = QDateTime::currentDateTime();
    QVector<HourlyData> hourlyForecast;
    QVector<DailyForecast> dailyForecasts;

    bool isValid = false;
};

#endif // WEATHERDATA_H
