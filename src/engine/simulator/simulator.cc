/**
 * @file simulator.cc
 * @author Eli Michaud
 * @since 7/3/2026
 */

#include "simulator.h"

#include "math/constants.h"

std::vector<Complex> QubitWiseMultiply(const std::vector<Complex> &in,
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

std::vector<Complex> SimulateCircuitQubitWise(const Circuit &circuit,
                                              const std::vector<Complex> &in) {

  const Circuit::GridSize_T num_qubits = circuit.GetNumQubits();
  const Circuit::GridSize_T num_layers = circuit.GetNumLayers();

  std::vector<Complex> state = in;

  for (Circuit::GridSize_T layer = 0; layer < num_layers; ++layer) {
    std::vector<const Circuit::Matrix_T *> matrix_list = circuit.GetMatricesInLayer(layer);
    std::vector<Circuit::Part> parts_list = circuit.GetPartsInLayer(layer);

    for (Circuit::GridSize_T qubit = 0; qubit < num_qubits; ++qubit) {
      if (parts_list[qubit] == Circuit::Part::kMatrix2x2) {
        state = QubitWiseMultiply(state, num_qubits, matrix_list[qubit], qubit, parts_list);
      }
    }
  }

  return state;
}

std::vector<Complex> SimulateCircuitQubitWise(const Circuit &circuit) {
  // Initialize state vector to = |0...0>
  std::vector<Complex> state(1 << circuit.GetNumQubits(), {0, 0});
  state[0] = 1;

  return SimulateCircuitQubitWise(circuit, state);
}

Matrix2D<Complex> ComputeLayerExplicit(const std::vector<Circuit::Part> &parts,
                                       const std::vector<const Circuit::Matrix_T *> &matrices) {
  if (parts.size() != matrices.size()) {
    throw std::invalid_argument("Layer parts list and matrix list must be the same length");
  }

  // We initialize to the 1x1 identity matrix = [1]
  Matrix2D<std::complex<float>> out_matrix{1, 1};
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
