cmake_minimum_required(VERSION 2.8)
project(HeartRateMonitor)

find_package(Qt4 REQUIRED)
find_package(Qwt REQUIRED)

#set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")

file(GLOB_RECURSE hrm_SOURCES "src/*.cpp")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB_RECURSE hrm_ui_FORMS "ui/*.ui")
file(GLOB_RECURSE hrm_qrc_RESOURCES "res/*.qrc")

include_directories("include")
include_directories("src")
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${QWT_INCLUDE_DIR})

# Qt stuff
qt4_wrap_cpp(hrm_moc_SOURCES ${hrm_moc_HEADERS})
qt4_wrap_ui(hrm_ui_SOURCES ${hrm_ui_FORMS})
qt4_add_resources(hrm_qrc_SOURCES ${hrm_qrc_RESOURCES})

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

add_executable(hrm ${hrm_SOURCES} ${hrm_moc_SOURCES} ${hrm_ui_SOURCES} ${hrm_qrc_SOURCES})
target_link_libraries(hrm ${QT_LIBRARIES} ${QWT_LIBRARY} QtSerialPort)
