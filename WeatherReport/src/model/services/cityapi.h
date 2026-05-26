#ifndef CITYAPI_H
#define CITYAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include "src/model/entities/City.h"

class CityApi : public QObject
{
    Q_OBJECT

public:
    explicit CityApi(QObject *parent = nullptr);
    ~CityApi();

    City getCityByName(const QString &cityName);

signals:
    void errorOccurred(const QString &error);

private:
    QNetworkAccessManager *m_networkManager;

    QByteArray syncGet(const QString &cityName);
};

#endif
