/**
 * @file simulator.cc
 * @author Eli Michaud
 * @since 7/3/2026
 */

#include "simulator.h"

#include "math/bitwise_math.h"
#include "math/constants.h"

void ApplyMatrixQubitWise(StateVector &state_vector, const Circuit::Matrix_T *matrix,
                          const Circuit::GridSize_T qubit_index,
                          const std::span<const Circuit::Part> parts) {

  if (state_vector.NumQubits() != parts.size()) {
    throw std::invalid_argument("Number of qubits must match number of parts!");
  }

  if (matrix->NumRows() != 2 || matrix->NumCols() != 2) {
    // TODO: Support different matrix sizes
    throw std::invalid_argument("Qubit-wise multiply must be called with a 2x2 matrix!");
  }

  if (qubit_index >= state_vector.NumQubits()) {
    throw std::invalid_argument("Qubit index must be lower than the number of qubits!");
  }

  // Output state vector (copying object)
  const StateVector out = state_vector;

  // Calculate masks for control qubits
  int inclusion_mask = 0;
  int desired_mask = 0;

  for (std::size_t i = 0; i < parts.size(); ++i) {
    const int bit = static_cast<int>(bit::TwoPowN(i)); // 2^i

    if (parts[i] == Circuit::Part::kControlBit) {
      inclusion_mask |= bit;
      desired_mask |= bit;
    } else if (parts[i] == Circuit::Part::kAntiControlBit) {
      inclusion_mask |= bit;
    }
  }

  // Apply qubit-wise multiply
  const int half_block_size = bit::TwoPowN(qubit_index);   // 2^q
  const int block_size = half_block_size * 2;

  const int num_blocks = static_cast<int>(out.Size() / block_size);

  for (int block = 0; block < num_blocks; ++block) {
    const int block_start = block * block_size;

    // Loop through the block
    for (int block_idx = 0; block_idx < half_block_size; ++block_idx) {
      const int idx_1 = block_start + block_idx;
      const int idx_2 = idx_1 + half_block_size;

      if ((idx_1 & inclusion_mask) != desired_mask) continue;

      out[idx_1] = (*matrix)[0][0] * state_vector[idx_1] + (*matrix)[0][1] * state_vector[idx_2];
      out[idx_2] = (*matrix)[1][0] * state_vector[idx_1] + (*matrix)[1][1] * state_vector[idx_2];
    }
  }

  state_vector = out;
}

void ApplySwap(StateVector &state_vector, const Circuit::GridSize_T qubit_a,
               const Circuit::GridSize_T qubit_b, const std::span<const Circuit::Part> parts) {

  if (state_vector.NumQubits() != parts.size()) {
    throw std::invalid_argument("Number of qubits must match number of parts!");
  }

  if (qubit_a < 0 || qubit_a > Circuit::kMaxQubits) {
    throw std::invalid_argument(std::format("Qubit index must be within the range 0 to {}! Got: {}",
                                            Circuit::kMaxQubits, qubit_a));
  }

  if (qubit_b < 0 || qubit_b > Circuit::kMaxQubits) {
    throw std::invalid_argument(std::format("Qubit index must be within the range 0 to {}! Got: {}",
                                            Circuit::kMaxQubits, qubit_a));
  }

  if (qubit_a == qubit_b) return; // We're done !

  // Output state vector (Copying object)
  const StateVector out = state_vector;

  // Calculate masks for control qubits
  int inclusion_mask = 0;
  int desired_mask = 0;

  for (std::size_t i = 0; i < parts.size(); ++i) {
    const int bit = static_cast<int>(bit::TwoPowN(i)); // 2^i

    if (parts[i] == Circuit::Part::kControlBit) {
      inclusion_mask |= bit;
      desired_mask |= bit;
    } else if (parts[i] == Circuit::Part::kAntiControlBit) {
      inclusion_mask |= bit;
    }
  }

  // Apply swap
  const int mask_a = bit::TwoPowN(qubit_a);
  const int mask_b = bit::TwoPowN(qubit_b);

  for (int k = 0; k < out.Size(); ++k) {
    if ((k & inclusion_mask) != desired_mask) continue; // Skip

    const auto ath_bit_of_k = bit::GetBit<int>(k, qubit_a);
    const auto bth_bit_of_k = bit::GetBit<int>(k, qubit_b);

    if (ath_bit_of_k == 1 && bth_bit_of_k == 0) {
      const int k_2 = (k & (~mask_a)) | mask_b; // Turn off bit A, turn on bit B.

      // Swap bits
      out[k_2] = state_vector[k];
      out[k] = state_vector[k_2];
    }
  }

  state_vector = out;
}

void ApplyCircuitQubitWise(const Circuit &circuit, StateVector &state_vector) {

  const Circuit::GridSize_T num_qubits = circuit.GetNumQubits();
  const Circuit::GridSize_T num_layers = circuit.GetNumLayers();

  if (state_vector.NumQubits() != num_qubits) {
    throw std::invalid_argument("State vector size doesn't match number of input qubits!");
  }

  for (Circuit::GridSize_T layer = 0; layer < num_layers; ++layer) {
    ApplyLayerQubitWise(state_vector, circuit, layer);
  }
}

void ApplyLayerQubitWise(StateVector &state_vector, const Circuit &circuit,
                         const Circuit::GridSize_T layer) {

  static constexpr std::size_t invalid_qubit_index = Circuit::kMaxQubits + 1;

  const auto matrix_list = circuit.GetMatricesInLayer(layer);
  auto parts_list = circuit.GetPartsInLayer(layer);

  /// Indices of swap gates (if they exist)
  std::array<std::size_t, 2> swap_indices = {{invalid_qubit_index, invalid_qubit_index}};

  for (Circuit::GridSize_T qubit = 0; qubit < circuit.GetNumQubits(); ++qubit) {
    if (parts_list[qubit] == Circuit::Part::kMatrix2x2) {
      ApplyMatrixQubitWise(state_vector, matrix_list[qubit], qubit, parts_list);
    } else if (parts_list[qubit] == Circuit::Part::kSwap) {
      const int idx = swap_indices[0] == invalid_qubit_index ? 0 : 1;
      swap_indices[idx] = qubit;
    }
  }

  if (swap_indices[0] != invalid_qubit_index && swap_indices[1] != invalid_qubit_index) {
    ApplySwap(state_vector, swap_indices[0], swap_indices[1], parts_list);
  }
}

Matrix2D<Complex> ComputeLayerExplicit(const std::vector<Circuit::Part> &parts,
                                       const std::vector<const Circuit::Matrix_T *> &matrices) {
  if (parts.size() != matrices.size()) {
    throw std::invalid_argument("Layer parts list and matrix list must be the same length");
  }

  // We initialize to the 1x1 identity matrix = [1]
  Matrix2D<Complex> out_matrix{1, 1};
  out_matrix.At(0, 0) = 1;

  // Number of qubits
  const Circuit::GridSize_T n_qubits = static_cast<Circuit::GridSize_T>(parts.size());

  // = [1] ⊗ [U_{n-1}] ⊗ ... ⊗ [U_0]
  for (int i = (n_qubits - 1); i >= 0; --i) {
    if (parts[i] == Circuit::Part::kMatrix2x2) {
      out_matrix = out_matrix.Tensor(*(matrices[i]));
    } else {
      out_matrix = out_matrix.Tensor(matrix::kIdentity);
    }
  }

  return out_matrix;
}
