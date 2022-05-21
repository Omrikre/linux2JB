cmake_minimum_required(VERSION 3.0)
project(linux2 VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
find_library(libzip zip)
find_library(json-c json-c)

set(SOURCE_FILES main.cpp)

add_executable(${PROJECT_NAME} main.cpp src/ZipUtilities.cpp include/ZipUtilities.h fetchStockData.sh
        src/stockData.cpp include/stockData.h include/main.h src/Mission.cpp
        include/Mission.h ./fetchStockData.sh)
target_link_libraries(${PROJECT_NAME} PUBLIC "${libzip}")
target_link_libraries(${PROJECT_NAME} PUBLIC "${json-c}")
target_include_directories(${PROJECT_NAME} PUBLIC)
