#ifndef WEATHERAPI_H
#define WEATHERAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include "src/model/entities/WeatherData.h"

class WeatherApi : public QObject
{
    Q_OBJECT

public:
    explicit WeatherApi(QObject *parent = nullptr);
    ~WeatherApi();

    WeatherData getCurrentWeather(const QString &cityName);

    QVector<DailyForecast> getForecast3Days(const QString &cityName);

signals:
    void errorOccurred(const QString &error);

private:
    QNetworkAccessManager *m_networkManager;
    QString m_apiKey;

    QByteArray syncGet(const QString &cityName, int days = 3);

    WeatherData parseCurrentWeatherJson(const QByteArray &rawData);
    QVector<DailyForecast> parseForecastJson(const QByteArray &rawData);
};

#endif // WEATHERAPI_H
