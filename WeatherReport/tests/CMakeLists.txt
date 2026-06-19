add_executable(tst_weatherviewmodel tst_weatherviewmodel.cpp)

# Находим пакеты
find_package(Qt6 REQUIRED COMPONENTS Test Core Network Sql)

# Линкуем с нужными модулями
target_link_libraries(tst_weatherviewmodel PRIVATE Qt6::Test Qt6::Core Qt6::Network Qt6::Sql)

# ДОБАВЛЯЕМ ПУТИ К ТВОИМ ФАЙЛАМ (без изменения структуры)
target_include_directories(tst_weatherviewmodel PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/viewmodel
    ${CMAKE_SOURCE_DIR}/src/model/services
    ${CMAKE_SOURCE_DIR}/src/model/entities
)

# ВКЛЮЧАЕМ СЮДА ТВОИ .CPP ФАЙЛЫ, КОТОРЫЕ НУЖНЫ ДЛЯ ТЕСТА
target_sources(tst_weatherviewmodel PRIVATE
    ${CMAKE_SOURCE_DIR}/src/viewmodel/weatherviewmodel.cpp
    ${CMAKE_SOURCE_DIR}/src/model/services/geocodingservice.cpp
    # Добавь сюда остальные .cpp файлы, которые используются в твоем коде
)

add_test(NAME tst_weatherviewmodel COMMAND tst_weatherviewmodel)
