cmake_minimum_required(VERSION 3.22)
project(untitled)

set(CMAKE_CXX_STANDARD 14)
find_library(libzip zip)
add_executable(untitled main.cpp src/ZipUtilities.cpp include/ZipUtilities.h src/stockData.cpp include/stockData.h include/main.h src/Mission.cpp include/Mission.h)
target_link_libraries(${PROJECT_NAME} "${libzip}")