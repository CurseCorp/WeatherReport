import QtQuick
import QtQuick.Controls

Window {
    width: 400
    height: 400 // Увеличил высоту, чтобы все поместилось
    visible: true
    title: "Weather Report - MVVM Test"
    color: "#1e1e1e"

    Column {
        anchors.centerIn: parent
        spacing: 15

        Text {
            id: cityNameText
            text: weatherViewModel.visibleCityName
            color: "#ffffff"
            font.pointSize: 20
            font.bold: true
            anchors.horizontalCenter: parent
        }

        // Блок с параметрами погоды
        Column {
            spacing: 5
            anchors.horizontalCenter: parent

            Text { text: "Температура: " + weatherViewModel.tempText; color: "#ffcc00"; font.pointSize: 14 }
                Text { text: "Давление: " + weatherViewModel.pressure; color: "#cccccc"; font.pointSize: 12 }
                Text { text: "Скорость ветра: " + weatherViewModel.windSpeed; color: "#cccccc"; font.pointSize: 12 }
                Text { text: "Влажность: " + weatherViewModel.humidity; color: "#cccccc"; font.pointSize: 12 }
        }

        TextField {
            id: cityInput
            placeholderText: "Введите город..."
            anchors.horizontalCenter: parent
            width: 200
        }

        Button {
            text: "Искать погоду"
            anchors.horizontalCenter: parent
            onClicked: weatherViewModel.loadWeather(cityInput.text)
        }
    }
}
