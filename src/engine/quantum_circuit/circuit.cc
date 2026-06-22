//
// Created by Eli Michaud on 6/12/2026.
//

#include "circuit.h"

#include <format>
#include <utility>

void Circuit::SetNumQubits(const GridSize_T new_num_qubits) {
  SetNewSize(new_num_qubits, num_layers_);
}

void Circuit::SetNumLayers(const GridSize_T new_num_layers) {
  SetNewSize(num_qubits_, new_num_layers);
}

void Circuit::SetNewSize(const GridSize_T new_num_qubits, const GridSize_T new_num_layers) {
  if (new_num_qubits < gates::kMinCircuitQubits || new_num_qubits > gates::kMaxCircuitQubits) {
    throw std::invalid_argument("Number of qubits is not within valid range!");
  }

  part_id_grid_ = part_id_grid_.WithNewSize(new_num_qubits, new_num_layers);
}

void Circuit::AddNewGate(const PartID id, CircuitPart part) {
  if (!gates_.Contains(id)) {
    gates_.Insert(id, std::move(part));
  }
}

void Circuit::SetCircuitPart(const GridSize_T qubit, const GridSize_T layer, const PartID id) {
  if (!gates_.Contains(id)) {
    throw std::invalid_argument("Trying to assign a part, but a part with that ID doesn't exist!");
  }

  part_id_grid_.At(qubit, layer) = id;
}

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