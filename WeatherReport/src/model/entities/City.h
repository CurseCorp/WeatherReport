#ifndef CITY_H
#define CITY_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSqlDatabase>
#include <QSqlQuery>

class City : public QObject
{
    Q_OBJECT

public:
    City(QObject *parent = nullptr);
    ~City();

    // Основные методы
    void fetchCityFromAPI(const QString &cityName);
    void saveToDatabase();

    // Геттеры
    int getId() const { return m_id; }
    QString getName() const { return m_name; }
    QString getDisplayName() const { return m_displayName.isEmpty() ? m_name : m_displayName; }
    QString getCountry() const { return m_country; }
    double getLatitude() const { return m_latitude; }
    double getLongitude() const { return m_longitude; }

signals:
    void errorOccurred(const QString& error);
    void dataSaved();
    void cityFound(const QString& displayName);  // Сигнал когда город найден

private slots:
    void onApiReplyFinished(QNetworkReply *reply);

private:
    int m_id;
    QString m_name;
    QString m_displayName;
    QString m_country;
    double m_latitude;
    double m_longitude;

    QNetworkAccessManager *m_networkManager;
    QSqlDatabase m_database;
    bool connectToDatabase();
};

#endif // CITY_H
