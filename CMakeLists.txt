cmake_minimum_required(VERSION 3.16)
project(myshell)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 17)
find_package(Readline REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)


include_directories(${Readline_INCLUDE_DIR})

FILE(GLOB INC include/*.h)
FILE(GLOB SRC src/*.cpp)
add_executable(myshell main.cpp ${SRC} ${INC})
include_directories(./mycat)
add_executable(mycat ./mycat/main.cpp)
target_link_libraries(myshell ${Readline_LIBRARY})
target_link_libraries(mycat Boost::program_options)
