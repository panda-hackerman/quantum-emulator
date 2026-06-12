cmake_minimum_required(VERSION 4.0)
project(QuantumEmulator
        VERSION 0.0.1
        LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/${CMAKE_SYSTEM_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/${CMAKE_SYSTEM_NAME})

# -- LIBRARIES -- #
if (NOT EMSCRIPTEN)
    add_subdirectory(vendor/glfw)
else ()
    add_library(glfw INTERFACE)
    target_link_options(glfw INTERFACE -sUSE_GLFW=3)
endif ()

add_subdirectory(vendor/webgpu)
add_subdirectory(vendor/glfw3webgpu)
add_subdirectory(vendor/imgui)

# -- COMPILE DEFS -- #
if ((WEBGPU_BACKEND STREQUAL "DAWN") OR (WEBGPU_BACKEND STREQUAL "EMSCRIPTEN"))
    target_compile_definitions(imgui PUBLIC IMGUI_IMPL_WEBGPU_BACKEND_DAWN)
elseif (WEBGPU_BACKEND STREQUAL "WGPU")
    target_compile_definitions(imgui PUBLIC IMGUI_IMPL_WEBGPU_BACKEND_WGPU)
else()
    message("WARNING: WebGPU backend not set!!")
endif ()

if (GLFW_BUILD_WIN32)
    target_compile_definitions(glfw3webgpu PRIVATE _GLFW_WIN32)
endif()
if (GLFW_BUILD_X11)
    target_compile_definitions(glfw3webgpu PRIVATE _GLFW_X11)
endif()
if (GLFW_BUILD_WAYLAND)
    target_compile_definitions(glfw3webgpu PRIVATE _GLFW_WAYLAND)
endif()

# -- ADD SUBDIRECTORIES -- #
add_subdirectory(src/engine)
add_subdirectory(src/app)

# -- COMPILE OPTIONS -- #
if (MSVC)
    target_compile_options(App PRIVATE /W4 /WX)
else ()
    target_compile_options(App PRIVATE -Wall)
endif ()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")