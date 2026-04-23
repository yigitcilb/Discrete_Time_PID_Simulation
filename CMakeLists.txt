cmake_minimum_required(VERSION 3.10)
project(ControlSystemGUI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON) 


find_package(Qt5 COMPONENTS Widgets Charts REQUIRED)

add_executable(ControlSystemGUI
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/util.cpp
    src/util.h
)

target_link_libraries(ControlSystemGUI Qt5::Widgets Qt5::Charts)