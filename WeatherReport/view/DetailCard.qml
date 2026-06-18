import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: card
    property string icon: "humidity.png"
    property string label: "Параметр"
    property string value: "--"
    property color  color: "#4FC3F7"

    Layout.fillWidth: true
    height: 64
    radius: 10
    border.color: "#1E2D42"
    border.width: 1
    gradient: Gradient {
        orientation: Gradient.Horizontal
        GradientStop { position: 0.0; color: Qt.rgba(card.color.r, card.color.g, card.color.b, 0.08) }
        GradientStop { position: 1.0; color: "#131929" }
    }

    Row {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 12
        spacing: 12

        Image {
            source: card.icon
            width: 22
            height: 22
            fillMode: Image.PreserveAspectFit
            anchors.verticalCenter: parent.verticalCenter
        }

        Column {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 2

            Text {
                text: card.label
                font.pixelSize: 14
                font.weight: Font.Medium
                color: "#E8EFF7"
            }
            Text {
                text: card.value
                font.pixelSize: 11
                color: "#7A8FA8"
            }
        }
    }
}