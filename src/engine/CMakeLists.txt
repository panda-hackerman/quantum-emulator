add_library(Engine
        collections/bimap.h
        collections/compile_time_map.h
        collections/sparse_set.h
        math/matrix.h
        quantum_circuit/circuit.h
        quantum_circuit/circuit.cc
        util/print_util.h
        simulator/simulator.h
        util/template_wizardry.h
        types.h
        math/constants.h
        simulator/simulator.cc
        math/constexpr_math.h
)

target_include_directories(Engine PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(Engine PRIVATE webgpu)