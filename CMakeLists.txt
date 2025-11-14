cmake_minimum_required(VERSION 4.0)
project(FinalProject)
cmake_policy(SET CMP0072 NEW)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenGL REQUIRED)

add_executable(FinalProject
        src/main.cpp
        src/glad/glad.c
        src/imgui/imgui.cpp
        src/imgui/imgui_demo.cpp
        src/imgui/imgui_draw.cpp
        src/imgui/imgui_impl_glfw.cpp
        src/imgui/imgui_impl_opengl3.cpp
        src/imgui/imgui_tables.cpp
        src/imgui/imgui_widgets.cpp
        "Header Files/ShaderClass.h"
        "Header Files/Shape.h"
        src/ShaderClass.cpp
        src/Shape.cpp
)

target_link_libraries(FinalProject
        glfw
        OpenGL::GL
)
