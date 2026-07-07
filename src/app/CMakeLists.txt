add_executable(App main.cc
        util/device_adapter_util.cc
        util/device_adapter_util.h
        util/wgpu_string_view_util.h
        util/sleep_util.h
        settings_constants.h
        application/application.cc
        application/application.h
        editor_windows/editor_window_manager.cc
        editor_windows/editor_window_manager.h
        application/device_window.h
        application/device_window.cc
        editor_windows/custom_windows.h
        editor_windows/custom_windows.cc
        resources/texture.cc
        resources/texture.h
        resources/editorconfig_handler.h
        theme.h
)

target_link_libraries(App PRIVATE Engine)
target_link_libraries(App PRIVATE glfw webgpu glfw3webgpu imgui implot stb_img)

# Automatically copy resource files to build directory
set(RESOURCE_DIR_SRC "../resources/app")
set(RESOURCE_DIR_OUT "resources/app")

target_compile_definitions(App PRIVATE
        RESOURCE_DIR="./${RESOURCE_DIR_OUT}"
)

if (NOT EMSCRIPTEN)
    add_custom_target(copy_resources ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCE_DIR_SRC}
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${RESOURCE_DIR_OUT}
            COMMENT "Copying app resources to build directory"
    )

    add_dependencies(App copy_resources)
else ()
    target_link_options(App PRIVATE
            --preload-file "${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCE_DIR_SRC}@${RESOURCE_DIR_OUT}"
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