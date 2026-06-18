import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: card

    property string timeStr: ""
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
        }

        Text {
            text: formatTemp(tempStr)
            font.pixelSize: 16
            font.bold: true
            color: "#4DA6FF"
            Layout.alignment: Qt.AlignHCenter
        }
    }
}