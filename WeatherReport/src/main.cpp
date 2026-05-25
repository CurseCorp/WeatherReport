#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "viewmodel/WeatherViewModel.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    WeatherViewModel viewModel;
    engine.rootContext()->setContextProperty("weatherViewModel", &viewModel);

    const QUrl url(u"qrc:/WeatherReport/view/Main.qml"_qs);
    engine.load(url);

    return app.exec();
}
