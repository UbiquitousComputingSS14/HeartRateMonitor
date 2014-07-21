cmake_minimum_required(VERSION 2.8)
project(HeartRateMonitor)

######################################################################
# If you compile on windows, specify the libraries here:

set(HRM_QT5 "E:/Programs/Qt/5.3/mingw482_32")
set(HRM_QT4 "E:/Programs/Qt/4.8.6")
set(HRM_QWT "E:/Programs/Qwt-6.1.0")
set(HRM_FFTW "E:/Data/Projects/C_C++/ext/fftw")

set(CMAKE_PREFIX_PATH ${HRM_QT5} ${HRM_QT4} ${HRM_QWT} ${HRM_FFTW})

######################################################################

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libstdc++ -static-libgcc -std=c++11 -Wall -g")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmakeModules")
cmake_policy(SET CMP0020 OLD)

# Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB_RECURSE hrm_ui_FORMS "ui/*.ui")
file(GLOB_RECURSE hrm_qrc_RESOURCES "res/*.qrc")

find_package(Qt5SerialPort)
find_package(Qt5Widgets)
if (!${Qt5_FOUND})
    find_package(Qt4 REQUIRED)
    qt4_wrap_ui(hrm_ui_SOURCES ${hrm_ui_FORMS})
    qt4_add_resources(hrm_qrc_SOURCES ${hrm_qrc_RESOURCES})
else (!${Qt5_FOUND})
    qt5_wrap_ui(hrm_ui_SOURCES ${hrm_ui_FORMS})
    qt5_add_resources(hrm_qrc_SOURCES ${hrm_qrc_RESOURCES})
endif (!${Qt5_FOUND})

find_package(Qwt REQUIRED)

# FFTW
find_package(FFTW)

# Application sources
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
include_directories(${FFTW_INCLUDE_DIR})

if (!${Qt5_FOUND})
    include(${QT_USE_FILE})
    add_definitions(${QT_DEFINITIONS})
endif (!${Qt5_FOUND})

if(WIN32)
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
    else(CMAKE_COMPILER_IS_GNUCXX)
        message(SEND_ERROR "You are using an unsupported Windows compiler! (Not GCC)")
    endif(CMAKE_COMPILER_IS_GNUCXX)
endif(WIN32)

add_executable(hrm ${hrm_SOURCES} ${hrm_moc_SOURCES} ${hrm_ui_SOURCES} ${hrm_qrc_SOURCES})

if (!${Qt5_FOUND})
    target_link_libraries(hrm ${QT_LIBRARIES} ${QWT_LIBRARY} QtSerialPort ${FFTW_LIBRARY})
else (!${Qt5_FOUND})
    target_link_libraries(hrm Qt5::Widgets Qt5::SerialPort ${QWT_LIBRARY} ${FFTW_LIBRARY})
endif (!${Qt5_FOUND})