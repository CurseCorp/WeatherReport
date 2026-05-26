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


}
