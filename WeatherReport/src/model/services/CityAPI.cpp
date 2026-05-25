#include "src/model/entities/City.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QUrlQuery>
#include <QSqlError>
#include <QDebug>

City::City(QObject *parent)
    : QObject(parent)
    , m_id(0)
    , m_latitude(0.0)
    , m_longitude(0.0)
{
    qDebug() << "City создан";
    m_networkManager = new QNetworkAccessManager(this);
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &City::onApiReplyFinished);
    connectToDatabase();
}

City::~City()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool City::connectToDatabase()
{
    if (m_database.isOpen()) {
        return true;
    }

    static int connectionCounter = 0;
    connectionCounter++;
    QString connectionName = QString("city_conn_%1").arg(connectionCounter);

    if (QSqlDatabase::contains(connectionName)) {
        QSqlDatabase::removeDatabase(connectionName);
    }

    m_database = QSqlDatabase::addDatabase("QPSQL", connectionName);
    m_database.setHostName("localhost");
    m_database.setPort(5432);
    m_database.setDatabaseName("WeatherReport");
    m_database.setUserName("postgres");
    m_database.setPassword("9098");

    if (!m_database.open()) {
        qDebug() << "Ошибка БД:" << m_database.lastError().text();
        return false;
    }

    qDebug() << "Подключение к БД успешно";
    return true;
}

void City::fetchCityFromAPI(const QString &cityName)
{
    QString apiKey = "bd8adf28f050476fb36183854262505";

    QUrl url("http://api.weatherapi.com/v1/search.json");
    QUrlQuery query;
    query.addQueryItem("key", apiKey);
    query.addQueryItem("q", cityName);
    url.setQuery(query);

    qDebug() << "Запрос к API:" << url.toString();
    m_networkManager->get(QNetworkRequest(url));
}

void City::onApiReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Ошибка API:" << reply->errorString();
        emit errorOccurred("Ошибка API: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    reply->deleteLater();

    qDebug() << "Ответ API:" << responseData;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

    if (!jsonDoc.isArray()) {
        emit errorOccurred("Ошибка формата ответа");
        return;
    }

    QJsonArray jsonArray = jsonDoc.array();
    if (jsonArray.isEmpty()) {
        emit errorOccurred("Город не найден");
        return;
    }

    QJsonObject cityObject = jsonArray[0].toObject();

    m_id = cityObject["id"].toInt();
    m_name = cityObject["name"].toString();

    // Пытаемся получить русское название
    m_displayName = m_name;
    if (cityObject.contains("local_names")) {
        QJsonObject localNames = cityObject["local_names"].toObject();
        if (localNames.contains("ru")) {
            m_displayName = localNames["ru"].toString();
        }
    }

    m_country = cityObject["country"].toString();
    m_latitude = cityObject["lat"].toDouble();
    m_longitude = cityObject["lon"].toDouble();

    qDebug() << "Найден город:" << m_displayName;

    // Посылаем сигнал с названием города для отображения
    emit cityFound(m_displayName);

    saveToDatabase();
}

void City::saveToDatabase()
{
    if (!m_database.isOpen()) {
        if (!connectToDatabase()) {
            emit errorOccurred("Нет подключения к БД");
            return;
        }
    }

    QSqlQuery query(m_database);

    // Проверяем, есть ли уже такой город
    query.prepare("SELECT ID FROM City WHERE ID = :id");
    query.bindValue(":id", m_id);

    bool exists = false;
    if (query.exec() && query.next()) {
        exists = true;
    }

    if (exists) {
        query.prepare("UPDATE City SET Name = :name, DisplayName = :displayName, "
                      "Country = :country, Latitude = :lat, Longitude = :lon "
                      "WHERE ID = :id");
    } else {
        query.prepare("INSERT INTO City (ID, Name, DisplayName, Country, Latitude, Longitude) "
                      "VALUES (:id, :name, :displayName, :country, :lat, :lon)");
    }

    query.bindValue(":id", m_id);
    query.bindValue(":name", m_name);
    query.bindValue(":displayName", m_displayName);
    query.bindValue(":country", m_country);
    query.bindValue(":lat", m_latitude);
    query.bindValue(":lon", m_longitude);

    if (!query.exec()) {
        qDebug() << "Ошибка сохранения:" << query.lastError().text();
        emit errorOccurred("Ошибка сохранения в БД");
    } else {
        qDebug() << "Город сохранён в БД:" << m_displayName;
        emit dataSaved();

        // Проверяем лимит 6 городов
        QSqlQuery countQuery(m_database);
        if (countQuery.exec("SELECT COUNT(*) FROM City")) {
            countQuery.next();
            if (countQuery.value(0).toInt() > 6) {
                QSqlQuery deleteQuery(m_database);
                deleteQuery.exec("DELETE FROM City WHERE created_at = (SELECT MIN(created_at) FROM City)");
                qDebug() << "Удалена старая запись (лимит 6 городов)";
            }
        }
    }
}
