/**
 * @file simulator.cc
 * @author Eli Michaud
 * @since 7/3/2026
 */

#include "simulator.h"

#include <numeric>

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

    const auto ath_bit_of_k = bit::GetBit(k, qubit_a);
    const auto bth_bit_of_k = bit::GetBit(k, qubit_b);

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
  const auto parts_list = circuit.GetPartsInLayer(layer);

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

Matrix2D<Complex> PartialDensityTraceImpl(const StateVector &state_vector,
                                          const std::span<const Circuit::GridSize_T> to_trace_out,
                                          const std::span<const Circuit::GridSize_T> to_keep) {

  const Circuit::GridSize_T num_qubits = state_vector.NumQubits();
  const std::size_t num_qubits_to_trace = to_trace_out.size();
  const std::size_t num_qubits_to_keep = to_keep.size();

  if (num_qubits_to_trace + num_qubits_to_keep != num_qubits) {
    throw std::invalid_argument("The qubits to trace out and to keep, together, "
                                "must contain every qubit!");
  }

  // Dimension of space being traced out
  const std::size_t trace_dimension = bit::TwoPowN(num_qubits_to_trace);
  const std::size_t out_size = bit::TwoPowN(num_qubits_to_keep);

  Matrix2D<Complex> output = {out_size, out_size};

  // Get rearranged lookup table
  std::vector<std::size_t> rearrange_lookup;
  rearrange_lookup.reserve(out_size);

  for (std::size_t i = 0; i < out_size; ++i) {
    rearrange_lookup.emplace_back(bit::RearrangeBits(i, to_keep));
  }

  // Generate 1st triangular half
  for (std::size_t shared_bits = 0; shared_bits < trace_dimension; ++shared_bits) {
    const std::size_t shared_rearranged = bit::RearrangeBits(shared_bits, to_trace_out);

    for (std::size_t output_row = 0; output_row < out_size; ++output_row) {
      const std::size_t input_row = shared_rearranged | rearrange_lookup[output_row];

      for (std::size_t output_col = 0; output_col <= output_row; ++output_col) {
        const std::size_t input_col = shared_rearranged | rearrange_lookup[output_col];

        // RHS is equivalent to M.At(input_row, input_col) where M = |p><p|
        output.At(output_row, output_col) = state_vector[input_row] * conj(state_vector[input_col]);
      }
    }
  }

  // Copy 2nd triangular half & conjugate
  for (std::size_t row = 0; row < out_size; ++row) {
    for (std::size_t col = 0; col < row; ++col) {
      output.At(col, row) = conj(output.At(row, col));
    }
  }

  return output;
}

Matrix2D<Complex> PartialDensityTraceOut(const StateVector &state_vector,
                                         const std::span<const Circuit::GridSize_T> to_trace_out) {

  const Circuit::GridSize_T num_qubits = state_vector.NumQubits();
  const std::size_t num_to_trace = to_trace_out.size();
  const std::size_t num_to_keep = num_qubits - num_to_trace;

  std::vector<Circuit::GridSize_T> to_keep;
  to_keep.reserve(num_to_keep);

  for (Circuit::GridSize_T qubit = 0; qubit < num_qubits; ++qubit) {
    if (!std::ranges::contains(to_trace_out, qubit)) {
      to_keep.push_back(qubit);
    }
  }

  return PartialDensityTraceImpl(state_vector, to_trace_out, to_keep);
}

Matrix2D<Complex> PartialDensityTraceIn(const StateVector &state_vector,
                                        const std::span<const Circuit::GridSize_T> to_keep) {

  const Circuit::GridSize_T num_qubits = state_vector.NumQubits();
  const std::size_t num_to_keep = to_keep.size();
  const std::size_t num_to_trace = num_qubits - num_to_keep;

  std::vector<Circuit::GridSize_T> to_trace_out;
  to_trace_out.reserve(num_to_trace);

  for (Circuit::GridSize_T qubit = 0; qubit < num_qubits; ++qubit) {
    if (!std::ranges::contains(to_keep, qubit)) {
      to_trace_out.push_back(qubit);
    }
  }

  return PartialDensityTraceImpl(state_vector, to_trace_out, to_keep);
}
