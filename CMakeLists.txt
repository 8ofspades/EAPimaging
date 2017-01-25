cmake_minimum_required (VERSION 2.8)
project (EAPprocessor)

option (WITH_GTK "Enable GTK Version" On)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory (src)

find_package(OpenCV REQUIRED)
