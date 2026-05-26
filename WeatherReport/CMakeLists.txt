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
    src/model/entities/City.h
    src/model/services/CityAPI.cpp
    src/model/entities/ForecastData.h
    src/model/entities/WeatherData.h
    src/model/entities/WeatherService.h
    src/model/services/cityapi.cpp
    src/model/services/cityapi.h
    src/model/services/WeatherAPI.cpp
    src/model/services/weatherapi.h
)


qt_add_qml_module(appWeatherReport
    URI WeatherReport
    VERSION 1.0
    RESOURCE_PREFIX "/"
    QML_FILES
        view/Main.qml

        view/TitleButton.qml
        view/ForecastCard.qml
        view/FavoriteItem.qml
        view/DetailCard.qml
        view/TabButton.qml
        view/SettingsView.qml
    RESOURCES
        images/rain.png
        images/sun.png


        images/humidity.png
        images/precipitation.png
        images/pressure.png
        images/windSpeed.png

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