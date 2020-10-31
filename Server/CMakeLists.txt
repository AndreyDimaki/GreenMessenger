cmake_minimum_required(VERSION 3.14)

project(server LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

add_subdirectory(../Common Common)

add_executable(server
  main.cpp
  MessengerServer.cpp
  MessengerServer.h
)
target_link_libraries(server
    Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)
