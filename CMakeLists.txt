cmake_minimum_required(VERSION 3.14)

project(CPDSO_Barangay_Assistance_Log LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Look for Qt6
find_package(Qt6 COMPONENTS Widgets REQUIRED)

# QXlsx - relative path
add_subdirectory(libs/QXlsx/QXlsx)

# Include QXlsx headers (from the cloned repo)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/QXlsx/QXlsx/header)

# Build the app
add_executable(CPDSO_Barangay_Assistance_Log WIN32
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    cpdso_icon.qrc
)

# Link Qt & QXlsx
target_link_libraries(CPDSO_Barangay_Assistance_Log
    PRIVATE Qt6::Widgets QXlsx
)

# Windows: add icon
if(WIN32)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/resources.rc")
    target_sources(CPDSO_Barangay_Assistance_Log PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
endif()
