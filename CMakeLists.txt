cmake_minimum_required(VERSION 3.6)
project(opengl_lib_3d_renderer_proj)


add_executable(opengl_lib_3d_renderer ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_compile_features(opengl_lib_3d_renderer PUBLIC cxx_std_20)
target_include_directories(opengl_lib_3d_renderer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(opengl_lib_3d_renderer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external/imreflect)

set(OPENGL_VERSION_MAJOR "4" CACHE STRING "OpenGL Version Major")
set(OPENGL_VERSION_MINOR "6" CACHE STRING "OpenGL Version Minor")

target_compile_definitions(opengl_lib_3d_renderer PRIVATE OPENGL_VERSION_MAJOR=${OPENGL_VERSION_MAJOR})
target_compile_definitions(opengl_lib_3d_renderer PRIVATE OPENGL_VERSION_MINOR=${OPENGL_VERSION_MINOR})
target_compile_definitions(opengl_lib_3d_renderer PRIVATE ASSET_DIR=${CMAKE_CURRENT_SOURCE_DIR}/data)

option(OPENGL_LIB_3D_RENDERER_BUILD_DOCS "Build Documentation" ON)
if(${OPENGL_LIB_3D_RENDERER_BUILD_DOCS})
    add_subdirectory(docs)
endif()

# Add custom OpenGL abstraction library
find_package(opengl_lib CONFIG REQUIRED)
target_link_libraries(opengl_lib_3d_renderer PRIVATE opengl_lib::opengl_lib)

# Add assimp 
find_package(assimp CONFIG REQUIRED)
target_link_libraries(opengl_lib_3d_renderer PRIVATE assimp::assimp)

# Add entt
find_package(EnTT CONFIG REQUIRED)
target_link_libraries(opengl_lib_3d_renderer PRIVATE EnTT::EnTT)

# Add cereal
find_package(cereal CONFIG REQUIRED)
target_link_libraries(opengl_lib_3d_renderer PRIVATE cereal::cereal)

find_package(imguimzo CONFIG REQUIRED)
target_link_libraries(opengl_lib_3d_renderer PRIVATE imguizmo)

# Add tl expected
find_package(tl-expected CONFIG REQUIRED)
target_link_libraries(opengl_lib_3d_renderer PRIVATE tl::expected)

# Add imguizmo
add_library(imguizmo
    external/imgui-gizmo/GraphEditor.cpp
    external/imgui-gizmo/GraphEditor.h
    external/imgui-gizmo/ImCurveEdit.cpp
    external/imgui-gizmo/ImCurveEdit.h
    external/imgui-gizmo/ImGradient.cpp
    external/imgui-gizmo/ImGradient.h
    external/imgui-gizmo/ImGuizmo.cpp
    external/imgui-gizmo/ImGuizmo.h
    external/imgui-gizmo/ImSequencer.cpp
    external/imgui-gizmo/ImSequencer.h
    external/imgui-gizmo/ImZoomSlider.h
)
target_include_directories(imguizmo PUBLIC external/imgui-gizmo)
target_link_libraries(imguizmo PRIVATE imgui)

target_link_libraries(opengl_lib_3d_renderer PRIVATE imguizmo)



