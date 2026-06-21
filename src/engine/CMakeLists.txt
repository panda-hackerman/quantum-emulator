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
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_link_libraries(Engine PRIVATE webgpu)