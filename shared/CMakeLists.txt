cmake_minimum_required(VERSION 3.0.2)

add_library(shared INTERFACE)

target_include_directories(shared INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})