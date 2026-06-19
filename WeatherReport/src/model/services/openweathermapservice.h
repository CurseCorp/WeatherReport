#ifndef OPENWEATHERMAPSERVICE_H
#define OPENWEATHERMAPSERVICE_H

#include "IWeatherService.h"
#include <QNetworkAccessManager>
#include <QString>
#include <QObject>
class OpenWeatherMapService : public QObject,public IWeatherService
{
    Q_OBJECT
public:
    OpenWeatherMapService();
    ~OpenWeatherMapService();

    void setApiKey(const QString &key);

    WeatherData getCurrentWeather(double latitude, double longitude) override;
    QVector<DailyForecast> getForecast3Days(double latitude, double longitude) override;

private:
    QNetworkAccessManager *m_networkManager;
    QString m_apiKey;

    QByteArray syncGet(const QUrl &url);
    double roundWindSpeed(double speed) const;

    WeatherData parseCurrentWeather(const QByteArray &rawData);
    QVector<DailyForecast> parseForecast(const QByteArray &rawData);
};

#endif // OPENWEATHERMAPSERVICE_H
