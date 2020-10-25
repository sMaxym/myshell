cmake_minimum_required(VERSION 3.16)
project(myshell)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 20)
find_package(Readline REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)


include_directories(${Readline_INCLUDE_DIR})

set(SRC argsmap.cpp syntax_analysis.cpp wildcard.cpp execution.cpp)
set(INC tree.h syntax_analysis.h inter_functions.h argsmap.h syntax_tree_parser.h wildcards.h
    pipes.h execution.h)
add_executable(myshell ${SRC} ${INC})
include_directories(./mycat)
add_executable(mycat ./mycat/main.cpp)
target_link_libraries(myshell ${Readline_LIBRARY})
target_link_libraries(mycat Boost::program_options)
