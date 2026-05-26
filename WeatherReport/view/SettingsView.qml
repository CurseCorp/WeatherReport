import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

//Настройки
Item {
    id: settingsView

    // ── Сигналы ───────────────────────────────────────────────────────
    signal close()

    //Цвета
    readonly property color bgDeep:      "#373641"
    readonly property color bgCard:      "#353333"
    readonly property color bgCardHover: "#1A2238"
    readonly property color accent:      "#4FC3F7"
    readonly property color accentWarm:  "#FFB347"
    readonly property color textPrimary: "#E8EFF7"
    readonly property color textSecond:  "#7A8FA8"
    readonly property color border:      "#353333"
    readonly property color danger:      "#EF5350"
    readonly property color success:     "#66BB6A"

    ColumnLayout {
        anchors { fill: parent; margins: 0 }
        spacing: 0

        //Заголовок
        Rectangle {
            Layout.fillWidth: true
            height: 60
            color: "transparent"

            RowLayout {
                anchors { fill: parent; leftMargin: 20; rightMargin: 12 }
                spacing: 10

                Text {
                    text: "⚙"
                    font.pixelSize: 18
                    color: settingsView.textSecond
                }
                Text {
                    text: "Настройки"
                    font.pixelSize: 17
                    font.weight: Font.Medium
                    color: settingsView.textPrimary
                    Layout.fillWidth: true
                }
                Rectangle {
                    width: 32; height: 32; radius: 8
                    color: closeHover.containsMouse
                           ? Qt.rgba(0.94, 0.33, 0.31, 0.15) : "transparent"
                    Behavior on color { ColorAnimation { duration: 140 } }
                    Text {
                        anchors.centerIn: parent
                        text: "✕"
                        font.pixelSize: 14
                        color: closeHover.containsMouse
                               ? settingsView.danger : settingsView.textSecond
                        Behavior on color { ColorAnimation { duration: 140 } }
                    }
                    MouseArea {
                        id: closeHover
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: settingsView.close()
                    }
                }
            }

            Rectangle {
                anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
                height: 1; color: settingsView.border
            }
        }

        //Тело настроек
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            clip: true

            ColumnLayout {
                width: parent.width
                spacing: 0

                //Единицы измерения: температура и скорость ветра
                SectionHeader { title: "ЕДИНИЦЫ ИЗМЕРЕНИЯ" }

                //Температура
                SettingsRow {
                    label:    "Температура"
                    sublabel: "Отображение в градусах"
                    rightContent: Row {
                        spacing: 0
                        UnitToggleButton {
                            text: "°C"
                            isActive: typeof settingsViewModel !== "undefined"
                                      ? settingsViewModel.isCelsius : true
                            accentColor: settingsView.accent
                            onToggled: if (typeof settingsViewModel !== "undefined")
                                           settingsViewModel.setUnit("celsius")
                        }
                        UnitToggleButton {
                            text: "°F"
                            isActive: typeof settingsViewModel !== "undefined"
                                      ? !settingsViewModel.isCelsius : false
                            accentColor: settingsView.accent
                            onToggled: if (typeof settingsViewModel !== "undefined")
                                           settingsViewModel.setUnit("fahrenheit")
                        }
                    }
                }

                //Скорость ветра
                SettingsRow {
                    label:    "Скорость ветра"
                    sublabel: "Единица скорости"
                    rightContent: Row {
                        spacing: 0
                        UnitToggleButton {
                            text: "м/с"
                            isActive: typeof settingsViewModel !== "undefined"
                                      ? settingsViewModel.windUnit === "ms" : true
                            accentColor: settingsView.accent
                            onToggled: if (typeof settingsViewModel !== "undefined")
                                           settingsViewModel.setWindUnit("ms")
                        }
                        UnitToggleButton {
                            text: "км/ч"
                            isActive: typeof settingsViewModel !== "undefined"
                                      ? settingsViewModel.windUnit === "kmh" : false
                            accentColor: settingsView.accent
                            onToggled: if (typeof settingsViewModel !== "undefined")
                                           settingsViewModel.setWindUnit("kmh")
                        }
                    }
                }

                //Настройка API
                SectionHeader { title: "API" }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.leftMargin: 20
                    Layout.rightMargin: 20
                    Layout.topMargin: 8
                    spacing: 8

                    Text {
                        text: "OpenWeatherMap API ключ"
                        font.pixelSize: 13
                        color: settingsView.textPrimary
                    }
                    Text {
                        text: "Получите ключ на openweathermap.org"
                        font.pixelSize: 11
                        color: settingsView.textSecond
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 44
                        radius: 10
                        color: settingsView.bgDeep
                        border.color: apiInput.activeFocus
                                      ? settingsView.accent : settingsView.border
                        border.width: apiInput.activeFocus ? 2 : 1
                        Behavior on border.color { ColorAnimation { duration: 200 } }

                        RowLayout {
                            anchors { fill: parent; leftMargin: 14; rightMargin: 10 }
                            spacing: 8
                            Text { text: "🔑"; font.pixelSize: 14 }
                            TextField {
                                id: apiInput
                                Layout.fillWidth: true
                                placeholderText: "Введите API ключ..."
                                font.pixelSize: 13
                                color: settingsView.textPrimary
                                background: Item {}
                                leftPadding: 0
                                placeholderTextColor: settingsView.textSecond
                                echoMode: showKey.checked
                                          ? TextInput.Normal : TextInput.Password
                                text: typeof settingsViewModel !== "undefined"
                                      ? settingsViewModel.apiKey : ""
                                onTextChanged: {
                                    if (typeof settingsViewModel !== "undefined")
                                        settingsViewModel.apiKey = text
                                }
                            }
                            CheckBox {
                                id: showKey
                                text: ""
                                indicator: Rectangle {
                                    width: 22; height: 22; radius: 6
                                    color: showKey.checked
                                           ? Qt.rgba(0.31,0.76,0.97,0.2) : "transparent"
                                    border.color: showKey.checked
                                                  ? settingsView.accent : settingsView.border
                                    Text {
                                        anchors.centerIn: parent
                                        text: "👁"
                                        font.pixelSize: 11
                                    }
                                }
                            }
                        }
                    }
                    Item { height: 4 }
                }

                //Обновления
                SectionHeader { title: "АВТООБНОВЛЕНИЕ" }

                SettingsRow {
                    label:    "Автообновление"
                    sublabel: "Обновлять данные автоматически"
                    rightContent: Switch {
                        checked: typeof settingsViewModel !== "undefined"
                                 ? settingsViewModel.autoRefresh : true
                        onToggled: {
                            if (typeof settingsViewModel !== "undefined")
                                settingsViewModel.autoRefresh = checked
                        }
                        indicator: Rectangle {
                            implicitWidth: 44; implicitHeight: 24; radius: 12
                            color: parent.checked ? settingsView.accent : settingsView.border
                            Behavior on color { ColorAnimation { duration: 200 } }
                            Rectangle {
                                x: parent.parent.checked ? parent.width - width - 3 : 3
                                y: 3; width: 18; height: 18; radius: 9
                                color: "white"
                                Behavior on x { SmoothedAnimation { duration: 200 } }
                            }
                        }
                    }
                }

                //Сохранение
                Rectangle {
                    Layout.fillWidth: true
                    Layout.leftMargin: 20
                    Layout.rightMargin: 20
                    Layout.topMargin: 24
                    height: 44
                    radius: 10
                    color: saveHover.containsMouse
                           ? Qt.lighter(settingsView.accent, 1.1) : settingsView.accent
                    Behavior on color { ColorAnimation { duration: 140 } }

                    Text {
                        anchors.centerIn: parent
                        text: "Сохранить"
                        font.pixelSize: 14
                        font.weight: Font.Medium
                        color: settingsView.bgDeep
                    }

                    MouseArea {
                        id: saveHover
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            if (typeof settingsViewModel !== "undefined")
                                settingsViewModel.saveSettings()
                            savedToast.visible = true
                            savedTimer.restart()
                            settingsView.close()
                        }
                    }
                }
            }
        }
    }

    //Всплывающее окно сохранения
    Rectangle {
        id: savedToast
        visible: false
        anchors { bottom: parent.bottom; horizontalCenter: parent.horizontalCenter;
                  bottomMargin: 20 }
        width: toastTxt.width + 32; height: 36; radius: 18
        color: settingsView.success

        Text {
            id: toastTxt
            anchors.centerIn: parent
            text: "✓  Настройки сохранены"
            font.pixelSize: 13
            color: "#0B0F1A"
            font.weight: Font.Medium
        }
        Timer {
            id: savedTimer
            interval: 2000
            onTriggered: savedToast.visible = false
        }
        NumberAnimation on opacity {
            running: savedToast.visible
            from: 0; to: 1; duration: 220
            easing.type: Easing.OutCubic
        }
    }

    //Вспомогательные компоненты

    component SectionHeader: Rectangle {
        property string title: ""
        Layout.fillWidth: true
        height: 36
        color: "transparent"
        Layout.topMargin: 8

        Text {
            anchors { left: parent.left; leftMargin: 20; verticalCenter: parent.verticalCenter }
            text: title
            font.pixelSize: 10
            font.weight: Font.Medium
            font.letterSpacing: 1.2
            color: settingsView.textSecond
        }
        Rectangle {
            anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
            height: 1; color: settingsView.border
        }
    }

    component SettingsRow: Rectangle {
        property string label:    ""
        property string sublabel: ""
        property Item   rightContent: null
        Layout.fillWidth: true
        height: 58
        color: "transparent"

        Row {
            anchors { left: parent.left; leftMargin: 20; verticalCenter: parent.verticalCenter }
            spacing: 0
            Column {
                spacing: 2
                Text {
                    text: label
                    font.pixelSize: 13
                    color: settingsView.textPrimary
                    font.weight: Font.Medium
                }
                Text {
                    text: sublabel
                    font.pixelSize: 11
                    color: settingsView.textSecond
                }
            }
        }
        Item {
            anchors { right: parent.right; rightMargin: 20; verticalCenter: parent.verticalCenter }
            children: rightContent ? [rightContent] : []
        }
        Rectangle {
            anchors { bottom: parent.bottom; left: parent.left; right: parent.right;
                      leftMargin: 20; rightMargin: 20 }
            height: 1; color: settingsView.border; opacity: 0.5
        }
    }

    component UnitToggleButton: Rectangle {
        property string text:        ""
        property bool   isActive:    false
        property color  accentColor: "#4FC3F7"
        signal toggled()
        width: 46; height: 30
        color: isActive ? accentColor : Qt.rgba(1,1,1,0.06)
        radius: 8
        Behavior on color { ColorAnimation { duration: 160 } }
        Text {
            anchors.centerIn: parent
            text: parent.text
            font.pixelSize: 12; font.weight: Font.Medium
            color: isActive ? settingsView.bgDeep : settingsView.textSecond
            Behavior on color { ColorAnimation { duration: 160 } }
        }
        MouseArea {
            anchors.fill: parent; cursorShape: Qt.PointingHandCursor
            onClicked: parent.toggled()
        }
    }
}
