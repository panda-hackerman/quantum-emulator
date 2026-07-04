cmake_minimum_required(VERSION 4.0)
project(QuantumEmulator
        VERSION 0.0.1
        LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/${CMAKE_SYSTEM_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/${CMAKE_SYSTEM_NAME})

option(TEST_BUILD "Build tests?" OFF)

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
add_subdirectory(vendor/implot)

if (TEST_BUILD)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory(vendor/gtest EXCLUDE_FROM_ALL)
    add_subdirectory(test)

    enable_testing()
    message(STATUS ">>> Testing enabled! :3")
endif ()

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

if (NOT TEST_BUILD)
    add_subdirectory(src/app)
endif ()

# -- COMPILE OPTIONS -- #
if (NOT TEST_BUILD)
    if (MSVC)
        target_compile_options(App PRIVATE /W4 /WX)
    else ()
        target_compile_options(App PRIVATE -Wall)
    endif ()
endif ()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")