cmake_minimum_required(VERSION 4.0)
project(QuantumEmulator
        VERSION 0.0.1
        LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/${CMAKE_SYSTEM_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/${CMAKE_SYSTEM_NAME})

# -- LIBRARIES -- #
if (NOT EMSCRIPTEN)
    add_subdirectory(vendor/glfw)
else()
    add_library(glfw INTERFACE)
    target_link_options(glfw INTERFACE -sUSE_GLFW=3)
endif ()

add_subdirectory(vendor/webgpu)
add_subdirectory(vendor/glfw3webgpu) # https://github.com/glfw/glfw/pull/2333

# -- ADD SUBDIRECTORIES -- #
add_subdirectory(src/engine)
add_subdirectory(src/app)

# -- COMPILE OPTIONS -- #
if (MSVC)
    target_compile_options(App PRIVATE /W4 /WX)
else()
    target_compile_options(App PRIVATE -Wall)
endif ()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")