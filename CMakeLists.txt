cmake_minimum_required(VERSION 3.16)
project(myshell)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 20)
find_package(Readline REQUIRED)
include_directories(${Readline_INCLUDE_DIR})
add_executable(myshell main.cpp tree.h syntax_analysis.h
    inter_functions.h argsmap.cpp argsmap.h syntax_tree_parser.h)
add_executable(example example.cpp)
target_link_libraries(example ${Readline_LIBRARY})
target_link_libraries(myshell ${Readline_LIBRARY})
