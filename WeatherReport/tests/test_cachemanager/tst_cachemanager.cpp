#include <QtTest>
#include "src/viewmodel/CacheManager.h"
#include "src/model/entities/WeatherData.h"

class TestCacheManager : public QObject {
    Q_OBJECT
private slots:

    void init() {
        QSettings settings("CurseCorp", "WeatherReport");
        settings.clear();
    }
    // Тест сериализации и десериализации
    void testSaveAndLoad() {
        WeatherData data;
        data.cityName = "Москва";
        data.temperatureCurrent = 20.0;
        data.isValid = true;

        CacheManager::save("Москва", "current", data);
        WeatherData loaded = CacheManager::load("Москва", "current");

        QCOMPARE(loaded.cityName, data.cityName);
        QCOMPARE(loaded.temperatureCurrent, data.temperatureCurrent);
        QVERIFY(loaded.isValid);
    }
    // Тест времени жизни кэша
    void testExpiration() {
        QString city = "Москва";
        CacheManager::updateCacheTimestamp(city);
        QVERIFY(!CacheManager::isCacheExpired(city));
    }
    // Тест обработки ошибок и пустых данных
    void testLoadEmpty() {
        WeatherData loaded = CacheManager::load("НеизвестныйГород", "current");
        QVERIFY(!loaded.isValid);
    }
    //  Проверка удаления старых записей истории ( Ротация 7 дней истории)
    void testHistoryRotation() {
        QString city = "RotationCity";
        WeatherData data;

        DailyForecast df;
        df.date = "2026-06-01";
        df.tempMin = 10.0;
        df.tempMax = 20.0;
        df.description = "Sunny";
        df.humidity = 50;
        df.windSpeedMs = 5.0;
        df.pressure = 1013;
        df.precipitationMm = 0.0;
        df.icon = "01d";

        data.dailyForecasts.append(df);
        for (int i = 1; i <= 8; ++i) {
            QDate date = QDate(2026, 6, 1).addDays(i);

            data.dailyForecasts[0].date = date.toString("yyyy-MM-dd");
            CacheManager::saveHistory(city, date, data);
        }

        QString dirPath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
                          "/WeatherReportApp/" + city + "/history";
        QDir dir(dirPath);
        int fileCount = dir.entryList({"*.json"}, QDir::Files).size();

        QCOMPARE(fileCount, 7);
    }

    // Проверка истечения кэша по времени
    void testCacheExpirationLogic() {
        QString city = "ExpiredCity";

        CacheManager::updateCacheTimestamp(city);
        QVERIFY(!CacheManager::isCacheExpired(city));

        QSettings settings("CurseCorp", "WeatherReport");
        settings.remove(city + "/lastUpdate");

        QVERIFY(CacheManager::isCacheExpired(city));
    }

    //  Сохранение и загрузка истории конкретного дня
    void testSaveLoadHistory() {
        QString city = "Moscow";
        QDate date(2026, 6, 20);
        WeatherData data;

        DailyForecast day;
        day.date = "2026-06-20";
        day.tempMin = 15.0;
        day.tempMax = 25.0;
        data.dailyForecasts.append(day);

        CacheManager::saveHistory(city, date, data);
        WeatherData loaded = CacheManager::loadHistory(city, date);

        QVERIFY(loaded.isValid);
        QCOMPARE(loaded.dailyForecasts.first().tempMin, 15.0);
    }
};
#include "tst_cachemanager.moc"
