add_library(Engine
        engine.h
        engine.cc
        collections/bimap.h
        collections/compile_time_map.h
        collections/sparse_set.h
)

target_include_directories(Engine PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(Engine PRIVATE webgpu)