#ifndef WEATHERVIEWMODEL_H
#define WEATHERVIEWMODEL_H

#include <QObject>
#include <QString>
#include <QVariantList>
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
    Q_PROPERTY(QString precipitation READ precipitation NOTIFY weatherUpdated)
    Q_PROPERTY(QString minTemp READ minTemp NOTIFY weatherUpdated)
    Q_PROPERTY(QString maxTemp READ maxTemp NOTIFY weatherUpdated)
     Q_PROPERTY(QString iconCode READ iconCode NOTIFY weatherUpdated)
    Q_PROPERTY(QStringList favoriteCities READ favoriteCities NOTIFY favoriteCitiesChanged)
    Q_PROPERTY(QVariantList forecastModel READ forecastModel NOTIFY forecastModelChanged)
    Q_PROPERTY(QVariantMap favoriteCityTemps READ favoriteCityTemps NOTIFY favoriteCityTempsChanged)
    Q_PROPERTY(QVariantList historyData READ historyData NOTIFY historyDataChanged)

public:
   explicit WeatherViewModel(std::shared_ptr<WeatherApi> service, QObject *parent = nullptr);
    QString cityNameText() const { return m_cityNameText; }
    QString tempText() const { return m_tempText; }
    QString humidity() const {return m_humidity;   }
    QString description() const {return m_description;   }
    QString windSpeed() const { return m_windSpeed; }
    QString pressure() const {return m_pressure;   }
    QString precipitation() const {return m_precipitation;   }
    QString minTemp() const {return m_minTemp;   }
    QString maxTemp() const {return m_maxTemp;   }
    QString iconCode() const {return m_iconCode;   }
    QStringList favoriteCities() const { return m_favoriteCities; }

    QVariantList forecastModel() const { return m_forecastModel; }
    void setForecastData(const QVariantList& newForecast);
    Q_INVOKABLE void addCityToFavorites(const QString &city);
    Q_INVOKABLE void removeCityFromFavorites(const QString &city);
    Q_INVOKABLE void loadWeather(const QString& city);
    Q_INVOKABLE void loadFavoriteTemps();
    Q_INVOKABLE void loadHistory(const QString &city);
    Q_INVOKABLE void refreshWeather(const QString &city);
    QVariantMap favoriteCityTemps() const { return m_favoriteCityTemps; }
    QVariantList historyData() const { return m_historyData; }
signals:
    void weatherUpdated();
    void favoriteCitiesChanged();
    void historyDataLoaded(const WeatherData &data);
    void forecastModelChanged();
    void favoriteCityTempsChanged();
    void historyDataChanged();
private:
    std::shared_ptr<WeatherApi> m_modelService;
    QString m_cityNameText = "—";
    QString m_tempText = "0";
    QString m_humidity = "0%";
    QString m_description = "Ливень";
    QString m_pressure = "740мм рт.ст";
    QString m_windSpeed = "28";
    QString m_precipitation = "0%";
    QString m_minTemp = "0";
    QString m_maxTemp = "0";
    QString m_iconCode = "0";
    QStringList m_favoriteCities;
    QVariantMap m_favoriteCityTemps;
    void loadFavoritesFromConfig();
    void updateUIData(const WeatherData &);
    QVariantList m_forecastModel;
    QVariantList m_historyData;

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
