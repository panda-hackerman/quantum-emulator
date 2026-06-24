add_library(Engine
        engine.h
        engine.cc
        collections/bimap.h
        collections/compile_time_map.h
        collections/sparse_set.h
        math/matrix.h
        quantum_circuit/circuit.cc
        quantum_circuit/circuit.h
        quantum_circuit/gates.h
        util/print_util.h
)

target_include_directories(Engine PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(Engine PRIVATE webgpu)