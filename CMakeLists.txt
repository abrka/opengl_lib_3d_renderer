cmake_minimum_required(VERSION 3.6)
project(opengl_lib_3d_renderer_proj)


add_executable(opengl_lib_3d_renderer ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_include_directories(opengl_lib_3d_renderer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(external/opengl_lib)
target_link_libraries(opengl_lib_3d_renderer PRIVATE opengl_lib)

set(OPENGL_VERSION_MAJOR "4" CACHE STRING "OpenGL Version Major" )
set(OPENGL_VERSION_MINOR "6" CACHE STRING "OpenGL Version Minor")

target_compile_definitions(opengl_lib_3d_renderer PRIVATE OPENGL_VERSION_MAJOR=${OPENGL_VERSION_MAJOR})
target_compile_definitions(opengl_lib_3d_renderer PRIVATE OPENGL_VERSION_MINOR=${OPENGL_VERSION_MINOR})
target_compile_definitions(opengl_lib_3d_renderer PRIVATE ASSET_DIR=${CMAKE_CURRENT_SOURCE_DIR}/data)
