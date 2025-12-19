cmake_minimum_required(VERSION 3.28.3)
project(GraphicsDemo LANGUAGES C CXX)

cmake_policy(SET CMP0072 NEW)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)
include(FetchContent)

if(NOT DEFINED ENV{XDG_SESSION_TYPE})
    set(SESSION_TYPE "x11")
else()
    string(TOLOWER "$ENV{XDG_SESSION_TYPE}" SESSION_TYPE)
endif()

if(SESSION_TYPE STREQUAL "wayland")
    message(STATUS "Detected Wayland session")
    set(GLFW_BUILD_WAYLAND ON CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11 OFF CACHE BOOL "" FORCE)
else()
    message(STATUS "Detected X11 session")
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
endif()

# Get a target for OpenGL.. which should be on the system as a dependency
find_package(OpenGL REQUIRED)

# Fetch glm from its get github repo and make its target available
message(STATUS "Fetching repo... https://github.com/g-truc/glm.git")
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.2
)
FetchContent_MakeAvailable(glm)


set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# fetch glfw and make its target available
message(STATUS "Fetching repo... https://github.com/glfw/glfw.git")
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
    )
FetchContent_MakeAvailable(glfw)

# Make some a file for the source directory
file(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS src/*.cpp)
# make a file for imgui
file(GLOB_RECURSE IMGUI_SOURCE CONFIGURE_DEPENDS external/imgui/*.cpp)
# add a library target named glad from external/glad/glad.c
add_library(glad STATIC external/glad/glad.c)
# include the target in the external/glad project
target_include_directories(glad PUBLIC external/glad)
# include the include directory in glad.. ie. glad.c -> #include <glad.h>
target_include_directories(glad
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
# add the executable with imgui and the source code
add_executable(GraphicsDemo
        ${SOURCE}
        ${IMGUI_SOURCE}
)

# include imgui in the project
target_include_directories(GraphicsDemo PRIVATE
        external/imgui
)

# OpenGL
if(TARGET OpenGL::GL)
    target_link_libraries(GraphicsDemo PRIVATE OpenGL::GL)
else()
    target_include_directories(GraphicsDemo PRIVATE ${OPENGL_INCLUDE_DIR})
    target_link_libraries(GraphicsDemo PRIVATE ${OPENGL_LIBRARIES})
endif()

# GLFW
target_link_libraries(GraphicsDemo PRIVATE glfw)

# GLM
target_link_libraries(GraphicsDemo PRIVATE glm::glm)

# GLAD
target_link_libraries(GraphicsDemo PRIVATE glad)

# install the binary
install(TARGETS GraphicsDemo
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# File for desktop entry and icons
configure_file(
        desktop_entry/graphicsdemo.desktop.in
        graphicsdemo.desktop
        @ONLY
)
# install the desktop entry
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/graphicsdemo.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)

# Install the icon to a standard path so the desktop entry can find it.
install(FILES
        desktop_entry/icon.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/512x512/apps
)

# Install assets
install(DIRECTORY assets/
        DESTINATION ${CMAKE_INSTALL_DATADIR}/GraphicsDemo/assets
)