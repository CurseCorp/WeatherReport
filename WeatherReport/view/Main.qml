import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Window {
    QtObject {
        id: weatherViewModel

        property string cityName:         "Старый Оскол, Россия"
        property string weekday:          "Понедельник"
        property string day:              "25"
        property string month:            "Май"
        property string temperature:      "+18°C"
        property string feelsLike:        "+15°C"
        property string description:      "Переменная облачность"
        property string iconCode:         "02d"
        property string humidity:         "65%"
        property string windSpeed:        "4.2 м/с"
        property string pressure:         "1013 hPa"
        property string precipitation:    "< 1.3 мм"
        property bool   isCelsius:         true
        property bool   isLoading:         false
        property bool   isOffline:         false
        property string lastUpdated:      "14:32"


        property ListModel forecastModel: ListModel {
            ListElement { dayName:"Пн"; date:"26 мая"; iconCode:"01d"; tempMax:"+21°"; tempMin:"+12°" }
            ListElement { dayName:"Вт"; date:"27 мая"; iconCode:"10d"; tempMax:"+17°"; tempMin:"+10°" }
            ListElement { dayName:"Ср"; date:"28 мая"; iconCode:"11d"; tempMax:"+14°"; tempMin:"+9°" }
            ListElement { dayName:"Чт"; date:"29 мая"; iconCode:"03d"; tempMax:"+16°"; tempMin:"+11°" }
            ListElement { dayName:"Пт"; date:"30 мая"; iconCode:"01d"; tempMax:"+23°"; tempMin:"+14°" }
            ListElement { dayName:"Сб"; date:"31 мая"; iconCode:"01d"; tempMax:"+23°"; tempMin:"+14°" }
            ListElement { dayName:"Вс"; date:"32 мая"; iconCode:"01d"; tempMax:"+23°"; tempMin:"+14°" }


        }
        property ListModel favoritesModel: ListModel {
            ListElement { cityName:"Москва";    country:"RU"; temperature:"+18°" }

        }
        property ListModel searchSuggestModel: ListModel {}

        function searchCity(name)   {}
        function selectCity(c, co)  {}
        function switchUnits()      { isCelsius = !isCelsius }
        function refreshWeather()   {}
        function removeFavorite(i)  { favoritesModel.remove(i) }
        function toggleFavorite()   {}
    }

    property int counterbutton: 0
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


    // Режим добавления избранного
    property bool addMode: false

    id: root
    width: 1200
    height: 720
    minimumWidth: 900
    minimumHeight: 600
    visible: true
    title: "WeatherApp"
    color: root.bgDeep
    flags: Qt.Window | Qt.FramelessWindowHint
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

//Заголовок ---------------------------------------------------
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

                Rectangle {
                    visible: typeof weatherViewModel !== "undefined"
                             ? weatherViewModel.isOffline : false
                    width: offlineTxt.width + 20
                    height: 22
                    radius: 11
                    color: Qt.rgba(0.94, 0.33, 0.31, 0.18)
                    Text {
                        id: offlineTxt
                        anchors.centerIn: parent
                        text: "● OFFLINE"
                        font.pixelSize: 10
                        font.weight: Font.Medium
                        color: root.danger
                        font.letterSpacing: 1
                    }
                }

                Item { Layout.fillWidth: true }

                Text {
                    text: typeof weatherViewModel !== "undefined"
                          ? "Обновлено: " + weatherViewModel.lastUpdated : ""
                    font.pixelSize: 11
                    color: root.textSecond
                    anchors.verticalCenter: parent.verticalCenter
                }

                TitleButton {
                    icon: "⟳"
                    tooltip: "Обновить"
                    accentColor: root.accent
                    onClicked: {
                        rotateAnim.restart()
                        if (typeof weatherViewModel !== "undefined")
                            weatherViewModel.refreshWeather()
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
//-------Левый бар с настройками------------------------------------------
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
                anchors.right: parent.right
                height: 700
                color: root.bgCard
            }
        }
        // Основа
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
                text: typeof weatherViewModel !== "undefined"
                      ? weatherViewModel.cityName : "—"
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
                text: typeof weatherViewModel !== "undefined"
                      ? weatherViewModel.weekday + ", "+ weatherViewModel.day +" "+ weatherViewModel.month +" "+ weatherViewModel.lastUpdated: "—"
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
                running: typeof weatherViewModel !== "undefined"
                         ? weatherViewModel.isLoading : false
                visible: running
                palette.dark: root.accent
            }

            Row {
                anchors.fill: parent
                anchors.margins: 35
                spacing: -40

                Column {
                    spacing: 4
                    anchors.verticalCenter: parent.verticalCenter

                    Text {
                        text: typeof weatherViewModel !== "undefined"
                              ? weatherViewModel.temperature : "--°"
                        font.pixelSize: 48
                        font.weight: Font.Light
                        color: root.textPrimary
                    }

                    Text {
                        text: typeof weatherViewModel !== "undefined"
                              ? "Ощущается как " + weatherViewModel.feelsLike : ""
                        font.pixelSize: 12
                        color: root.textSecond
                    }

                    Text {
                        text: typeof weatherViewModel !== "undefined"
                              ? weatherViewModel.description : ""
                        font.pixelSize: 13
                        color: root.textSecond
                    }
                }

                Image {
                    source: getWeatherEmoji(weatherViewModel.iconCode)

                    width: 400
                    height: 400

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
                        icon: "humidity.png"
                        label: "Влажность"
                        value: typeof weatherViewModel !== "undefined"
                               ? weatherViewModel.humidity : "--"
                        color: root.accent
                    }
                    DetailCard {
                        Layout.alignment: Qt.AlignHCenter
                        icon: "windSpeed.png"
                        label: "Ветер"
                        value: typeof weatherViewModel !== "undefined"
                               ? weatherViewModel.windSpeed : "--"
                        color: root.success
                    }
                    DetailCard {
                        Layout.alignment: Qt.AlignHCenter
                        icon: "pressure.png"
                        label: "Давление"
                        value: typeof weatherViewModel !== "undefined"
                               ? weatherViewModel.pressure : "--"
                        color: root.accentWarm
                    }
                    DetailCard {
                        Layout.alignment: Qt.AlignHCenter
                        icon: "precipitation.png"
                        label: "Осадки"
                        value: typeof weatherViewModel !== "undefined"
                               ? weatherViewModel.precipitation : "--"
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

                Layout.preferredWidth: 660
                height: 230
                radius: 14
                color: root.bgCard
                clip: true
                Text {
                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Прогноз на 7 дней"
                    font.pixelSize: 16
                    font.weight: Font.Medium
                    color: root.textPrimary
                    Layout.fillWidth: true
                    Layout.preferredHeight: 30
                }
                ScrollView {
                    anchors.top:  parent.top
                    anchors.margins: 33
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    ScrollBar.horizontal.policy: ScrollBar.AsNeeded
                    ScrollBar.vertical.policy: ScrollBar.AlwaysOff
                    clip: true

                    Row {
                        spacing: 14
                        padding: 4

                        Repeater {
                            model: typeof weatherViewModel !== "undefined"
                                   ? weatherViewModel.forecastModel : null

                            ForecastCard {
                                dayName:  model.dayName
                                dateStr:  model.date
                                iconCode: model.iconCode
                                tempMax:  model.tempMax
                                tempMin:  model.tempMin

                                opacity: 0
                                Component.onCompleted: {
                                    appearTimer.interval = index * 80
                                    appearTimer.start()
                                }
                                Timer {
                                    id: appearTimer
                                    onTriggered: {
                                        fadeIn.start()
                                        slideIn.start()
                                    }
                                }
                                NumberAnimation on opacity {
                                    id: fadeIn
                                    running: false
                                    from: 0
                                    to: 1
                                    duration: 350
                                    easing.type: Easing.OutCubic
                                }
                                NumberAnimation on y {
                                    id: slideIn
                                    running: false
                                    from: 20
                                    to: 0
                                    duration: 350
                                    easing.type: Easing.OutCubic
                                }
                            }
                        }
                    }
                }
            }
            //поиск

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

                // Поисковая строка
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

                    Behavior on color {
                        ColorAnimation { duration: 200 }
                    }
                    Behavior on border.color {
                        ColorAnimation { duration: 200 }
                    }

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
                                color: root.addMode ? root.textPrimary : root.textSecond
                                background: Item {}
                                leftPadding: 0
                                placeholderTextColor: root.addMode ? root.textSecond : Qt.rgba(0.47, 0.56, 0.66, 0.45)
                                enabled: root.addMode
                                readOnly: !root.addMode

                                Behavior on color { ColorAnimation { duration: 200 } }

                                onTextChanged: {
                                    if (!root.addMode) return
                                    if (text.length >= 2 && typeof weatherViewModel !== "undefined")
                                        weatherViewModel.searchCity(text)
                                    suggestPopup.visible = text.length >= 2
                                }
                                Keys.onReturnPressed: {
                                    if (!root.addMode) return
                                    suggestPopup.visible = false
                                    if (typeof weatherViewModel !== "undefined")
                                        weatherViewModel.searchCity(text)
                                }
                                Keys.onEscapePressed: {
                                    suggestPopup.visible = false
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
                                    onClicked: {
                                        searchInput.text = ""
                                        suggestPopup.visible = false
                                    }
                                }
                            }
                        }
                    }
                }

                // Кнопка "Добавить" — появляется в addMode
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
                            if (typeof weatherViewModel !== "undefined") {
                                var alreadyExists = false
                                for (var i = 0; i < weatherViewModel.favoritesModel.count; i++) {
                                    if (weatherViewModel.favoritesModel.get(i).cityName === cityText) {
                                        alreadyExists = true
                                        break
                                    }
                                }
                                if (!alreadyExists) {
                                    weatherViewModel.favoritesModel.append({
                                        cityName: cityText,
                                        country: "—",
                                        temperature: "—"
                                    })
                                }
                            }
                            searchInput.text = ""
                            suggestPopup.visible = false
                            root.addMode = false
                        }
                    }
                }

                // Подсказки поиска
                Rectangle {
                    id: suggestPopup
                    visible: false
                    anchors.top: searchBoxOuter.bottom
                    anchors.topMargin: 6
                    anchors.left: searchBoxOuter.left
                    anchors.right: searchBoxOuter.right
                    height: Math.min(suggestList.count * 44, 200)
                    radius: 10
                    color: root.bgCard
                    border.color: root.accent
                    border.width: 1
                    clip: true
                    z: 100

                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: -1
                        radius: parent.radius + 2
                        color: Qt.rgba(0, 0, 0, 0.35)
                        z: -1
                        transform: Translate { y: 4 }
                    }

                    ListView {
                        id: suggestList
                        anchors.fill: parent
                        model: typeof weatherViewModel !== "undefined"
                               ? weatherViewModel.searchSuggestModel : null
                        clip: true
                        delegate: Rectangle {
                            width: suggestList.width
                            height: 44
                            color: sih.containsMouse ? root.bgCardHover : "transparent"
                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 14
                                anchors.rightMargin: 14
                                Text {
                                    text: model.cityName
                                    font.pixelSize: 13
                                    color: root.textPrimary
                                    Layout.fillWidth: true
                                }
                                Text {
                                    text: model.country
                                    font.pixelSize: 12
                                    color: root.textSecond
                                }
                            }
                            Rectangle {
                                anchors.bottom: parent.bottom
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.leftMargin: 14
                                anchors.rightMargin: 14
                                height: 1
                                color: root.border
                                visible: index < suggestList.count - 1
                            }
                            MouseArea {
                                id: sih
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    searchInput.text = model.cityName
                                    suggestPopup.visible = false
                                    if (typeof weatherViewModel !== "undefined")
                                        weatherViewModel.selectCity(model.cityName, model.country)
                                }
                            }
                        }
                    }
                }
            }

            // Текущая погода
            Rectangle {
                Layout.fillWidth: true
                height: 240
                radius: 14
                color: root.bgCard
                clip: true

                Rectangle {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: parent.height * 0.6
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: Qt.rgba(0.31, 0.76, 0.97, 0.07) }
                        GradientStop { position: 1.0; color: "transparent" }
                    }
                }

                BusyIndicator {
                    anchors.centerIn: parent
                    running: typeof weatherViewModel !== "undefined"
                             ? weatherViewModel.isLoading : false
                    visible: running
                    palette.dark: root.accent
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 4
                    visible: typeof weatherViewModel !== "undefined"
                             ? !weatherViewModel.isLoading : true

                    Text {
                        text: typeof weatherViewModel !== "undefined"
                              ? weatherViewModel.cityName : "—"
                        font.pixelSize: 18
                        font.weight: Font.Medium
                        color: root.textPrimary
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }
                    Text {
                        text: typeof weatherViewModel !== "undefined"
                              ? weatherViewModel.description : ""
                        font.pixelSize: 13
                        color: root.textSecond
                    }

                    Item { height: 4 }

                    RowLayout {
                        spacing: 12
                        Text {
                            text: typeof weatherViewModel !== "undefined"
                                  ? getWeatherEmoji(weatherViewModel.iconCode) : "🌡"
                            font.pixelSize: 56
                        }
                        ColumnLayout {
                            spacing: 0
                            Text {
                                text: typeof weatherViewModel !== "undefined"
                                      ? weatherViewModel.temperature : "--°"
                                font.pixelSize: 48
                                font.weight: Font.Light
                                color: root.textPrimary
                            }
                            Text {
                                text: typeof weatherViewModel !== "undefined"
                                      ? "Ощущается как " + weatherViewModel.feelsLike : ""
                                font.pixelSize: 12
                                color: root.textSecond
                            }
                        }
                    }

                    Item { Layout.fillHeight: true }

                    // Переключатель C / F
                    Rectangle {
                        width: 80
                        height: 28
                        radius: 14
                        color: root.border
                        Layout.alignment: Qt.AlignRight

                        Rectangle {
                            width: 40
                            height: 28
                            radius: 14
                            color: root.accent
                            x: typeof weatherViewModel !== "undefined"
                               ? (weatherViewModel.isCelsius ? 0 : 40) : 0
                            Behavior on x {
                                SmoothedAnimation { duration: 200 }
                            }
                        }
                        RowLayout {
                            anchors.fill: parent
                            spacing: 0
                            Text {
                                Layout.fillWidth: true
                                text: "°C"
                                font.pixelSize: 12
                                font.weight: Font.Medium
                                horizontalAlignment: Text.AlignHCenter
                                color: typeof weatherViewModel !== "undefined"
                                       ? (weatherViewModel.isCelsius ? root.bgDeep : root.textSecond)
                                       : root.bgDeep
                                Behavior on color {
                                    ColorAnimation { duration: 200 }
                                }
                            }
                            Text {
                                Layout.fillWidth: true
                                text: "°F"
                                font.pixelSize: 12
                                font.weight: Font.Medium
                                horizontalAlignment: Text.AlignHCenter
                                color: typeof weatherViewModel !== "undefined"
                                       ? (!weatherViewModel.isCelsius ? root.bgDeep : root.textSecond)
                                       : root.textSecond
                                Behavior on color {
                                    ColorAnimation { duration: 200 }
                                }
                            }
                        }
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                if (typeof weatherViewModel !== "undefined")
                                    weatherViewModel.switchUnits()
                            }
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
                                      ? weatherViewModel.favoritesModel.count : "0"
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
                                model: typeof weatherViewModel !== "undefined"
                                       ? weatherViewModel.favoritesModel : null
                                FavoriteItem {
                                    cityName:    model.cityName
                                    country:     model.country
                                    temperature: model.temperature
                                    itemIndex:   index
                                    appRoot: root
                                    onSelectCity: {
                                        if (typeof weatherViewModel !== "undefined") {
                                            weatherViewModel.cityName = cityName
                                            weatherViewModel.selectCity(cityName, country)
                                        }
                                    }
                                    onRemoveCity: {
                                        if (typeof weatherViewModel !== "undefined")
                                            weatherViewModel.removeFavorite(itemIndex)
                                        if (appRoot.counterbutton > 0){
                                            appRoot.counterbutton --
                                        }
                                    }
                                }
                            }
                        }
                    }
                TitleButton {
                    visible: counterbutton<6


                    icon: root.addMode ? "✕" : "+"
                    tooltip: root.addMode ? "Отмена" : "Добавить город"
                    accentColor: root.addMode ? root.danger : root.success
                    onClicked: {

                        root.addMode = !root.addMode
                        if (!root.addMode) {
                            searchInput.text = ""
                            suggestPopup.visible = false
                        } else {
                            counterbutton ++
                            searchInput.forceActiveFocus()
                        }
                    }
                }


                }


            }
        }
    Drawer {
        id: settingsDrawer
        width: 320
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
            onClose: settingsDrawer.close()
        }
    }

    function getWeatherEmoji(code) {
        if (!code) return "🌡"
        var c = code.substring(0, 2)
        var map = {
            "01": "sun.png",
            "02": "rain.png",
            "03": "☁️",
            "04": "☁️",
            "09": "🌧",
            "10": "🌦",
            "11": "⛈",
            "13": "❄️",
            "50": "🌫"
        }
        return map[c] || "🌡"
    }
}


