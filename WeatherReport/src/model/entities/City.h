#ifndef CITY_H
#define CITY_H

#include <QString>

struct City
{
    int id = 0;
    QString name;
    QString displayName;
    QString country;
    double latitude = 0.0;
    double longitude = 0.0;
    bool isValid = false;
};

#endif
