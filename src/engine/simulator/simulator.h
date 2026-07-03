/**
 * @file simulator.h
 *
 * @brief Different functions to simulate running the circuit.
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
 * \code (I⊗ ... ⊗I⊗U⊗I⊗ ... ⊗I) * |a>\endcode, where I is the (2x2) identity matrix.
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
[[nodiscard]] inline auto SimulateQubitWiseMultiply(const std::vector<Complex> &in,
                                                    const Circuit::GridSize_T num_qubits,
                                                    const Circuit::Matrix_T *matrix,
                                                    const Circuit::GridSize_T qubit_index,
                                                    const std::vector<Circuit::Part> &parts) {

  if (num_qubits != parts.size()) {
    throw std::invalid_argument("Must have as many parts as qubits!");
  }

  if (matrix->NumRows() != 2 || matrix->NumCols() != 2) {
    throw std::invalid_argument("Qubit-wise multiply must be called with a 2x2 matrix!");
  }

  if (qubit_index >= num_qubits) {
    throw std::invalid_argument("Qubit index must be lower than the number of qubits!");
  }

  if (qubit_index < 0 || qubit_index > Circuit::kMaxQubits) {
    throw std::invalid_argument(std::format("Qubit index must be within the range 0 to {}! Got: {}",
                                            Circuit::kMaxQubits, qubit_index));
  }

  const int state_vector_size = (1 << num_qubits);  // 2^n

  if (in.size() != state_vector_size) {
    throw std::invalid_argument("State vector size must be equal to 2^n, where n is the "
                                "number of qubits!");
  }

  // Output state vector
  std::vector<Complex> out = in;

  // Calculate masks for control qubits
  int inclusion_mask = 0;
  int desired_mask = 0;

  for (std::size_t i = 0; i < parts.size(); ++i) {
    const int bit = 1 << i;

    if (parts[i] == Circuit::Part::kControlBit) {
      inclusion_mask |= bit;
      desired_mask |= bit;
    } else if (parts[i] == Circuit::Part::kAntiControlBit) {
      inclusion_mask |= bit;
    }
  }

  const int half_block_size = (1 << qubit_index);   // 2^q
  const int block_size = half_block_size * 2;

  const int num_blocks = state_vector_size / block_size;

  for (int block = 0; block < num_blocks; ++block) {
    const int block_start = block * block_size;

    // Loop through the block
    for (int block_idx = 0; block_idx < half_block_size; ++block_idx) {
      const int idx_1 = block_start + block_idx;
      const int idx_2 = idx_1 + half_block_size;

      if ((idx_1 & inclusion_mask) != desired_mask) continue;

      out[idx_1] = (*matrix)[0][0] * in[idx_1] + (*matrix)[0][1] * in[idx_2];
      out[idx_2] = (*matrix)[1][0] * in[idx_1] + (*matrix)[1][1] * in[idx_2];
    }
  }

  return out;
}

// // TODO: Control Bits
// [[nodiscard]] constexpr auto ComputeLayer(const std::vector<Circuit::Part> &parts,
//                                           const std::vector<const Circuit::Matrix_T *> &matrices)
//                                           {
//   if (parts.size() != matrices.size()) {
//     throw std::invalid_argument("Layer parts list and matrix list must be the same length");
//   }
//
//   // We initialize to the 1x1 identity matrix = [1]
//   Matrix2D<std::complex<float>> out_matrix{1, 1};
//   out_matrix.At(0, 0) = 1;
//
//   // Number of qubits
//   const Circuit::GridSize_T n_qubits = static_cast<Circuit::GridSize_T>(parts.size());
//
//   // = [1] ⊗ [U_{n-1}] ⊗ ... ⊗ [U_0]
//   for (int i = (n_qubits - 1); i >= 0; --i) {
//     if (parts[i] == Circuit::Part::kMatrix2x2) {
//       out_matrix = out_matrix.Tensor(*matrices[i]);
//     } else {
//       out_matrix = out_matrix.Tensor(matrix::kIdentity);
//     }
//   }
//
//   return out_matrix;
// }

#endif // SIMULATOR_H
