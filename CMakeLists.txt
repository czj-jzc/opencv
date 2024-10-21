cmake_minimum_required(VERSION 3.11)

project(Demo)

find_package( OpenCV REQUIRED )

include_directories(${OpenCV_INCLUDE_DIRS} )

add_executable(Demo ./src/main.cpp)

target_link_libraries( Demo ${OpenCV_LIBS} )