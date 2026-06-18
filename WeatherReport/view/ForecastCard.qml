import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: card

    property string dateStr:  ""
    property var    hourly:   []   // массив объектов { time: "00:00", temp: "11.9" }

    width:  parent ? parent.width : 900
    height: 60
    radius: 0
    color:  "transparent"

    // Разделитель снизу
    Rectangle {
        anchors.left:   parent.left
        anchors.right:  parent.right
        anchors.bottom: parent.bottom
        height: 1
        color:  "#2E3340"
    }

    ColumnLayout {
        anchors { fill: parent; leftMargin: 12; rightMargin: 12; topMargin: 8; bottomMargin: 8 }
        spacing: 1

        Text {
            text: card.dateStr
            font.pixelSize: 13
            font.weight: Font.DemiBold
            color: "#E8EFF7"
        }

        Row {
            Layout.fillWidth: true
            spacing: 0

            Repeater {
                model: card.hourly

                Column {
                    width:   card.hourly.length > 0
                             ? (card.width - 24) / card.hourly.length
                             : 36
                    spacing: 1

                    Text {
                        width: parent.width
                        text:  modelData.time
                        font.pixelSize: 10
                        color: "#5A6478"
                        horizontalAlignment: Text.AlignHCenter
                    }

                    Text {
                        width: parent.width
                        text:  formatTemp(modelData.temp)
                        font.pixelSize: 9
                        color: "#4DA6FF"
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }
    }
}