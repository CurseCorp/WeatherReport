import QtQuick 2.15
import QtQuick.Layouts 1.15

// ── Карточка детального параметра (влажность, ветер, давление, видимость)
Rectangle {
    id: card
    property string icon:  "💧"
    property string label: "Параметр"
    property string value: "--"
    property color  color: "#4FC3F7"

    Layout.fillWidth: true
    height: 68
    radius: 10
    border.color: "#1E2D42"
    border.width: 1

    gradient: Gradient {
        orientation: Gradient.Horizontal
        GradientStop { position: 0.0; color: Qt.rgba(card.color.r, card.color.g, card.color.b, 0.08) }
        GradientStop { position: 1.0; color: "#131929" }
    }

    RowLayout {
        anchors {leftMargin: 12; rightMargin: 12 }
        spacing: 11

        Text { text: card.icon; font.pixelSize: 22 }
        Text {
            text: card.label
            font.pixelSize: 11
            color: "#7A8FA8"
        }
        Text {
            text: card.value
            font.pixelSize: 16
            font.weight: Font.Medium
            color: "#E8EFF7"
        }

    }
}
