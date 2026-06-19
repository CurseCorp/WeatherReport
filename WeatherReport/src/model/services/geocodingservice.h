#ifndef GEOCODINGSERVICE_H
#define GEOCODINGSERVICE_H

#include <QObject>
#include <QVector>
#include <QCache>
#include <QTimer>
#include <QNetworkAccessManager>
#include "src/model/entities/City.h"

class GeocodingService : public QObject
{
    Q_OBJECT

public:
    explicit GeocodingService(QObject *parent = nullptr);
    ~GeocodingService();

    void searchCities(const QString &query);
    QVector<CityData> searchCitiesSync(const QString &query);
signals:
    void searchFinished(const QVector<CityData> &results);
    void errorOccurred(const QString &error);

private slots:
    void performSearch();
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_networkManager;
    QCache<QString, QVector<CityData>> m_cache;
    QTimer m_debounceTimer;
    QString m_pendingQuery;
    QString m_apiKey;

    QVector<CityData> parseSearchResults(const QByteArray &rawData);

    void doSearch(const QString &query);
};

#endif // GEOCODINGSERVICE_H
