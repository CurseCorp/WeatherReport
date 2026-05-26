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

    WeatherData getWeatherByCityName(const QString &cityName);

signals:
    void errorOccurred(const QString &error);

private:
    QNetworkAccessManager *m_networkManager;
    QByteArray syncGet(const QString &cityName);
    WeatherData parseWeatherJson(const QByteArray &rawData);

    QString m_apiKey;
};

#endif // WEATHERAPI_H
