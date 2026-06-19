#ifndef WEATHERICONMAPPER_H
#define WEATHERICONMAPPER_H

#include <QString>

class WeatherIconMapper
{
public:
    static QString getIconNameForWeatherAPI(const QString &code);
    static QString getIconNameForOpenWeatherMap(int code);
};

#endif // WEATHERICONMAPPER_H
