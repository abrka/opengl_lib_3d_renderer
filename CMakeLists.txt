cmake_minimum_required(VERSION 3.6)
project(opengl_lib_3d_renderer_proj)


add_executable(opengl_lib_3d_renderer ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_compile_features(opengl_lib_3d_renderer PUBLIC cxx_std_20)
target_include_directories(opengl_lib_3d_renderer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Add custom OpenGL abstraction library
add_subdirectory(external/opengl_lib)
target_link_libraries(opengl_lib_3d_renderer PRIVATE opengl_lib)

# Add assimp 
add_subdirectory(external/assimp)
target_link_libraries(opengl_lib_3d_renderer PRIVATE assimp::assimp)

# Add entt
add_subdirectory(external/entt)
target_link_libraries(opengl_lib_3d_renderer PRIVATE EnTT::EnTT)

# Add cereal
set(JUST_INSTALL_CEREAL ON)
add_subdirectory(external/cereal)
target_link_libraries(opengl_lib_3d_renderer PRIVATE cereal::cereal)

# Add tl expected
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


set(OPENGL_VERSION_MAJOR "4" CACHE STRING "OpenGL Version Major")
set(OPENGL_VERSION_MINOR "6" CACHE STRING "OpenGL Version Minor")

target_compile_definitions(opengl_lib_3d_renderer PRIVATE OPENGL_VERSION_MAJOR=${OPENGL_VERSION_MAJOR})
target_compile_definitions(opengl_lib_3d_renderer PRIVATE OPENGL_VERSION_MINOR=${OPENGL_VERSION_MINOR})
target_compile_definitions(opengl_lib_3d_renderer PRIVATE ASSET_DIR=${CMAKE_CURRENT_SOURCE_DIR}/data)
