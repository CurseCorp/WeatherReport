#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <memory>
#include "viewmodel/WeatherViewModel.h"
#include "model/services/weatherapi.h"
#include "model/services/geocodingservice.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    auto weatherService = std::make_shared<WeatherApi>();
    auto geoService = std::make_shared<GeocodingService>();
    auto weatherViewModel = std::make_unique<WeatherViewModel>(weatherService, geoService);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("weatherViewModel", weatherViewModel.get());

    const QUrl url(QStringLiteral("qrc:/qt/qml/WeatherReport/view/Main.qml"));
    engine.load(url);

    return app.exec();
}
