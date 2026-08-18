cmake_minimum_required(VERSION 3.16)
project(orderBook)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(orderBook src/main.cpp src/OrderBook.cpp src/CsvParser.cpp src/benchmark.cpp)


