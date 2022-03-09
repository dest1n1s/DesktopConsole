cmake_minimum_required(VERSION 3.21)
project(DesktopConsole)

set(CMAKE_CXX_STANDARD 23)

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

add_executable(DesktopConsole main.cpp)
