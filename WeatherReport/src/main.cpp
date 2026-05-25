#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext> // <-- ОБЯЗАТЕЛЬНО ДОБАВЬ ЭТОТ ИНКЛЮД
#include "viewmodel/WeatherViewModel.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // Создаем экземпляр твоей ViewModel
    WeatherViewModel viewModel;

    // Регистрируем её в QML под именем "weatherViewModel"
    engine.rootContext()->setContextProperty("weatherViewModel", &viewModel);

    const QUrl url(u"qrc:/WeatherReport/view/Main.qml"_qs);
    engine.load(url);

    return app.exec();
}
