import QtQuick
import QtQuick.Controls

Window {
    width: 400
    height: 300
    visible: true
    title: "Weather Report - MVVM Test"
    color: "#1e1e1e"

    Column {
        anchors.centerIn: parent
        spacing: 20

        Text {
            text: weatherViewModel.visibleCityName
            color: "#ffffff"
            font.pointSize: 18
            anchors.horizontalCenter: parent
        }

        TextField {
            id: cityInput
            placeholderText: "Введите город..."
            color: "#000000"
            width: 200
        }

        Button {
            text: "Искать погоду"
            anchors.horizontalCenter: parent
            onClicked: weatherViewModel.searchCity(cityInput.text)
        }


        Text{
            text: weatherViewModel.tempText
            color: "#123123"
            font.pointSize: 22
        }
    }
}
