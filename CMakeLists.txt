cmake_minimum_required(VERSION 3.21)
project(DreamDeveloperDesk VERSION 0.7.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

foreach(REQUIRED_PATH
    "external/glfw/CMakeLists.txt"
    "external/glad/src/glad.c"
    "external/glad/include/glad/glad.h"
    "external/glm/glm/glm.hpp"
    "external/stb/stb_image.h"
    "external/imgui/imgui.cpp")
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${REQUIRED_PATH}")
        message(FATAL_ERROR
            "Missing vendored dependency: ${REQUIRED_PATH}\n"
            "Ensure the complete external/ directory is present in your checkout.")
    endif()
endforeach()

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
endif()
add_subdirectory(external/glfw)
find_package(OpenGL REQUIRED)

add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

add_library(imgui STATIC
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC external/imgui external/imgui/backends)
target_link_libraries(imgui PUBLIC glfw)

add_executable(DreamDeveloperDesk
    src/main.cpp
    src/app/Application.cpp
    src/graphics/Shader.cpp
    src/graphics/Mesh.cpp
    src/graphics/Primitives.cpp
    src/graphics/Camera.cpp
    src/graphics/Texture.cpp
    src/graphics/Framebuffer.cpp
    src/scene/SceneNode.cpp
    src/scene/Scene.cpp
    src/scene/WowSequence.cpp
    src/scene/SceneFactory.cpp
    src/scene/RoomFactory.cpp
    src/scene/DeskFactory.cpp
    src/scene/Lighting.cpp
    src/ui/UiPanel.cpp
)
target_include_directories(DreamDeveloperDesk PRIVATE
    src external/glm external/stb
)
target_compile_definitions(DreamDeveloperDesk PRIVATE GLFW_INCLUDE_NONE)
target_link_libraries(DreamDeveloperDesk PRIVATE glfw glad imgui OpenGL::GL)

if(MSVC)
    target_compile_options(DreamDeveloperDesk PRIVATE /W4 /permissive-)
else()
    target_compile_options(DreamDeveloperDesk PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_custom_command(TARGET DreamDeveloperDesk POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:DreamDeveloperDesk>/assets"
    VERBATIM
)
set_target_properties(DreamDeveloperDesk PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:DreamDeveloperDesk>"
)

# Pure CPU test of the finale transitions: no OpenGL calls.
include(CTest)
if(BUILD_TESTING)
    add_executable(wow_sequence_tests
        tests/WowSequenceTests.cpp
        src/scene/WowSequence.cpp
    )
    target_include_directories(wow_sequence_tests PRIVATE src)
    add_test(NAME WowSequenceStateMachine COMMAND wow_sequence_tests)
endif()
