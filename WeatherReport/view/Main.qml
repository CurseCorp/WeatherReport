import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15


Window {
    id: root
    width: 1200
    height: 720
    minimumWidth: 900
    minimumHeight: 600
    visible: true
    title: "WeatherApp"
    color: root.bgDeep
    flags: Qt.Window | Qt.FramelessWindowHint

    // Локальное UI-состояние (которого нет в C++ ViewModel)
    property bool   addMode: false
    property int    counterbutton: 0

    property bool   isMs: true                       // единицы скорости ветра — чисто визуально, конвертация на стороне QML
    property bool   isCelsius: true                   // TODO: пока неактивно — tempText уже приходит из C++ с готовым "°C"
    property bool   isOffline: false                  // TODO: нужен сигнал об ошибке сети в ViewModel, чтобы это иметь смысл
    property string apiKey: ""                        // TODO: нет в ViewModel — поле-заглушка для совместимости с SettingsView
    property bool   autoRefresh: true                  // TODO: нет в ViewModel — поле-заглушка для совместимости с SettingsView

    property bool   isLoadingLocal: false
    property string lastUpdatedLocal: ""

    property bool showPastForecast: false

    Component.onCompleted: {
        requestWeather("Москва")
    }

    // Оборачиваем вызов C++ метода, чтобы локально подсвечивать BusyIndicator
    function requestWeather(city) {
        isLoadingLocal = true
        if (typeof weatherViewModel !== "undefined")
            weatherViewModel.loadWeather(city)
    }
    function refWeather(city) {
        if (typeof weatherViewModel !== "undefined")
            weatherViewModel.refreshWeather(city)
    }

    // Реагируем на сигналы реального ViewModel
    Connections {
        target: typeof weatherViewModel !== "undefined" ? weatherViewModel : null
        function onWeatherUpdated() {
            isLoadingLocal = false
            lastUpdatedLocal = Qt.formatTime(new Date(), "hh:mm")
        }
    }
    function searchCity(name)   {}
    function selectCity(c, co)  {}
    function switchUnitsTemp()      { isCelsius = !isCelsius }
    function switchUnitsSpeed()      { isMs = !isMs }
    function refreshWeather()   {}
    function removeFavorite(i)  { favoritesModel.remove(i) }
    function toggleFavorite()   {}
    function celsiusToFahrenheit(c) {
                return Math.round((c * 9 / 5 + 32)*10)/10
            }

    function formatTemp(celsiusValue) {
        var value = isCelsius ? Math.round(celsiusValue) : celsiusToFahrenheit(celsiusValue)
        var sign = value >= 0 ? "+" : ""
        return sign + value + "°" + (isCelsius ? "C" : "F")
    }
    // Конвертация скорости ветра м/с -> км/ч (чисто визуальная, без участия C++)
    function msToKmh(raw) {
        var n = parseFloat(raw)
        if (isNaN(n)) return raw
        return Math.round(n * 3.6 * 10) / 10
    }
    function formatSpeed(rawValue) {
        if (isMs) return rawValue + " м/с"
        return msToKmh(rawValue) + " км/ч"
    }

    // Подбор иконки по текстовому описанию погоды (iconCode в ViewModel пока нет)
    function getWeatherEmoji(description) {
        var d = (description || "").toLowerCase()
        if (d.indexOf("гроза") !== -1)                          return "images/storm.png"
        if (d.indexOf("снег") !== -1)                            return "images/snow.png"
        if (d.indexOf("дожд") !== -1 || d.indexOf("ливен") !== -1) return "images/rain.png"
        if (d.indexOf("облач") !== -1 || d.indexOf("пасмурно") !== -1) return "images/cloud.png"
        if (d.indexOf("туман") !== -1)                           return "images/fog.png"
        if (d.indexOf("ясно") !== -1 || d.indexOf("солнечно") !== -1) return "images/sun.png"
        return "images/sun.png"
    }

    // Текущая дата на русском (в ViewModel нет полей weekday/day/month)
    function currentDateText() {
        var days = ["Воскресенье", "Понедельник", "Вторник", "Среда", "Четверг", "Пятница", "Суббота"]
        var months = ["января", "февраля", "марта", "апреля", "мая", "июня",
                       "июля", "августа", "сентября", "октября", "ноября", "декабря"]
        var now = new Date()
        return days[now.getDay()] + ", " + now.getDate() + " " + months[now.getMonth()]
    }


    function toggleForecastMode() {
        showPastForecast = !showPastForecast
        // Обновляем заголовок
        forecastTitle.text = showPastForecast ? "Прогноз на 7 дней назад" : "Прогноз на 7 дней"
        // Здесь можно добавить логику для загрузки данных за прошлые дни
        // если это необходимо
    }


    // Цветовая палитра
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

    MouseArea {
        id: dragArea
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 54
        property point clickPos
        onPressed: clickPos = Qt.point(mouse.x, mouse.y)
        onPositionChanged: {
            root.x += mouse.x - clickPos.x
            root.y += mouse.y - clickPos.y
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Заголовок
        Rectangle {
            Layout.fillWidth: true
            height: 54
            color: root.bgCard

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 24
                anchors.rightMargin: 16
                spacing: 12

                Row {
                    spacing: 8
                    Text {
                        text: "☁"
                        font.pixelSize: 22
                        color: root.accent
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Text {
                        text: "WeatherReport"
                        font.pixelSize: 16
                        font.weight: Font.DemiBold
                        font.letterSpacing: 1
                        color: root.textPrimary
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                Item { Layout.fillWidth: true }

                Text {
                    text: lastUpdatedLocal.length > 0 ? "Обновлено: " + lastUpdatedLocal : ""
                    font.pixelSize: 11
                    color: root.textSecond
                    Layout.alignment: Qt.AlignHCenter
                }

                TitleButton {
                    icon: "⟳"
                    tooltip: "Обновить"
                    accentColor: root.accent
                    onClicked: {
                        rotateAnim.restart()
                        if (typeof weatherViewModel !== "undefined")
                            refWeather(weatherViewModel.cityNameText)

                    }
                    RotationAnimator on rotation {
                        id: rotateAnim
                        running: false
                        from: 0
                        to: 360
                        duration: 700
                        easing.type: Easing.InOutQuad
                    }
                }
                TitleButton {
                    icon: "─"
                    tooltip: "Свернуть"
                    accentColor: root.textSecond
                    onClicked: root.showMinimized()
                }
                TitleButton {
                    icon: "✕"
                    tooltip: "Закрыть"
                    accentColor: root.danger
                    onClicked: Qt.quit()
                }
            }
        }

        // Левый бар с настройками
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0
            Rectangle {
                Layout.preferredWidth: 40
                Layout.fillHeight: true
                color: root.bgCard

                Column {
                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.topMargin: 20

                    TitleButton {
                        icon: "≡"
                        tooltip: "Настройки"
                        accentColor: root.textSecond
                        onClicked: settingsDrawer.open()
                    }
                }
            }
            Rectangle {
                Layout.alignment: Qt.AlignRight
                height: 700
                color: root.bgCard
            }
        }

        // Город + дата
        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 50
            anchors.topMargin: 60

            Layout.preferredWidth: 600
            height: 100
            radius: 14
            color: "transparent"
            clip: true

            Text {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.margins: 20
                anchors.leftMargin: 30
                text: typeof weatherViewModel !== "undefined" ? weatherViewModel.cityNameText : "—"
                font.pixelSize: 30
                font.weight: Font.Medium
                color: root.textPrimary
                elide: Text.ElideRight
                Layout.fillWidth: true
            }

            Text {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.margins: 55
                anchors.leftMargin: 30
                text: currentDateText()
                font.pixelSize: 15
                font.weight: Font.Small
                color: root.textPrimary
                elide: Text.ElideRight
                Layout.fillWidth: true
            }
        }

        // Основная погода
        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 70
            anchors.topMargin: 170

            Layout.preferredWidth: 470
            height: 260
            radius: 14
            color: root.bgCard
            clip: true

            BusyIndicator {
                anchors.centerIn: parent
                running: isLoadingLocal
                visible: running
                palette.dark: root.accent
            }

            Row {
                anchors.fill: parent
                anchors.margins: 35
                spacing: -15

                Column {
                    spacing: 4
                    anchors.verticalCenter: parent.verticalCenter

                    Text {
                        text: typeof weatherViewModel !== "undefined" ? formatTemp(weatherViewModel.tempText) : "--°"
                        font.pixelSize: 48
                        font.weight: Font.Light
                        color: root.textPrimary
                    }

                    Text {
                        text: typeof weatherViewModel !== "undefined"
                              ? "Мин " + formatTemp(weatherViewModel.minTemp) + "  /  Макс " + formatTemp(weatherViewModel.maxTemp)
                              : ""
                        font.pixelSize: 12
                        color: root.textSecond
                    }

                    Text {
                        text: typeof weatherViewModel !== "undefined" ? weatherViewModel.description : ""
                        font.pixelSize: 13
                        color: root.textSecond
                    }
                }

                Image {
                    source: typeof weatherViewModel !== "undefined"
                            ? getWeatherEmoji(weatherViewModel.description) : "images/sun.png"
                    width: 280
                    height: 280
                    fillMode: Image.PreserveAspectFit
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }

        // 4 свойства
        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 560
            anchors.topMargin: 170

            Layout.preferredWidth: 170
            height: 260
            radius: 14
            color: root.bgCard
            clip: true
            GridLayout {
                columns: 1
                rowSpacing: 3
                columnSpacing: 3
                anchors.left: parent.left
                anchors.leftMargin: 10
                DetailCard {
                    Layout.alignment: Qt.AlignHCenter
                    icon: "images/humidity.png"
                    label: "Влажность"
                    value: typeof weatherViewModel !== "undefined" ? weatherViewModel.humidity : "--"
                    color: root.accent
                }
                DetailCard {
                    Layout.alignment: Qt.AlignHCenter
                    icon: "images/windSpeed.png"
                    label: "Ветер"
                    value: typeof weatherViewModel !== "undefined" ? formatSpeed(weatherViewModel.windSpeed) : "--"
                    color: root.success
                }
                DetailCard {
                    Layout.alignment: Qt.AlignHCenter
                    icon: "images/pressure.png"
                    label: "Давление"
                    value: typeof weatherViewModel !== "undefined" ? weatherViewModel.pressure : "--"
                    color: root.accentWarm
                }
                DetailCard {
                    Layout.alignment: Qt.AlignHCenter
                    icon: "images/precipitation.png"
                    label: "Осадки"
                    value: typeof weatherViewModel !== "undefined" ? weatherViewModel.precipitation : "--"
                    color: "#CE93D8"
                }
            }
        }

        // Дни погоды
        Rectangle {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.leftMargin: 70
            anchors.bottomMargin: 30

            width: 660
            height: 230
            radius: 14
            color: root.bgCard
            clip: true

            Text {
                id: forecastTitle
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 10
                text: "Прогноз на 3 дня"
                font.pixelSize: 16
                font.weight: Font.Medium
                color: root.textPrimary
            }

            ListView {
                id: forecastListView
                anchors.top: forecastTitle.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 8
                anchors.topMargin: 4
                clip: true
                model: typeof weatherViewModel !== "undefined" ? weatherViewModel.forecastModel : null
                orientation: ListView.Vertical

                delegate: ForecastCard {
                    width:   forecastListView.width
                    dateStr: modelData.date
                    hourly:  modelData.hourly
                }
            }
        }

        // Поисковая строка / добавление в избранное
        // Поиска с подсказками в ViewModel нет (searchCity не реализован) — просто вводим
        // название города и сразу добавляем его в избранное через addCityToFavorites().
        Rectangle {
            id: searchAreaWrapper
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 755
            anchors.topMargin: 115

            Layout.preferredWidth: root.addMode ? 520 : 400
            width: root.addMode ? 520 : 400
            height: 40
            radius: 14
            color: "transparent"

            Behavior on width {
                NumberAnimation { duration: 200; easing.type: Easing.OutCubic }
            }

            Rectangle {
                id: searchBoxOuter
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: root.addMode ? addFavBtn.left : parent.right
                anchors.rightMargin: root.addMode ? 8 : 0
                height: 40
                radius: 14
                color: root.addMode ? "#EDEAEA" : root.bgCard
                border.color: root.addMode
                              ? (searchInput.activeFocus ? root.accent : root.border)
                              : root.bgCard
                border.width: root.addMode ? (searchInput.activeFocus ? 2 : 1) : 1
                clip: true

                Behavior on color { ColorAnimation { duration: 200 } }
                Behavior on border.color { ColorAnimation { duration: 200 } }

                Rectangle {
                    id: searchBox
                    anchors.fill: parent
                    radius: 14
                    color: "transparent"

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 14
                        anchors.rightMargin: 8
                        spacing: 8

                        Text {
                            text: "🔍"
                            font.pixelSize: 14
                            color: root.addMode ? root.textSecond : Qt.rgba(0.47, 0.56, 0.66, 0.5)
                            Behavior on color { ColorAnimation { duration: 200 } }
                        }

                        TextField {
                            id: searchInput
                            Layout.fillWidth: true
                            placeholderText: root.addMode ? "Введите город для добавления..." : "Нажмите +"
                            font.pixelSize: 14
                            color: root.addMode ? "#1A1A2E" : root.textSecond
                            background: basic
                            leftPadding: 0
                            placeholderTextColor: root.addMode ? root.textSecond : Qt.rgba(0.47, 0.56, 0.66, 0.45)
                            enabled: root.addMode
                            readOnly: !root.addMode

                            Behavior on color { ColorAnimation { duration: 200 } }

                            Keys.onReturnPressed: {
                                if (!root.addMode) return
                                var cityText = text.trim()
                                if (cityText.length === 0) return
                                if (typeof weatherViewModel !== "undefined")
                                    weatherViewModel.addCityToFavorites(cityText)
                                text = ""
                                root.addMode = false
                            }
                            Keys.onEscapePressed: {
                                focus = false
                                root.addMode = false
                                text = ""
                            }
                        }

                        Rectangle {
                            width: 22
                            height: 22
                            radius: 11
                            color: clearHover.containsMouse ? Qt.rgba(1, 1, 1, 0.1) : "transparent"
                            visible: searchInput.text.length > 0 && root.addMode
                            Text {
                                anchors.centerIn: parent
                                text: "✕"
                                font.pixelSize: 11
                                color: root.textSecond
                            }
                            MouseArea {
                                id: clearHover
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                onClicked: searchInput.text = ""
                            }
                        }
                    }
                }
            }

            Rectangle {
                id: addFavBtn
                anchors.top: parent.top
                anchors.right: parent.right
                width: root.addMode ? 106 : 0
                height: 40
                radius: 14
                clip: true
                color: addBtnHover.containsMouse
                       ? Qt.rgba(0.40, 0.76, 0.42, 0.25)
                       : Qt.rgba(0.40, 0.73, 0.42, 0.18)
                border.color: root.success
                border.width: 1
                opacity: root.addMode ? 1 : 0

                Behavior on width   { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
                Behavior on opacity { NumberAnimation { duration: 150 } }

                RowLayout {
                    anchors.centerIn: parent
                    spacing: 5
                    visible: root.addMode
                    Text {
                        text: "✓"
                        font.pixelSize: 14
                        font.weight: Font.Medium
                        color: root.success
                    }
                    Text {
                        text: "Добавить"
                        font.pixelSize: 13
                        font.weight: Font.Medium
                        color: root.success
                    }
                }

                MouseArea {
                    id: addBtnHover
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        if (!root.addMode) return
                        var cityText = searchInput.text.trim()
                        if (cityText.length === 0) return
                        if (typeof weatherViewModel !== "undefined")
                            weatherViewModel.addCityToFavorites(cityText)
                        searchInput.text = ""
                        root.addMode = false
                    }
                }
            }
        }

        // Избранное
        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 755
            anchors.topMargin: 170

            Layout.preferredWidth: 400
            height: 520
            radius: 14
            color: root.bgCard
            clip: true
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 16

                RowLayout {
                    Text {
                        text: "Избранные города"
                        font.pixelSize: 16
                        font.weight: Font.Medium
                        color: root.textPrimary
                        Layout.fillWidth: true
                    }
                    Rectangle {
                        width: favCountTxt.width + 18
                        height: 22
                        radius: 11
                        color: Qt.rgba(0.31, 0.76, 0.97, 0.15)
                        Text {
                            id: favCountTxt
                            anchors.centerIn: parent
                            text: typeof weatherViewModel !== "undefined"
                                  ? weatherViewModel.favoriteCities.length : "0"
                            font.pixelSize: 11
                            font.weight: Font.Medium
                            color: root.accent
                        }
                    }
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                    ScrollBar.vertical.policy: ScrollBar.AlwaysOff
                    clip: true

                    ColumnLayout {
                        width: parent.width
                        spacing: 8

                        Repeater {
                            model: typeof weatherViewModel !== "undefined" ? weatherViewModel.favoriteCities : null
                            FavoriteItem {
                                cityName:    modelData
                                temperature: {
                                    var temps = weatherViewModel.favoriteCityTemps
                                    return temps[modelData] !== undefined ? temps[modelData] : "—"
                                }
                                itemIndex:   index
                                appRoot:     root
                                onSelectCity: {
                                    if (typeof weatherViewModel !== "undefined")
                                        requestWeather(cityName)
                                }
                                onRemoveCity: {
                                    if (typeof weatherViewModel !== "undefined")
                                        weatherViewModel.removeCityFromFavorites(cityName)
                                    if (appRoot.counterbutton > 0)
                                        appRoot.counterbutton--
                                }
                            }
                        }
                    }
                }

                TitleButton {
                    visible: counterbutton < 6
                    icon: root.addMode ? "✕" : "+"
                    tooltip: root.addMode ? "Отмена" : "Добавить город"
                    accentColor: root.addMode ? root.danger : root.success
                    onClicked: {
                        root.addMode = !root.addMode
                        if (!root.addMode) {
                            searchInput.text = ""
                        } else {
                            counterbutton++
                            searchInput.forceActiveFocus()
                        }
                    }
                }
            }
        }
    }

    // Вкладка настроек.
    // SettingsView получает root, а не weatherViewModel: единицы измерения теперь
    // живут на стороне QML (isCelsius/isMs), а apiKey/autoRefresh — заглушки,
    // т.к. в C++ ViewModel их пока нет.
    Drawer {
        id: settingsDrawer
        width: 250
        height: root.height
        edge: Qt.LeftEdge
        modal: true
        dim: true
        background: Rectangle {
            color: root.bgCard
            Rectangle {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                width: 1
                color: root.border
            }
        }
        SettingsView {
            anchors.fill: parent
            viewModel: root
            onClose: settingsDrawer.close()
        }
    }
}
