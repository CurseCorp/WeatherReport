#ifndef CITYDATA_H
#define CITYDATA_H

#include <QString>

struct CityData
{
    int id = 0;
    QString name;
    QString displayName;
    QString country;
    double latitude = 0.0;
    double longitude = 0.0;
    bool isValid = false;
};

#endif // CITYDATA_H
