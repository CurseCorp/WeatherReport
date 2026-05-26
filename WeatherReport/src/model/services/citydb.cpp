#include "src/model/services/citydb.h"
#include <QSqlError>
#include <QDebug>

CityDB::CityRepository(QObject *parent)
    : QObject(parent)
{
    connectToDatabase();
}

CityRepository::~CityRepository()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool CityRepository::connectToDatabase()
{
    if (m_database.isOpen()) {
        return true;
    }

    static int connectionCounter = 0;
    connectionCounter++;
    QString connectionName = QString("city_repo_%1").arg(connectionCounter);

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

bool CityRepository::cityExists(int cityId)
{
    if (!m_database.isOpen()) {
        return false;
    }

    QSqlQuery query(m_database);
    query.prepare("SELECT ID FROM City WHERE ID = :id");
    query.bindValue(":id", cityId);

    return query.exec() && query.next();
}

void CityRepository::saveCity(const CityApi &city)
{
    if (!m_database.isOpen()) {
        if (!connectToDatabase()) {
            emit errorOccurred("Нет подключения к БД");
            return;
        }
    }

    QSqlQuery query(m_database);

    bool exists = cityExists(city.getId());

    if (exists) {
        query.prepare("UPDATE City SET Name = :name, DisplayName = :displayName, "
                      "Country = :country, Latitude = :lat, Longitude = :lon "
                      "WHERE ID = :id");
    } else {
        query.prepare("INSERT INTO City (ID, Name, DisplayName, Country, Latitude, Longitude) "
                      "VALUES (:id, :name, :displayName, :country, :lat, :lon)");
    }

    query.bindValue(":id", city.getId());
    query.bindValue(":name", city.getName());
    query.bindValue(":displayName", city.getDisplayName());
    query.bindValue(":country", city.getCountry());
    query.bindValue(":lat", city.getLatitude());
    query.bindValue(":lon", city.getLongitude());

    if (!query.exec()) {
        qDebug() << "Ошибка сохранения:" << query.lastError().text();
        emit errorOccurred("Ошибка сохранения в БД");
    } else {
        qDebug() << "Город сохранён в БД:" << city.getDisplayName();
        enforceMaxCitiesLimit();
        emit citySaved();
    }
}

void CityAPI::enforceMaxCitiesLimit()
{
    if (!m_database.isOpen()) {
        return;
    }

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

QVector<CityApi> CityRepository::loadAllCities()
{
    QVector<CityApi> cities;

    if (!m_database.isOpen()) {
        return cities;
    }

    QSqlQuery query(m_database);
    if (query.exec("SELECT ID, Name, DisplayName, Country, Latitude, Longitude FROM City ORDER BY created_at DESC")) {
        while (query.next()) {
            CityApi city;
            city.setId(query.value("ID").toInt());
            city.setName(query.value("Name").toString());
            city.setDisplayName(query.value("DisplayName").toString());
            city.setCountry(query.value("Country").toString());
            city.setLatitude(query.value("Latitude").toDouble());
            city.setLongitude(query.value("Longitude").toDouble());
            cities.append(city);
        }
    }

    qDebug() << "Загружено городов:" << cities.size();
    return cities;
}
