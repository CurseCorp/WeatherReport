#include "weatherviewmodel.h"
#include <QDebug>
WeatherViewModel::WeatherViewModel(QObject *parent)
    : QObject(parent), m_visibleCityName("Москва (Ожидание поиска)") {
}

QString WeatherViewModel::visibleCityName() const {
    return m_visibleCityName;
}

void WeatherViewModel::searchCity(const QString &cityName) {
    if (cityName.isEmpty()) return;

    // В будущем здесь будет вызов Модели: model->getCurrentWeather(cityName)
    qDebug() << "ViewModel поймала поиск для города:" << cityName;

    // Пока симулируем работу: меняем текст и пинаем интерфейс
    m_visibleCityName = cityName.toUpper() + " (+22°C, Симуляция)";

    // Обязательно сообщаем QML, что данные изменились!
    emit visibleCityNameChanged();
}
