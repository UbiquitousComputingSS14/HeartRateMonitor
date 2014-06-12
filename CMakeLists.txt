cmake_minimum_required(VERSION 2.8)
project(HeartRateMonitor)

# Qt
find_package(Qt4 REQUIRED)
find_package(Qwt REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB_RECURSE hrm_ui_FORMS "ui/*.ui")
file(GLOB_RECURSE hrm_qrc_RESOURCES "res/*.qrc")

qt4_wrap_ui(hrm_ui_SOURCES ${hrm_ui_FORMS})
qt4_add_resources(hrm_qrc_SOURCES ${hrm_qrc_RESOURCES})

#set(CMAKE_CXX_FLAGS "-std=c++11 -Wall")

file(GLOB_RECURSE hrm_SOURCES "src/*.cpp")
file(GLOB_RECURSE hrm_HEADERS "src/*.h")

set(hrm_INCLUDE_DIRS "")
foreach (_headerFile ${hrm_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list(APPEND hrm_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES hrm_INCLUDE_DIRS)

include_directories("include")
include_directories(${hrm_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${QWT_INCLUDE_DIR})

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

add_executable(hrm ${hrm_SOURCES} ${hrm_moc_SOURCES} ${hrm_ui_SOURCES} ${hrm_qrc_SOURCES})
target_link_libraries(hrm ${QT_LIBRARIES} ${QWT_LIBRARY} QtSerialPort)
