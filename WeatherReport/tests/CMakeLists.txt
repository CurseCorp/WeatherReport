
add_executable(tst_weatherviewmodel
    "test_weatherviewmodel/tst_weatherviewmodel.cpp"

)

find_package(Qt6 REQUIRED COMPONENTS Test Core Network Sql)

file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")
list(REMOVE_ITEM SRC_FILES "${CMAKE_SOURCE_DIR}/src/main.cpp")

target_sources(tst_weatherviewmodel PRIVATE ${SRC_FILES})

target_include_directories(tst_weatherviewmodel PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(tst_weatherviewmodel PRIVATE Qt6::Test Qt6::Core Qt6::Network Qt6::Sql)
