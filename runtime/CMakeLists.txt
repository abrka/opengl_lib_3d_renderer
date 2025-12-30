cmake_minimum_required(VERSION 3.2...3.6)
project(abrka_game_runtime_proj)

file(GLOB_RECURSE ABRKA_GAME_RUNTIME "src/*.cpp" "src/*.h" "src/*.hpp")
add_executable(abrka_game_runtime ${ABRKA_GAME_RUNTIME})

target_link_libraries(abrka_game_runtime PRIVATE abrka::game_engine)

set(OPENGL_VERSION_MAJOR "4" CACHE STRING "OpenGL Version Major")
set(OPENGL_VERSION_MINOR "6" CACHE STRING "OpenGL Version Minor")

target_compile_definitions(abrka_game_runtime PRIVATE OPENGL_VERSION_MAJOR=${OPENGL_VERSION_MAJOR})
target_compile_definitions(abrka_game_runtime PRIVATE OPENGL_VERSION_MINOR=${OPENGL_VERSION_MINOR})