cmake_minimum_required(VERSION 3.14)

project(bluetoothCrashMinimalExample LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Bluetooth REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Bluetooth REQUIRED)

add_executable(bluetoothCrashMinimalExample
  main.cpp
  DeviceScanner.hpp
  ServiceScanner.hpp
  ScannedDevicesFilter.hpp
)
target_link_libraries(bluetoothCrashMinimalExample Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Bluetooth)
