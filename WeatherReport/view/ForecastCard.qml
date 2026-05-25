import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: card

    property string dayName:  "Пн"
    property string dateStr:  "1 янв"
    property string iconCode: "01d"
    property string tempMax:  "--°"
    property string tempMin:  "--°"
    property string humidity: "--%"

    width: 80
    height: 180
    radius: 14
    color: hoverArea.containsMouse ? "#1A2238" : "#131929"
    border.color: hoverArea.containsMouse ? "#4FC3F7" : "#1E2D42"
    border.width: 1

    Behavior on color        { ColorAnimation { duration: 180 } }
    Behavior on border.color { ColorAnimation { duration: 180 } }

    // Тень — смещённый прямоугольник позади карточки
    Rectangle {
        anchors.fill: parent
        anchors.margins: -2
        radius: parent.radius + 2
        color: Qt.rgba(0, 0, 0, hoverArea.containsMouse ? 0.45 : 0.25)
        z: 1
        transform: Translate { y: hoverArea.containsMouse ? 6 : 3 }
        Behavior on y { NumberAnimation { duration: 180 } }
    }

    // Анимация поднятия при наведении
    Behavior on y { SmoothedAnimation { velocity: 200 } }

    ColumnLayout {
        anchors { fill: parent; margins: 10 }
        spacing: 2

        Text {
            text: card.dayName
            font.pixelSize: 15; font.weight: Font.DemiBold
            color: "#E8EFF7"
            Layout.alignment: Qt.AlignHCenter
        }
        Text {
            text: card.dateStr
            font.pixelSize: 11; color: "#7A8FA8"
            Layout.alignment: Qt.AlignHCenter
        }

        Item { height: 2 }

        Text {
            text: getWeatherEmoji(card.iconCode)
            font.pixelSize: 44
            Layout.alignment: Qt.AlignHCenter
        }

        Item { height: 2 }

        Text {
            text: card.tempMax
            font.pixelSize: 22; font.weight: Font.Medium; color: "#E8EFF7"
            Layout.alignment: Qt.AlignHCenter
        }
        Text {
            text: card.tempMin
            font.pixelSize: 14; color: "#7A8FA8"
            Layout.alignment: Qt.AlignHCenter
        }

        Item { Layout.fillHeight: true }

    }

    MouseArea {
        id: hoverArea
        anchors.fill: parent
        hoverEnabled: true; cursorShape: Qt.PointingHandCursor
        onEntered: card.y = -4
        onExited:  card.y = 0
    }

    function getWeatherEmoji(code) {
        if (!code) return "🌡"
        var c = code.substring(0, 2)
        var map = { "01":"☀️","02":"⛅","03":"☁️","04":"☁️","09":"🌧","10":"🌦","11":"⛈","13":"❄️","50":"🌫" }
        return map[c] || "🌡"
    }
}
