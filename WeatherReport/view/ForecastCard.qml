import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: card

    property string timeStr: ""
    property string iconImage: ""
    property var tempStr

    width: 70
    height: 160

    radius: 10
    color: "#353333"

    border.color: "#807C7C"
    border.width: 1

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 10

        Text {
            text: timeStr
            font.pixelSize: 14
            color: "#E8EFF7"
            Layout.alignment: Qt.AlignHCenter
            z: 1
        }
        Image{
            source:getWeatherImage(iconImage)
            anchors.centerIn: parent
            height: 30
            width: 30
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            anchors.verticalCenter: parent.verticalCenter
        }
        Text {
            text: formatTemp(tempStr)
            font.pixelSize: 16
            font.bold: true
            color: root.textPrimary
            Layout.alignment: Qt.AlignHCenter
            z: 1
        }
    }
    function getWeatherImage(code) {
        if (!code) return "🌡";

        // 1. Извлекаем часы из строки (предполагаем формат "HH:mm" или "H:mm")
        var hour = parseInt(timeStr.split(":")[0]);
        var isNight = (hour >= 23 || hour <= 4);

        // 2. Создаем карту
        var map = {
            // Используем тернарный оператор для выбора иконки для sunny
            "sunny": (isNight ? "images/night.png" : "images/sun.png"),
            "partly_cloudy": "images/partlycloudy.png",
            "cloudy": "images/overcast.png",
            "overcast": "images/overcast.png",
            "fog": "images/fog.png",
            "drizzle": "images/drizzle.png",
            "thunderstorm": "images/thunderstorm.png",
            "snow": "images/snow.png",
            "light_snow": "images/snow.png",
            "heavy_snow": "images/snow.png",
            "light_rain": "images/rain.png",
            "heavy_rain": "images/rain.png",
            "rain": "images/rain.png"
        };

        return map[code] || "images/pressure";
    }
}
