cmake_minimum_required(VERSION 4.0)
project(QuantumEmulator
        VERSION 0.0.1
        LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/${CMAKE_SYSTEM_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/${CMAKE_SYSTEM_NAME})

#add_executable(QuantumEmulator main.cpp)

# -- WebGPU (Dawn) -- #
add_subdirectory(webgpu)
#target_copy_webgpu_binaries(QuantumEmulator)
#set(DAWN_FETCH_DEPENDENCIES ON)
#add_subdirectory("dawn" EXCLUDE_FROM_ALL)
#
#target_link_libraries(QuantumEmulator PRIVATE webgpu_dawn webgpu_glfw glfw)

add_subdirectory(src/engine)
add_subdirectory(src/app)