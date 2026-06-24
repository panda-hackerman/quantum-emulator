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

  if (new_num_layers < gates::kMinCircuitDepth || new_num_layers > gates::kMaxCircuitDepth) {
    throw std::invalid_argument("Number of layers is not within valid range!");
  }

  part_id_grid_ = part_id_grid_.WithNewSize(new_num_qubits, new_num_layers);
  num_qubits_ = new_num_qubits;
  num_layers_ = new_num_layers;
}

void Circuit::SetCircuitPart(const GridSize_T qubit, const GridSize_T layer, const GateID id) {
  if (!gates::kIdToGateMap.Contains(id)) {
    throw std::invalid_argument("Trying to assign a part, but a part with that ID doesn't exist!");
  }

  if (!IsInBounds(qubit, layer)) {
    throw std::out_of_range("Tried to set circuit part that is out of range!");
  }

  part_id_grid_.At(qubit, layer) = id;
}

bool Circuit::IsInBounds(const GridSize_T qubit, const GridSize_T layer) const {
  if (qubit < 0 || qubit >= num_qubits_ || layer < 0 || layer >= num_layers_) {
    return false;
  }

  return true;
}

Circuit::GateID Circuit::GetIdAt(const GridSize_T qubit, const GridSize_T layer) {
  if (!IsInBounds(qubit, layer)) {
    throw std::out_of_range("Tried to access circuit gate that's out of range!");
  }

  return part_id_grid_.At(qubit, layer);
}

const CircuitPart &Circuit::GetCircuitPart(const GridSize_T qubit, const GridSize_T layer) {
  if (!IsInBounds(qubit, layer)) {
    throw std::out_of_range("Tried to access circuit gate that's out of range!");
  }

  const GateID id = part_id_grid_.At(qubit, layer);
  return gates::kIdToGateMap.Get(id);
}