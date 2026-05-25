import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: item

    property string cityName:    "—"
    property string country:     "—"
    property string temperature: "--°"
    property int    itemIndex:   0

    signal selectCity()
    signal removeCity()

    width:  parent ? parent.width : 400
    height: 64
    radius: 12
    color:  rowHover.containsMouse ? "#1A2238" : "#131929"
    border.color: rowHover.containsMouse ? "#4FC3F7" : "#1E2D42"
    border.width: 1

    Behavior on color        { ColorAnimation { duration: 160 } }
    Behavior on border.color { ColorAnimation { duration: 160 } }

    // Тень
    Rectangle {
        anchors.fill: parent; anchors.margins: -1
        radius: parent.radius + 1
        color: Qt.rgba(0, 0, 0, 0.28)
        z: -1
        transform: Translate { y: 3 }
    }

    RowLayout {
        anchors { fill: parent; leftMargin: 16; rightMargin: 12 }
        spacing: 12

        Rectangle {
            width: 38; height: 38; radius: 10
            color: Qt.rgba(0.31, 0.76, 0.97, 0.12)
            Text { anchors.centerIn: parent; text: "📍"; font.pixelSize: 18 }
        }

        ColumnLayout {
            Layout.fillWidth: true; spacing: 2
            Text { text: item.cityName; font.pixelSize: 14; font.weight: Font.Medium; color: "#E8EFF7"; elide: Text.ElideRight; Layout.fillWidth: true }
            Text { text: item.country; font.pixelSize: 12; color: "#7A8FA8" }
        }

        Text { text: item.temperature; font.pixelSize: 20; font.weight: Font.Light; color: "#E8EFF7" }

        // Кнопка удаления
        Rectangle {
            id: deleteBtn
            width: 32; height: 32; radius: 8
            color: deleteHover.containsMouse ? Qt.rgba(0.94, 0.33, 0.31, 0.2) : "transparent"
            Behavior on color { ColorAnimation { duration: 140 } }
            Text {
                anchors.centerIn: parent; text: "✕"; font.pixelSize: 13
                color: deleteHover.containsMouse ? "#EF5350" : "#7A8FA8"
                Behavior on color { ColorAnimation { duration: 140 } }
            }
            MouseArea {
                id: deleteHover; anchors.fill: parent
                hoverEnabled: true; cursorShape: Qt.PointingHandCursor
                onClicked: removeAnim.start()
            }
            SequentialAnimation {
                id: removeAnim
                NumberAnimation { target: item; property: "opacity"; to: 0; duration: 180; easing.type: Easing.InQuad }
                ScriptAction { script: item.removeCity() }
            }
        }
    }

    MouseArea {
        id: rowHover
        anchors { fill: parent; rightMargin: 44 }
        hoverEnabled: true; cursorShape: Qt.PointingHandCursor
        onClicked: item.selectCity()
    }
}
