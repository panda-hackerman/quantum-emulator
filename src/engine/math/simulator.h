//
// Created by Eli Michaud on 6/26/2026.
//

#ifndef SIMULATOR_H
#define SIMULATOR_H
#include "quantum_circuit/circuit.h"

// TODO: Control Bits
[[nodiscard]] constexpr auto ComputeLayer(const std::vector<Circuit::Part> &parts,
                                          const std::vector<const Circuit::Matrix_T *> &matrices) {
  if (parts.size() != matrices.size()) {
    throw std::invalid_argument("Layer parts list and matrix list must be the same length");
  }

  // We initialize to the 1x1 identity matrix = [1]
  Matrix2D<std::complex<float>> out_matrix{1, 1};
  out_matrix.At(0, 0) = 1;

  // = [1] ⊗ [U_{n-1}] ⊗ ... ⊗ [U_0]
  const int n_qubits = parts.size(); // Number of qubits
  for (int i = (n_qubits - 1); i >= 0; --i) {
    if (parts[i] == Circuit::Part::kMatrix2x2) {
      out_matrix = out_matrix.Tensor(*matrices[i]);
    } else {
      out_matrix = out_matrix.Tensor(matrix::kIdentity);
    }
  }

  return out_matrix;
}

#endif // SIMULATOR_H
