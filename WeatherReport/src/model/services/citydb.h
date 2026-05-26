#ifndef CITYDB_H
#define CITYDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "src/model/services/CityApi.h"

class CityRepository : public QObject
{
    Q_OBJECT

public:
    explicit CityRepository(QObject *parent = nullptr);
    ~CityRepository();

    void saveCity(const CityApi &city);
    void enforceMaxCitiesLimit();
    bool cityExists(int cityId);

    // Загрузка городов
    QVector<CityApi> loadAllCities();

signals:
    void errorOccurred(const QString& error);
    void citySaved();

private:
    QSqlDatabase m_database;
    bool connectToDatabase();
};

#endif
