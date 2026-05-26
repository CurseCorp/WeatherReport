#ifndef WEATHERVIEWMODEL_H
#define WEATHERVIEWMODEL_H

#include <QObject>
#include <QString>
#include <memory>
#include "../model/services/weatherapi.h"
class WeatherViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString cityNameText READ cityNameText NOTIFY weatherUpdated)
    Q_PROPERTY(QString tempText READ tempText NOTIFY weatherUpdated)
    Q_PROPERTY(QString humidity READ humidity NOTIFY weatherUpdated)
    Q_PROPERTY(QString description READ description NOTIFY weatherUpdated)
    Q_PROPERTY(QString pressure READ pressure NOTIFY weatherUpdated)
    Q_PROPERTY(QString windSpeed READ windSpeed NOTIFY weatherUpdated)
    Q_PROPERTY(QStringList favoriteCities READ favoriteCities NOTIFY favoriteCitiesChanged)
public:
   explicit WeatherViewModel(std::shared_ptr<WeatherApi> service, QObject *parent = nullptr);
    QString cityNameText() const { return m_cityNameText; }
    QString tempText() const { return m_tempText; }
    QString humidity() const {return m_humidity;   }
    QString description() const {return m_description;   }
    QString windSpeed() const { return m_windSpeed; }
    QString pressure() const {return m_pressure;   }
    QStringList favoriteCities() const { return m_favoriteCities; }
    Q_INVOKABLE void addCityToFavorites(const QString &city);
    Q_INVOKABLE void removeCityFromFavorites(const QString &city);
    Q_INVOKABLE void loadWeather(const QString& city);

signals:
    void weatherUpdated();
    void favoriteCitiesChanged();
private:
    std::shared_ptr<WeatherApi> m_modelService;
    QString m_cityNameText = "—";
    QString m_tempText = "0°C";
    QString m_humidity = "0%";
    QString m_description = "Ливень";
    QString m_pressure = "740мм рт.ст";
    QString m_windSpeed = "28";
    QStringList m_favoriteCities;
    void loadFavoritesFromConfig();

};

#endif // WEATHERVIEWMODEL_H
//temp
//desc
//влажность
//Скорость ветра
//Давление
//Дата и время

//City
//Id
// Name
// Country code
// Coordinates

//Forecast
//Cach
