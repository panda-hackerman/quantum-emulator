add_executable(App main.cc
        util/device_adapter_util.cc
        util/device_adapter_util.h
        util/wgpu_string_view_util.h
        util/sleep_util.h
        util/settings_constants.h
        application/application.cc
        application/application.h
        editor_windows/editor_window_manager.cc
        editor_windows/editor_window_manager.h
        application/device_window.h
        application/device_window.cc
        editor_windows/custom_windows.h
        editor_windows/custom_windows.cc
)

target_link_libraries(App PRIVATE Engine)
target_link_libraries(App PRIVATE glfw webgpu glfw3webgpu imgui implot)

# Automatically copy resource files to build directory
target_compile_definitions(App PRIVATE
        RESOURCE_DIR="./resources/app"
)

if (NOT EMSCRIPTEN)
    add_custom_target(copy_resources ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/resources
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/app
            COMMENT "Copying app resources to build directory"
    )

    add_dependencies(App copy_resources)
else ()
    target_link_options(App PRIVATE
            --preload-file "${CMAKE_CURRENT_SOURCE_DIR}/resources@resources/app"
    )
endif ()

if (XCODE)
    set_target_properties(App PROPERTIES
            XCODE_GENERATE_SCHEME ON
            XCODE_SCHEME_ENABLE_GPU_FRAME_CAPTURE_MODE "Metal"
    )
endif ()

if (EMSCRIPTEN)
    target_link_options(App PRIVATE
            -sUSE_GLFW=3
            -sASYNCIFY
            -sALLOW_MEMORY_GROWTH
            -fexceptions
    )

    set_target_properties(App PROPERTIES SUFFIX ".html")
    target_compile_options(App PRIVATE -fexceptions)
endif ()

target_copy_webgpu_binaries(App)