cmake_minimum_required(VERSION 3.5)

project(Client LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

add_library(Common STATIC
    user.cpp
    user.h
    message.cpp
    message.h
)

target_link_libraries(Common
    Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)



