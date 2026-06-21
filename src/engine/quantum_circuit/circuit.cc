// //
// // Created by Eli Michaud on 6/12/2026.
// //
//
// #include "circuit.h"
//
// #include <format>
//
// Circuit::Gate::Gate(std::string name, const IComplexMatrix *matrix) :
//     name(std::move(name)), matrix(matrix) {
//   assert(matrix->NumRows() == matrix->NumCols() && "Must be a square matrix");
//   assert((matrix->NumRows() & (matrix->NumRows() - 1)) == 0 && "Matrix must be a power of 2");
// }
//
// int Circuit::Gate::NumQubits() const {
//   return static_cast<int>(std::log2(matrix->NumRows()));
// }
//
// void Circuit::SetNumQubits(const std::uint8_t new_num_qubits) {
//   if (new_num_qubits != num_qubits_) {
//     for (CircuitLayer &layer : layers_) {
//       layer.resize(new_num_qubits, Gate{});
//     }
//   }
//
//   num_qubits_ = new_num_qubits;
// }
//
// void Circuit::SetNumLayers(const std::uint8_t new_num_layers) {
//   if (new_num_layers != num_layers_) {
//     layers_.resize(new_num_layers, CircuitLayer{num_qubits_, Gate{}});
//   }
//
//   num_layers_ = new_num_layers;
// }
//
// void Circuit::SetNewSize(const std::uint8_t new_num_qubits, const std::uint8_t new_num_layers) {
//   if (new_num_layers == num_layers_) {
//     SetNumQubits(new_num_qubits);
//     return;
//   }
//
//   if (new_num_qubits == num_qubits_) {
//     SetNumLayers(new_num_layers);
//     return;
//   }
//
//   if (new_num_layers > num_layers_) {
//     for (CircuitLayer &layer : layers_) {
//       layer.resize(new_num_qubits, Gate{});
//     }
//
//     layers_.resize(new_num_layers, CircuitLayer{new_num_qubits, Gate{}});
//   }
//
//   if (new_num_layers < num_layers_) {
//     layers_.resize(new_num_layers);
//
//     for (CircuitLayer &layer : layers_) {
//       layer.resize(new_num_qubits, Gate{});
//     }
//   }
//
//   num_qubits_ = new_num_qubits;
//   num_layers_ = new_num_layers;
// }
//
// void Circuit::SetGate(std::uint8_t qubit, std::uint8_t layer, const Gate &gate) {
//   if (qubit >= num_qubits_ || layer >= num_layers_) {
//     throw std::out_of_range(
//         std::format("Can't set qubit {} on layer {}: the circuit only has {} qubits and {} layers!",
//                     qubit, layer, num_qubits_, num_layers_));
//   }
//
//   layers_.at(layer).at(qubit) = gate;
// }
//
// Circuit BuildExampleCircuit() {
//
//   Circuit circuit{3, 4};
//
//   const Circuit::Gate pauli_x{"X", &gates::kPauliX};
//   const Circuit::Gate pauli_z{"Z", &gates::kPauliZ};
//   const Circuit::Gate hadamard{"H", &gates::kHadamard};
//
//   // Layer 0
//   circuit.SetGate(2, 0, pauli_x);
//   circuit.SetGate(1, 0, hadamard);
//
//
//
//   // circuit.SetGate()
//
//   return circuit;
// }
