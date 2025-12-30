cmake_minimum_required(VERSION 3.2...3.6)
project(abrka_game_engine_proj)

file(GLOB_RECURSE ABRKA_GAME_ENGINE_SOURCES "src/*.cpp" "src/*.h" "src/*.hpp")

add_library(abrka_game_engine ${ABRKA_GAME_ENGINE_SOURCES})
add_library(abrka::game_engine ALIAS abrka_game_engine)

target_include_directories(abrka_game_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_features(abrka_game_engine PRIVATE cxx_std_20)

target_compile_definitions(abrka_game_engine PUBLIC ENGINE_ASSET_DIR=${CMAKE_CURRENT_SOURCE_DIR}/data)

set(OPENGL_LIB_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
add_subdirectory(external/opengl_lib)
target_link_libraries(abrka_game_engine PUBLIC opengl_lib::opengl_lib)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(abrka_game_engine PUBLIC assimp::assimp)

find_package(EnTT CONFIG REQUIRED)
target_link_libraries(abrka_game_engine PUBLIC EnTT::EnTT)

find_package(cereal CONFIG REQUIRED)
target_link_libraries(abrka_game_engine PUBLIC cereal::cereal)

find_package(tl-expected CONFIG REQUIRED)
target_link_libraries(abrka_game_engine PUBLIC tl::expected)

find_package(imguizmo CONFIG REQUIRED)
target_link_libraries(abrka_game_engine PUBLIC imguizmo::imguizmo)

find_package(Lua CONFIG REQUIRED)
target_include_directories(abrka_game_engine PUBLIC ${LUA_INCLUDE_DIR})
target_link_libraries(abrka_game_engine PUBLIC ${LUA_LIBRARIES})

find_package(sol2 CONFIG REQUIRED)
target_link_libraries(abrka_game_engine PUBLIC sol2::sol2)

find_package(tinyfiledialogs CONFIG REQUIRED)
target_link_libraries(abrka_game_engine PUBLIC tinyfiledialogs::tinyfiledialogs)

option(ABRKA_GAME_ENGINE_BUILD_DOCS "Build Documentation" ON)
if(${ABRKA_GAME_ENGINE_BUILD_DOCS})
    add_subdirectory(docs)
endif()

option(ABRKA_GAME_ENGINE_BUILD_EDITOR "Build Editor Project" ON)
if(${ABRKA_GAME_ENGINE_BUILD_EDITOR})
    add_subdirectory(editor)
endif()

option(ABRKA_GAME_ENGINE_BUILD_GAME "Build Game Framework Project" ON)
if(${ABRKA_GAME_ENGINE_BUILD_GAME})
    add_subdirectory(game)
endif()

