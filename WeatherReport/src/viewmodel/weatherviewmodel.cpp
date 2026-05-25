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


    qDebug() << "ViewModel поймала поиск для города:" << cityName;

 
    m_visibleCityName = cityName.toUpper() + " (+22°C, Симуляция)";


    emit visibleCityNameChanged();
}
