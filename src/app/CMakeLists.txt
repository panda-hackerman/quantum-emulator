add_executable(App main.cc)

target_link_libraries(App PRIVATE Engine)
#target_link_libraries(App PRIVATE webgpu)

target_copy_webgpu_binaries(App)