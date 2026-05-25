import QtQuick 2.15
//Можно убрать
// ── Кнопка вкладки ────────────────────────────────────────────────────
Rectangle {
    id: tab
    property string text:        "Tab"
    property bool   isActive:    false
    property color  textColor:   "#E8EFF7"
    property color  secondColor: "#7A8FA8"
    property color  accentLine:  "#4FC3F7"
    signal clicked()

    width: labelText.width + 32
    height: 48
    color: "transparent"

    Text {
        id: labelText
        anchors.centerIn: parent
        text: tab.text
        font.pixelSize: 14
        font.weight: tab.isActive ? Font.Medium : Font.Normal
        color: tab.isActive ? tab.textColor : tab.secondColor
        Behavior on color { ColorAnimation { duration: 180 } }
    }

    // Активная линия снизу
    Rectangle {
        anchors { bottom: parent.bottom; horizontalCenter: parent.horizontalCenter }
        width: tab.isActive ? labelText.width : 0
        height: 2
        radius: 1
        color: tab.accentLine
        Behavior on width { SmoothedAnimation { duration: 220 } }
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: tab.clicked()
    }
}
