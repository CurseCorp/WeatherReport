#ifndef WEATHERVIEWMODEL_H
#define WEATHERVIEWMODEL_H

#include <QObject>
#include <QString>
#include <memory>
#include "../model/entities/WeatherService.h"
class WeatherViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString cityNameText READ cityNameText NOTIFY weatherUpdated)
    Q_PROPERTY(QString tempText READ tempText NOTIFY weatherUpdated)
public:
   explicit WeatherViewModel(std::shared_ptr<WeatherService> service, QObject *parent = nullptr);
    QString cityNameText() const { return m_cityNameText; }
    QString tempText() const { return m_tempText; }
    QString humidity() const {return m_humidity;   }
    QString description() const {return m_description;   }
    Q_INVOKABLE void loadWeather(const QString& city);
signals:
    void weatherUpdated();
private:
    std::shared_ptr<WeatherService> m_modelService;
    QString m_cityNameText = "—";
    QString m_tempText = "0°C";
    QString m_humidity = "0%";
    QString m_description = "Not wodnloaded yet";
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
