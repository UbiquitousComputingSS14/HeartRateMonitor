cmake_minimum_required(VERSION 2.8)
project(HeartRateMonitor)

find_package(Qt4 REQUIRED)
find_package(Qwt REQUIRED)

#set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")

file(GLOB_RECURSE hrm_SOURCES "src/*.cpp")

file(GLOB_RECURSE hrm_moc_HEADERS "src/*.h")
file(GLOB_RECURSE hrm_ui_HEADERS "ui/*.ui")

include_directories("include")
include_directories("src")
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${QWT_INCLUDE_DIR})

# Qt stuff
QT4_WRAP_CPP(hrm_moc_SOURCES ${hrm_moc_HEADERS})
QT4_WRAP_UI(hrm_ui_SOURCES ${hrm_ui_HEADERS})

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

add_executable(hrm ${hrm_SOURCES} ${hrm_moc_SOURCES} ${hrm_ui_SOURCES})
target_link_libraries(hrm ${QT_LIBRARIES} ${QWT_LIBRARY} QtSerialPort)
