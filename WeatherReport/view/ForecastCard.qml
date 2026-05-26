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
    color: hoverArea.containsMouse ? "#3A3A3A" : "#353333"
    border.color: hoverArea.containsMouse ? "#807C7C" : "#807C7C"
    border.width: 1

    Behavior on color        { ColorAnimation { duration: 180 } }
    Behavior on border.color { ColorAnimation { duration: 180 } }

    //Тень
    Rectangle {
        anchors.fill: parent; anchors.margins: -1
        radius: parent.radius + 1
        color: Qt.rgba(0, 0, 0, 0.28)
        z: -1
        transform: Translate { y: 3 }
    }

    //Анимация поднятия при наведении
    Behavior on y { SmoothedAnimation { velocity: 100 } }

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
