cmake_minimum_required(VERSION 3.2...3.6)
project(opengl_lib_3d_renderer_example_game_proj)

file(GLOB_RECURSE OPENGL_LIB_3D_RENDERER_EXAMPLE_GAME_SOURCES "src/*.cpp" "src/*.h" "src/*.hpp")
add_executable(opengl_lib_3d_renderer_example_game ${OPENGL_LIB_3D_RENDERER_EXAMPLE_GAME_SOURCES})

target_link_libraries(opengl_lib_3d_renderer_example_game PRIVATE abrka::game_engine)

set(OPENGL_VERSION_MAJOR "4" CACHE STRING "OpenGL Version Major")
set(OPENGL_VERSION_MINOR "6" CACHE STRING "OpenGL Version Minor")

target_compile_definitions(opengl_lib_3d_renderer_example_game PRIVATE OPENGL_VERSION_MAJOR=${OPENGL_VERSION_MAJOR})
target_compile_definitions(opengl_lib_3d_renderer_example_game PRIVATE OPENGL_VERSION_MINOR=${OPENGL_VERSION_MINOR})
target_compile_definitions(opengl_lib_3d_renderer_example_game PRIVATE ASSET_DIR=${CMAKE_CURRENT_SOURCE_DIR}/data)