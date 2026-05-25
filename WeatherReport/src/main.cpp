#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <memory>

#include "model/entities/WeatherService.h"
#include "viewmodel/WeatherViewModel.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    auto globalWeatherService = std::make_shared<WeatherService>();

    auto weatherViewModel = std::make_unique<WeatherViewModel>(globalWeatherService);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("weatherViewModel", weatherViewModel.get());

    const QUrl url(QStringLiteral("qrc:/WeatherReport/view/Main.qml"));
    engine.load(url);

    return app.exec();
}
