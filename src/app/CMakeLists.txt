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
)

target_link_libraries(App PRIVATE Engine)
target_link_libraries(App PRIVATE glfw webgpu glfw3webgpu imgui)

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