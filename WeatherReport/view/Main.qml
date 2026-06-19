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

    property bool showHistory: false
    property int currentDayIndex: 0
    property bool findpressed: false

    Component.onCompleted: {
        requestWeather("Москва")
    }

    // Оборачиваем вызов C++ метода, чтобы локально подсвечивать BusyIndicator
    function requestWeather(city) {
        isLoadingLocal = true
        if (typeof weatherViewModel !== "undefined")
            weatherViewModel.loadWeather(city)
            weatherViewModel.loadHistory(city)
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
        var sign = value >= 0 ? "+" : "-"
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
    function getWeatherImage(code) {
        if (!code) return "🌡"
        var map = {
            "sunny": "images/Mainsun.png",
            "partly_cloudy": "images/Mainpartlycloudy.png",
            "cloudy": "images/Mainovercast.png",
            "overcast": "images/Mainovercast.png",
            "fog": "images/Mainfog.png",
            "drizzle": "images/Maindrizzle.png",
            "thunderstorm": "images/Mainthunderstorm.png",
            "snow": "images/Mainsnow.png",
            "light_snow": "images/Mainsnow.png",
            "heavy_snow": "images/Mainsnow.png",
            "light_rain": "images/Mainrain.png",
            "heavy_rain": "images/Mainrain.png",
            "rain": "images/Mainrain.png",
            "night": "images/Mainnight.png"
        }
        return map[code] || "images/pressure"
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
            Row {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.margins: 20
                anchors.leftMargin: 30

                spacing: 10

                Text {
                    text: typeof weatherViewModel !== "undefined"
                          ? weatherViewModel.cityNameText
                          : "—"

                    font.pixelSize: 30
                    font.weight: Font.Medium
                    color: root.textPrimary

                    anchors.verticalCenter: parent.verticalCenter
                }

                TitleButton {
                    anchors.verticalCenter: parent.verticalCenter
                    icon: root.findpressed ? "◀" : "▶"
                    tooltip: root.findpressed ? "Отмена" : "Поиск города"
                    accentColor: root.findpressed ? root.danger : root.accent

                    onClicked: {
                        root.findpressed = !root.findpressed

                        if (!root.findpressed) {
                            searchforonecountry.text = ""
                        } else {
                            searchforonecountry.forceActiveFocus()
                        }
                    }
                }
                TextField {
                    id: searchforonecountry

                    anchors.verticalCenter: parent.verticalCenter

                    visible: root.findpressed
                    width: 180
                    color: root.textPrimary
                    placeholderText: "Введите город"
                    enabled: root.findpressed
                    readOnly: !root.findpressed
                    Keys.onReturnPressed: {
                        var cityText = text.trim()
                        if (cityText.length === 0) return

                        requestWeather(cityText)

                        text = ""
                        root.findpressed = false
                    }

                    Keys.onEscapePressed: {
                        text = ""
                        root.findpressed = false
                    }
                }
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
            // Позиционирование основного контейнера
            anchors {
                top: parent.top
                left: parent.left
                topMargin: 170
                leftMargin: 70
            }

            width: 470
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

            // Используем RowLayout для управления горизонтальным положением
            RowLayout {
                anchors.fill: parent
                anchors.margins: 35
                spacing: 20 // Заменили отрицательный отступ на реальный

                // Левая часть с текстом
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter
                    spacing: 4

                    Text {
                        text: typeof weatherViewModel !== "undefined" ? formatTemp(weatherViewModel.tempText) : "--°"
                        font.pixelSize: 48
                        font.weight: Font.Light
                        color: root.textPrimary
                    }

                    Text {
                        text: typeof weatherViewModel !== "undefined"
                              ? "Мин " + formatTemp(weatherViewModel.minTemp) + " / Макс " + formatTemp(weatherViewModel.maxTemp)
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

                // Правая часть с иконкой
                Image {
                    Layout.preferredWidth: 150
                    Layout.preferredHeight: 150
                    Layout.alignment: Qt.AlignVCenter

                    source: typeof weatherViewModel !== "undefined"
                            ? getWeatherImage(weatherViewModel.iconCode) : "images/sun.png"
                    fillMode: Image.PreserveAspectFit
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
            id: forecastWindow
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.leftMargin: 70
            anchors.bottomMargin: 30
            width: 660
            height: 230
            radius: 14
            color: root.bgCard
            clip: true

            Row {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 10

                TitleButton {
                    icon: "◀"
                    enabled: !showHistory && currentDayIndex > 0
                    onClicked: currentDayIndex--
                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width - 210
                    horizontalAlignment: Text.AlignHCenter

                    text: showHistory
                          ? "История прогнозов"
                          : (weatherViewModel.forecastModel.length > 0
                             ? "Прогноз погоды на 24 часа: " +
                               weatherViewModel.forecastModel[currentDayIndex].date
                             : "")

                    font.pixelSize: 16
                    font.weight: Font.Medium
                    color: root.textPrimary
                }

                TitleButton {
                    icon: "▶"
                    enabled: !showHistory &&
                             weatherViewModel.forecastModel.length > 0 &&
                             currentDayIndex < weatherViewModel.forecastModel.length - 1
                    onClicked: currentDayIndex++
                }

                // Кнопка переключения режима
                TitleButton {
                    icon: showHistory ? "☀" : "🕘"

                    onClicked: {
                        showHistory = !showHistory
                    }
                }
            }
            ScrollView {
                    id: hourlyScroll

                    visible: !showHistory

                    anchors.top: parent.top
                    anchors.topMargin: 40
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom

                    ScrollBar.vertical.policy: ScrollBar.AlwaysOff
                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                    WheelHandler {
                        onWheel: function(event) {
                            var flick = hourlyScroll.contentItem

                            flick.contentX -= event.angleDelta.y

                            if (flick.contentX < 0)
                                flick.contentX = 0

                            if (flick.contentX > flick.contentWidth - flick.width)
                                flick.contentX = flick.contentWidth - flick.width

                            event.accepted = true
                        }
                    }

                    Row {
                        spacing: 10
                        padding: 10

                        Repeater {
                            model: weatherViewModel.forecastModel[currentDayIndex].hourly

                            ForecastCard {
                                timeStr: modelData.time
                                iconImage: modelData.icon
                                tempStr: modelData.temp
                            }
                        }
                    }
                }


                // ИСТОРИЯ ПРОГНОЗОВ

            ListView {
                visible: showHistory

                anchors.top: parent.top
                anchors.topMargin: 40
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom

                clip: true
                spacing: 8

                model: weatherViewModel.historyData

                delegate: Rectangle {
                    width: ListView.view.width - 20
                    height: 60
                    radius: 8
                    color: Qt.rgba(1, 1, 1, 0.05)

                    Row {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 25

                        Text {
                            width: 70
                            text: modelData.date
                            color: root.textPrimary
                            font.bold: true
                        }

                        Text {
                            width: 90
                            text: formatTemp(modelData.tempMin) + " / " + formatTemp(modelData.tempMax)
                            color: root.textPrimary
                        }

                        Text {
                            text: modelData.description
                            color: root.textPrimary
                            elide: Text.ElideRight
                        }
                    }
                }
            }
        }

        // Поисковая строка / добавление в избранное

        Rectangle {
            id: searchAreaWrapper
            anchors { top: parent.top; left: parent.left; leftMargin: 755; topMargin: 115 }
            width: root.addMode ? 520 : 400
            height: 40
            color: "transparent"

            Behavior on width { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }

            Rectangle {
                id: searchBoxOuter
                anchors { top: parent.top; left: parent.left; right: root.addMode ? addFavBtn.left : parent.right; rightMargin: root.addMode ? 8 : 0 }
                height: 40
                radius: 14
                color: root.addMode ? "#EDEAEA" : root.bgCard
                border {
                    color: root.addMode ? (searchInput.activeFocus ? root.accent : root.border) : root.bgCard
                    width: root.addMode ? (searchInput.activeFocus ? 2 : 1) : 1
                }
                clip: true

                RowLayout {
                    anchors { fill: parent; leftMargin: 14; rightMargin: 8 }
                    spacing: 8

                    Text { text: "🔍"; font.pixelSize: 14; color: root.addMode ? root.textSecond : "#788EA8" }

                    TextField {
                        id: searchInput
                        Layout.fillWidth: true
                        placeholderText: root.addMode ? "Введите город для добавления..." : "Нажмите +"
                        font.pixelSize: 14
                        color: root.addMode ? "#1A1A2E" : root.textSecond
                        background: null
                        enabled: root.addMode
                        readOnly: !root.addMode
                        onTextChanged: {
                            if (text.length >= 2) {
                                weatherViewModel.searchCities(text)
                                suggestionsPopup.open() // Добавьте это!
                            } else {
                                suggestionsPopup.close()
                            }
                        }
                    }
                }
            }


            Popup {
                id: suggestionsPopup
                parent: searchBoxOuter
                y: searchBoxOuter.height + 5
                width: searchBoxOuter.width
                padding: 0
                closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

                background: Rectangle { radius: 10; color: "#FFFFFF"; border.color: "#E0E0E0" }

                ListView {
                    id: listView
                    model: weatherViewModel.searchResults
                    height: contentHeight

                    delegate: ItemDelegate {
                        width: suggestionsPopup.width
                        text: modelData.displayName

                        onClicked: {
                            searchInput.text = modelData.displayName
                            weatherViewModel.addCityToFavorites(modelData.name)


                            suggestionsPopup.close()

                            root.addMode = false


                            searchInput.focus = false
                        }
                    }
                }
                }

            Rectangle {
                id: addFavBtn
                anchors { top: parent.top; right: parent.right }
                width: root.addMode ? 106 : 0
                height: 40
                radius: 14
                clip: true
                color: addBtnHover.containsMouse ? "#BBE5BE" : "#E2F2E2"
                border { color: root.success; width: 1 }
                opacity: root.addMode ? 1 : 0

                Behavior on width { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }

                RowLayout {
                    anchors.centerIn: parent
                    Text { text: "✓"; color: root.success }
                    Text { text: "Добавить"; color: root.success; font.weight: Font.Medium }
                }

                MouseArea {
                    id: addBtnHover
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {

                        root.addMode = false
                        suggestionsPopup.close()
                    }
                }
            }
        }
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
