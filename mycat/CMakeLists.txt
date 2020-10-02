cmake_minimum_required(VERSION 3.5)

project(mycat LANGUAGES CXX)
find_package(Boost COMPONENTS program_options REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(mycat main.cpp)

target_link_libraries(mycat Boost::program_options)
