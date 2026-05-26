import QtQuick 2.15
import QtQuick.Controls 2.15

//Красивая кнопка
Rectangle {
    id: btn
    property string icon:        "×"
    property string tooltip:     ""
    property color  accentColor: "#7A8FA8"
    signal clicked()

    width: 32; height: 32; radius: 80
    color: ma.containsMouse ? Qt.rgba(accentColor.r, accentColor.g, accentColor.b, 0.15)
                            : "transparent"
    Behavior on color { ColorAnimation { duration: 140 } }

    Text {
        anchors.centerIn: parent
        text: btn.icon
        font.pixelSize: 14
        color: ma.containsMouse ? btn.accentColor : "#7A8FA8"
        Behavior on color { ColorAnimation { duration: 140 } }
    }

    ToolTip.visible: ma.containsMouse && btn.tooltip !== ""
    ToolTip.text:    btn.tooltip
    ToolTip.delay:   600

    MouseArea {
        id: ma
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: btn.clicked()
    }
}
