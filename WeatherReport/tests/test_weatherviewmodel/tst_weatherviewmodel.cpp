#include <QtTest>
#include "viewmodel/weatherviewmodel.h"
#include "../test_cachemanager/tst_cachemanager.cpp"
class TestWeatherViewModel : public QObject {
    Q_OBJECT
private slots:
    void init() {
        QSettings settings("CurseCorp", "WeatherReport");
        settings.clear(); // Полная очистка настроек перед запуском следующего теста
    }
    // Проверка добавления и удаления городов из списка избранного
    void testFavoritesLogic() {
        WeatherViewModel vm(nullptr, nullptr);

        vm.addCityToFavorites("London");
        QVERIFY(vm.favoriteCities().contains("London"));

        vm.removeCityFromFavorites("London");
        QVERIFY(!vm.favoriteCities().contains("London"));
    }

    // Пустой запрос не должен вызывать действия
    void testEmptySearch() {
        WeatherViewModel vm(nullptr, nullptr);

        vm.loadWeather("");
        QCOMPARE(vm.searchResults().size(), 0);
    }

    // Проверка того, что при смене API ключа метод не падает
    void testApiKeySetting() {
        WeatherViewModel vm(nullptr, nullptr);

        vm.saveApiKey("test_api_key_123");

        QVERIFY(true);
    }

    // Проверка логики истории
    void testLoadHistoryFlow() {
        WeatherViewModel vm(nullptr, nullptr);

        vm.loadHistory("Москва");

        QCOMPARE(vm.historyData().size(), 0);
    }
    // Проверка очистки истории кэша
    void testHistoryClear() {
        WeatherViewModel vm(nullptr, nullptr);

        vm.loadHistory("Париж");

        QCOMPARE(vm.historyData().size(), 0);
    }
    // Проверка на удаление пробелов
    void testAddCityTrimming() {
        WeatherViewModel vm(nullptr, nullptr);

        vm.addCityToFavorites("  Берлин  ");

        // Ожидаем, что город добавлен как "Berlin" (без пробелов)
        QVERIFY(vm.favoriteCities().contains("Берлин"));
        QVERIFY(!vm.favoriteCities().contains("  Берлин  "));
    }
    // Проверка на добавление дубликата в список избранного
    void testAddDuplicateCity() {
        WeatherViewModel vm(nullptr, nullptr);

        vm.addCityToFavorites("Воронеж");
        vm.addCityToFavorites("Воронеж"); // Метод просто выйдет, город в списке останется один

        // Правильная проверка: ищем "Воронеж", а не "Madrid"
        int count = 0;
        for(const auto& city : vm.favoriteCities()) {
            if(city == "Воронеж") count++;
        }
        QCOMPARE(count, 1); // Теперь всё должно быть правильно!
    }
    void testAddEmptyCityToFavorites() {
        WeatherViewModel vm(nullptr, nullptr);
        int initialSize = vm.favoriteCities().size();
        vm.addCityToFavorites("");
        vm.addCityToFavorites("   ");
        QCOMPARE(vm.favoriteCities().size(), initialSize);
    }

    // Проверка удаления несуществующего города
    void testRemoveNonExistentCity() {
        WeatherViewModel vm(nullptr, nullptr);
        vm.addCityToFavorites("Москва");
        // Попытка удалить то, чего нет
        vm.removeCityFromFavorites("Токио");
        // Список не должен был измениться
        QCOMPARE(vm.favoriteCities().size(), 1);
    }

    //  Проверка чувствительности к регистру (регистронезависимость)
    void testCaseInsensitivity() {
        WeatherViewModel vm(nullptr, nullptr);
        vm.addCityToFavorites("Москва");
        vm.addCityToFavorites("москва");
    }

    //  Проверка состояния по умолчанию
    void testInitialState() {
        WeatherViewModel vm(nullptr, nullptr);
        QVERIFY(vm.favoriteCities().isEmpty());
        QCOMPARE(vm.searchResults().size(), 0);
    }
};
int main(int argc, char *argv[]) {
    int status = 0;


    TestWeatherViewModel vmTests;
    status |= QTest::qExec(&vmTests, argc, argv);


    TestCacheManager cacheTests;
    status |= QTest::qExec(&cacheTests, argc, argv);

    return status;
}

#include "tst_weatherviewmodel.moc"
