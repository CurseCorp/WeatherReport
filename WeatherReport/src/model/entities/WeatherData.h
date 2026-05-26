#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include <QString>
#include <QDateTime>

struct WeatherData
{
    int cityId = 0;
    QString cityName;

    double temperatureCurrent = 0.0;
    double temperatureMin = 0.0;
    double temperatureMax = 0.0;

    QString description;

    int humidity = 0;

    double windSpeedMs = 0.0;

    int pressure = 0;

    double precipitationMm = 0.0;

    double feelsLike = 0.0;

    QDateTime createdAt = QDateTime::currentDateTime();

    bool isValid = false;
};

#endif
