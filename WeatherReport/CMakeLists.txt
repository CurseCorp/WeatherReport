cmake_minimum_required(VERSION 3.16)

project(WeatherReport VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# ДОБАВЛЕНЫ Network и Sql
find_package(Qt6 REQUIRED COMPONENTS Core Quick Network Sql)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appWeatherReport
    src/main.cpp
    src/viewmodel/WeatherViewModel.h
    src/viewmodel/WeatherViewModel.cpp
    src/viewmodel/CacheManager.h
    src/model/entities/City.h

    src/model/entities/ForecastData.h
    src/model/entities/WeatherData.h
    src/model/entities/WeatherService.h
    src/model/services/WeatherAPI.cpp
    src/model/services/weatherapi.h
    src/model/services/IWeatherService.h
    src/model/services/openweathermapservice.cpp
    src/model/services/openweathermapservice.h
    src/model/services/weatherapiservice.cpp
    src/model/services/weatherapiservice.h
    src/model/services/weathericonmapper.cpp
    src/model/services/weathericonmapper.h
    src/model/services/weatherservicefactory.cpp
    src/model/services/weatherservicefactory.h
    src/model/services/weatherservicemanager.cpp
    src/model/services/weatherservicemanager.h
    src/model/services/geocodingservice.cpp
    src/model/services/geocodingservice.h
)


qt_add_qml_module(appWeatherReport
    URI WeatherReport
    VERSION 1.0
    QML_FILES
        view/Main.qml

        view/TitleButton.qml
        view/ForecastCard.qml
        view/FavoriteItem.qml
        view/DetailCard.qml
        view/SettingsView.qml
    RESOURCES
        view/images/night.png
        view/images/rain.png
        view/images/sun.png
        view/images/fog.png
        view/images/snow.png
        view/images/partlycloudy.png
        view/images/thunderstorm.png
        view/images/drizzle.png
        view/images/overcast.png
        view/images/humidity.png
        view/images/precipitation.png
        view/images/pressure.png
        view/images/windSpeed.png
        view/images/Mainnight.png
        view/images/Mainrain.png
        view/images/Mainsun.png
        view/images/Mainfog.png
        view/images/Mainsnow.png
        view/images/Mainpartlycloudy.png
        view/images/Mainthunderstorm.png
        view/images/Maindrizzle.png
        view/images/Mainovercast.png
        SOURCES src/viewmodel/CacheManager.h

)

set_target_properties(appWeatherReport PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appWeatherReport
    PRIVATE
        Qt6::Core
        Qt6::Quick
        Qt6::Network
        Qt6::Sql
)

include(GNUInstallDirs)
install(TARGETS appWeatherReport
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
