cmake_minimum_required(VERSION 3.14)
project(CPDSO-Barangay-Assistance-Log LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(Qt6 COMPONENTS Widgets REQUIRED)

list(APPEND CMAKE_PREFIX_PATH "D:/1 Github Projects/joe-ed/CPDSO-Barangay-Assistance-Log/libs/QXlsx/QXlsx/build")
list (APPEND CMAKE_PREFIX_PATH "C:/Qt/6.9.1/mingw_64/lib)")
include_directories("D:/1 Github Projects/joe-ed/CPDSO-Barangay-Assistance-Log/libs/QXlsx/QXlsx/header")  # adjust to point to your QXlsx source

add_subdirectory(libs/QXlsx/QXlsx/)


add_executable(CPDSO-Barangay-Assistance-Log WIN32
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    cpdso_icon.qrc
)

target_link_libraries(CPDSO-Barangay-Assistance-Log PRIVATE Qt6::Widgets QXlsx)

if(WIN32)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/resources.rc")
    target_sources(CPDSO-Barangay-Assistance-Log PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
endif()

