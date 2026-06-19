#include "weathericonmapper.h"
#include <QHash>
#include <QDebug>

QString WeatherIconMapper::getIconNameForWeatherAPI(const QString &code)
{
    static QHash<QString, QString> map = {
        {"1000", "sunny"},
        {"1001", "sunny"},
        {"1002", "sunny"},
        {"1003", "partly_cloudy"},
        {"1004", "partly_cloudy"},
        {"1005", "partly_cloudy"},
        {"1006", "cloudy"},
        {"1007", "cloudy"},
        {"1008", "cloudy"},
        {"1009", "overcast"},

        {"1030", "fog"},
        {"1135", "fog"},
        {"1147", "fog"},

        {"1063", "drizzle"},
        {"1066", "snow"},
        {"1069", "snow"},
        {"1072", "drizzle"},

        {"1087", "thunderstorm"},
        {"1114", "snow"},
        {"1117", "snow"},

        {"1150", "drizzle"},
        {"1153", "drizzle"},
        {"1168", "drizzle"},
        {"1171", "drizzle"},

        {"1180", "light_rain"},
        {"1183", "rain"},
        {"1186", "rain"},
        {"1189", "rain"},
        {"1192", "heavy_rain"},
        {"1195", "heavy_rain"},

        {"1198", "rain"},
        {"1201", "rain"},

        {"1204", "snow"},
        {"1207", "snow"},
        {"1210", "light_snow"},
        {"1213", "snow"},
        {"1216", "heavy_snow"},
        {"1219", "heavy_snow"},
        {"1222", "light_snow"},
        {"1225", "heavy_snow"},
        {"1237", "snow"},

        {"1240", "light_rain"},
        {"1243", "rain"},
        {"1246", "heavy_rain"},

        {"1249", "snow"},
        {"1252", "snow"},
        {"1255", "light_snow"},
        {"1258", "heavy_snow"},
        {"1261", "snow"},
        {"1264", "snow"},

        {"1273", "thunderstorm"},
        {"1276", "thunderstorm"},
        {"1279", "thunderstorm"},
        {"1282", "thunderstorm"}
    };

    if (!map.contains(code)) {
        qDebug() << "⚠️ [WeatherAPI] Неизвестный код иконки:" << code;
    }

    return map.value(code, "sunny");
}

QString WeatherIconMapper::getIconNameForOpenWeatherMap(int code)
{
    static QHash<int, QString> map = {
        {200, "thunderstorm"},
        {201, "thunderstorm"},
        {202, "thunderstorm"},
        {210, "thunderstorm"},
        {211, "thunderstorm"},
        {212, "thunderstorm"},
        {221, "thunderstorm"},
        {230, "thunderstorm"},
        {231, "thunderstorm"},
        {232, "thunderstorm"},

        {300, "drizzle"},
        {301, "drizzle"},
        {302, "drizzle"},
        {310, "drizzle"},
        {311, "drizzle"},
        {312, "drizzle"},
        {313, "drizzle"},
        {314, "drizzle"},
        {321, "drizzle"},

        {500, "light_rain"},
        {501, "rain"},
        {502, "heavy_rain"},
        {503, "heavy_rain"},
        {504, "heavy_rain"},
        {511, "rain"},
        {520, "rain"},
        {521, "rain"},
        {522, "heavy_rain"},
        {531, "rain"},

        {600, "light_snow"},
        {601, "snow"},
        {602, "heavy_snow"},
        {611, "snow"},
        {612, "snow"},
        {613, "snow"},
        {615, "snow"},
        {616, "snow"},
        {620, "snow"},
        {621, "snow"},
        {622, "heavy_snow"},

        {701, "fog"},
        {711, "fog"},
        {721, "fog"},
        {731, "fog"},
        {741, "fog"},
        {751, "fog"},
        {761, "fog"},
        {762, "fog"},
        {771, "fog"},
        {781, "fog"},

        {800, "sunny"},
        {801, "partly_cloudy"},
        {802, "partly_cloudy"},
        {803, "cloudy"},
        {804, "overcast"}
    };

    if (!map.contains(code)) {
        qDebug() << "⚠️ [OpenWeatherMap] Неизвестный код иконки:" << code;
    }

    return map.value(code, "sunny");
}
