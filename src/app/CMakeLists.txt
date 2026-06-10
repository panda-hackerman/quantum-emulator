add_executable(App main.cc
        util/device_adapter_util.cc
        util/device_adapter_util.h
        application.cc
        application.h
        util/wgpu_string_view_util.h
        util/sleep_util.h
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
    set_target_properties(App PROPERTIES SUFFIX ".html")
    target_link_options(App PRIVATE -sASYNCIFY)
endif ()

target_copy_webgpu_binaries(App)