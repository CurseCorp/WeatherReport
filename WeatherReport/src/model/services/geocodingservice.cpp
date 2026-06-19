#include "geocodingservice.h"
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

GeocodingService::GeocodingService(QObject *parent)
    : QObject(parent)
{
    m_networkManager = new QNetworkAccessManager(this);
    m_apiKey = "bd8adf28f050476fb36183854262505";
    m_cache.setMaxCost(100);

    m_debounceTimer.setSingleShot(true);
    m_debounceTimer.setInterval(300);

    connect(&m_debounceTimer, &QTimer::timeout, this, &GeocodingService::performSearch);
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &GeocodingService::onReplyFinished);

    qDebug() << "GeocodingService создан";
}

GeocodingService::~GeocodingService()
{
}

void GeocodingService::searchCities(const QString &query)
{
    if (query.trimmed().length() < 2) {
        emit searchFinished(QVector<CityData>());
        return;
    }

    if (m_cache.contains(query)) {
        qDebug() << "Геокодинг: данные из кэша для:" << query;
        emit searchFinished(*m_cache[query]);
        return;
    }

    m_pendingQuery = query;
    m_debounceTimer.start();
    qDebug() << "Геокодинг: запрос отложен для:" << query;
}

void GeocodingService::performSearch()
{
    doSearch(m_pendingQuery);
}

void GeocodingService::doSearch(const QString &query)
{
    if (query.isEmpty()) return;

    qDebug() << "Геокодинг: реальный запрос для:" << query;

    QUrl url("http://api.weatherapi.com/v1/search.json");
    QUrlQuery urlQuery;
    urlQuery.addQueryItem("key", m_apiKey);
    urlQuery.addQueryItem("q", query);
    url.setQuery(urlQuery);

    QNetworkRequest request(url);
    m_networkManager->get(request);
}

void GeocodingService::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Ошибка геокодинга:" << reply->errorString();
        emit errorOccurred(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray rawData = reply->readAll();
    reply->deleteLater();

    QVector<CityData> results = parseSearchResults(rawData);

    if (!results.isEmpty()) {
        m_cache.insert(m_pendingQuery, new QVector<CityData>(results));
    }

    emit searchFinished(results);
}

QVector<CityData> GeocodingService::parseSearchResults(const QByteArray &rawData)
{
    QVector<CityData> results;

    QJsonDocument doc = QJsonDocument::fromJson(rawData);
    if (!doc.isArray()) return results;

    QJsonArray jsonArray = doc.array();
    for (const auto &item : jsonArray) {
        QJsonObject obj = item.toObject();

        CityData city;
        city.id = obj["id"].toInt();
        city.name = obj["name"].toString();
        city.country = obj["country"].toString();
        city.latitude = obj["lat"].toDouble();
        city.longitude = obj["lon"].toDouble();

        city.displayName = city.name;
        if (obj.contains("local_names")) {
            QJsonObject localNames = obj["local_names"].toObject();
            if (localNames.contains("ru")) {
                city.displayName = localNames["ru"].toString();
            }
        }

        city.isValid = true;
        results.append(city);
    }

    return results;
}
