cmake_minimum_required(VERSION 3.2...3.6)
project(opengl_lib_3d_renderer_proj)

file(GLOB_RECURSE OPENGL_LIB_3D_RENDERER_SOURCES "src/*.cpp" "src/*.h" "src/*.hpp")

add_library(opengl_lib_3d_renderer INTERFACE ${OPENGL_LIB_3D_RENDERER_SOURCES})
add_library(abrka::game_engine ALIAS opengl_lib_3d_renderer)

target_include_directories(opengl_lib_3d_renderer INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_features(opengl_lib_3d_renderer INTERFACE cxx_std_20)

target_compile_definitions(opengl_lib_3d_renderer INTERFACE ENGINE_ASSET_DIR=${CMAKE_CURRENT_SOURCE_DIR}/data)

find_package(opengl_lib CONFIG REQUIRED)
target_link_libraries(opengl_lib_3d_renderer INTERFACE opengl_lib::opengl_lib)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(opengl_lib_3d_renderer INTERFACE assimp::assimp)

find_package(EnTT CONFIG REQUIRED)
target_link_libraries(opengl_lib_3d_renderer INTERFACE EnTT::EnTT)

find_package(cereal CONFIG REQUIRED)
target_link_libraries(opengl_lib_3d_renderer INTERFACE cereal::cereal)

find_package(tl-expected CONFIG REQUIRED)
target_link_libraries(opengl_lib_3d_renderer INTERFACE tl::expected)

find_package(imguizmo CONFIG REQUIRED)
target_link_libraries(opengl_lib_3d_renderer INTERFACE imguizmo::imguizmo)

option(OPENGL_LIB_3D_RENDERER_BUILD_DOCS "Build Documentation" ON)
if(${OPENGL_LIB_3D_RENDERER_BUILD_DOCS})
    add_subdirectory(docs)
endif()

option(OPENGL_LIB_3D_RENDERER_BUILD_EXAMPLE "Build Example Project" ON)
if(${OPENGL_LIB_3D_RENDERER_BUILD_EXAMPLE})
    add_subdirectory(example)
endif()

