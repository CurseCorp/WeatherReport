#ifndef WEATHERVIEWMODEL_H
#define WEATHERVIEWMODEL_H

#include <QObject>
#include <QString>
#include <memory>
class WeatherViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString visibleCityName READ visibleCityName NOTIFY visibleCityNameChanged)
public:
    explicit WeatherViewModel(QObject *parent = nullptr);
    QString visibleCityName() const;
    Q_INVOKABLE void searchCity(const QString &cityName);
signals:
    void visibleCityNameChanged();
private:
    QString m_visibleCityName;
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
