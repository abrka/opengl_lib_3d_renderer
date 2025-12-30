cmake_minimum_required(VERSION 3.2...3.6)
project(imreflect_project)

add_library(ImReflect INTERFACE "src/ImReflect.hpp")
add_library(ImReflect::ImReflect ALIAS ImReflect)

target_include_directories(ImReflect INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)
