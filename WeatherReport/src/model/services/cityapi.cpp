#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include "src/model/services/cityapi.h"
CityApi::CityApi(QObject *parent)
    : QObject(parent)
{
    m_networkManager = new QNetworkAccessManager(this);
    qDebug() << "CityApi создан";
}

CityApi::~CityApi()
{
}

QByteArray CityApi::syncGet(const QString &cityName)
{
    QString apiKey = "bd8adf28f050476fb36183854262505";

    QUrl url("http://api.weatherapi.com/v1/search.json");
    QUrlQuery query;
    query.addQueryItem("key", apiKey);
    query.addQueryItem("q", cityName);
    url.setQuery(query);

    qDebug() << "Запрос к API:" << url.toString();

    QNetworkRequest request(url);
    QNetworkReply *reply = m_networkManager->get(request);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QByteArray responseData;
    if (reply->error() == QNetworkReply::NoError) {
        responseData = reply->readAll();
        qDebug() << "Ответ API:" << responseData;
    } else {
        qDebug() << "Ошибка API:" << reply->errorString();
        emit errorOccurred(reply->errorString());
    }

    reply->deleteLater();
    return responseData;
}

City CityApi::getCityByName(const QString &cityName)
{
    City result;
    result.isValid = false;

    if (cityName.trimmed().isEmpty()) {
        qDebug() << "Название города пустое";
        emit errorOccurred("Название города не может быть пустым");
        return result;
    }

    QByteArray rawData = syncGet(cityName);

    if (rawData.isEmpty()) {
        qDebug() << "Ответ API пустой";
        emit errorOccurred("Пустой ответ от API");
        return result;
    }

    QJsonDocument doc = QJsonDocument::fromJson(rawData);

    if (!doc.isArray()) {
        qDebug() << "Ошибка: ожидался массив JSON";
        emit errorOccurred("Неверный формат ответа API");
        return result;
    }

    QJsonArray jsonArray = doc.array();
    if (jsonArray.isEmpty()) {
        qDebug() << "Город не найден:" << cityName;
        emit errorOccurred("Город '" + cityName + "' не найден");
        return result;
    }

    QJsonObject cityObject = jsonArray[0].toObject();

    result.id = cityObject["id"].toInt();
    result.name = cityObject["name"].toString();

    result.displayName = result.name;
    if (cityObject.contains("local_names")) {
        QJsonObject localNames = cityObject["local_names"].toObject();
        if (localNames.contains("ru")) {
            result.displayName = localNames["ru"].toString();
        }
    }

    result.country = cityObject["country"].toString();
    result.latitude = cityObject["lat"].toDouble();
    result.longitude = cityObject["lon"].toDouble();
    result.isValid = true;

    qDebug() << "Город найден:";
    qDebug() << "  ID:" << result.id;
    qDebug() << "  Название (англ):" << result.name;
    qDebug() << "  Название (рус):" << result.displayName;
    qDebug() << "  Страна:" << result.country;
    qDebug() << "  Координаты:" << result.latitude << "," << result.longitude;

    return result;
}
