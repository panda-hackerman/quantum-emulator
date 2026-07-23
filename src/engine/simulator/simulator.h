/**
 * @file simulator.h
 *
 * @brief Different functions to simulate running a quantum circuit.
 * @see Circuit
 *
 * @author Eli Michaud
 * @since 6/26/2026
 */

#ifndef SIMULATOR_H
#define SIMULATOR_H
#include "quantum_circuit/circuit.h"
#include "state_vector.h"

/**
 * Simulate the effect of a circuit layer with a single gate and any number of control and/or
 * anti-control bits on the state vector.
 *
 * @details For a matrix U and state vector |a>, this algorithm returns
 * `(I⊗ ... ⊗I⊗U⊗I⊗ ... ⊗I) * |a>`, where I is the (2x2) identity matrix.
 *
 * @remark Implementation based on McGuffin et al. \cite mcguffin_2025.
 *
 * @param state_vector The input state vector |a>. Will be modified in-place.
 * @param matrix The 2x2 unitary matrix representing the gate.
 * @param qubit_index Which qubit this gate is applied to.
 * @param parts A list of parts (indicates where the control bits are). Size must be equal to the
 * number of qubits.
 */
void ApplyMatrixQubitWise(StateVector &state_vector, const Circuit::Matrix_T *matrix,
                          Circuit::GridSize_T qubit_index, std::span<const Circuit::Part> parts);

/**
 * Simulate the effect of a swap gate on a state vector.
 *
 * @param state_vector The input state vector. Will be modified in-place.
 * @param qubit_a The first qubit to swap
 * @param qubit_b The second qubit to swap
 * @param parts The list of parts (indicates where the control bits are).
 */
void ApplySwap(StateVector &state_vector, Circuit::GridSize_T qubit_a, Circuit::GridSize_T qubit_b,
               std::span<const Circuit::Part> parts);

/**
 * Simulate an entire circuit using the Qubit-wise Multiply Method.
 * @param circuit The circuit to simulate
 * @param state_vector The input (the starting state vector). Modified in-place.
 */
void ApplyCircuitQubitWise(const Circuit &circuit, StateVector &state_vector);

/**
 * Simulate an entire layer using the Qubit-wise Multiply Method
 * @param state_vector The input state vector
 * @param circuit The circuit we're simulating
 * @param layer Which layer?
 */
void ApplyLayerQubitWise(StateVector &state_vector, const Circuit &circuit,
                         Circuit::GridSize_T layer);

// TODO: Control Bits
/**
 * Explicitly compute the matrix for a layer.
 * @warning Doesn't work with control/ anti-control qubits, or swap gates!
 * @param parts A list of parts in the layer
 * @param matrices A list of matrices in the later
 * @return A 2^n by 2^n matrix
 */
[[nodiscard]] Matrix2D<Complex> ComputeLayerExplicit(
    const std::vector<Circuit::Part> &parts,
    const std::vector<const Circuit::Matrix_T *> &matrices);

#endif // SIMULATOR_H
