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

/**
 * Simulate the effect of a circuit layer with a single gate and any number of control and/or
 * anti-control bits on the state vector.
 *
 * @details For a matrix U and state vector |a>, this algorithm returns
 * `(I⊗ ... ⊗I⊗U⊗I⊗ ... ⊗I) * |a>`, where I is the (2x2) identity matrix.
 *
 * @remark Implementation based on McGuffin et al. \cite mcguffin_2025.
 *
 * @param in The input state vector |a>. Size must be 2^n.
 * @param num_qubits The number of qubits (n).
 * @param matrix The 2x2 unitary matrix representing the gate.
 * @param qubit_index Which qubit this gate is applied to.
 * @param parts A list of parts (indicates where the control bits are). Size must be equal to the
 * number of qubits.
 * @return The state vector after applying the given gate.
 */
[[nodiscard]] std::vector<Complex> QubitWiseMultiply(const std::vector<Complex> &in,
                                                     Circuit::GridSize_T num_qubits,
                                                     const Circuit::Matrix_T *matrix,
                                                     Circuit::GridSize_T qubit_index,
                                                     const std::vector<Circuit::Part> &parts);

/**
 * Simulate the effect of a swap gate on a state vector.
 *
 * @param in The input state vector.
 * @param num_qubits The number of qubits.
 * @param qubit_a The first qubit to swap
 * @param qubit_b The second qubit to swap
 * @param parts The list of parts (indicates where the control bits are).
 * @return
 */
[[nodiscard]] std::vector<Complex> ApplySwap(const std::vector<Complex> &in,
                                             Circuit::GridSize_T num_qubits,
                                             Circuit::GridSize_T qubit_a,
                                             Circuit::GridSize_T qubit_b,
                                             const std::vector<Circuit::Part> &parts);

/**
 * Simulate an entire circuit using the Qubit-wise Multiply Method.
 * @param circuit The circuit to simulate
 * @param in The input to the circuit (the starting state vector)
 * @return The state vector after simulating the circuit.
 */
[[nodiscard]] std::vector<Complex> SimulateCircuitQubitWise(const Circuit &circuit,
                                                            const std::vector<Complex> &in);

/**
 * Simulate an entire circuit using the Qubit-wise Multiply Method.
 * Starts with all qubits initialized to zero.
 * @param circuit The circuit to simulate
 * @return The state vector after simulating the circuit.
 */
[[nodiscard]] std::vector<Complex> SimulateCircuitQubitWise(const Circuit &circuit);

/**
 * Simulate an entire layer using the Qubit-wise Multiply Method
 * @param in The input state vector
 * @param num_qubits The number of qubits in the layer
 * @param matrix_list The list of matrices in this layer
 * @param parts_list  The list of parts in this layer
 * @return The s
 */
[[nodiscard]] std::vector<Complex> SimulateLayerQubitWise(
    const std::vector<Complex> &in, Circuit::GridSize_T num_qubits,
    const std::vector<const Circuit::Matrix_T *> &matrix_list,
    const std::vector<Circuit::Part> &parts_list);

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
