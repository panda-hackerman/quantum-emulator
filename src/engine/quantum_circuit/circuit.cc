//
// Created by Eli Michaud on 6/12/2026.
//

#include "circuit.h"

#include <iostream>

void Circuit::AddEmpty(const GridSize_T qubit, const GridSize_T layer) {
  id_to_matrix_.Remove(IndexOf(qubit, layer));
  parts_array_[IndexOf(qubit, layer)] = Part::kEmpty;
}

void Circuit::AddGate(const GridSize_T qubit, const GridSize_T layer, const Matrix_T *matrix) {
  if (matrix->NumCols() != 2 || matrix->NumRows() != 2) {
    throw std::invalid_argument("A single qubit gate must be a 2x2 matrix!");
  }

  id_to_matrix_.Insert(IndexOf(qubit, layer), matrix);
  parts_array_[IndexOf(qubit, layer)] = Part::kMatrix2x2;
}

void Circuit::AddControlBit(const GridSize_T qubit, const GridSize_T layer) {
  id_to_matrix_.Remove(IndexOf(qubit, layer));
  parts_array_[IndexOf(qubit, layer)] = Part::kControlBit;
}

void Circuit::AddAntiControlBit(const GridSize_T qubit, const GridSize_T layer) {
  id_to_matrix_.Remove(IndexOf(qubit, layer));
  parts_array_[IndexOf(qubit, layer)] = Part::kAntiControlBit;
}

void Circuit::AddMeasurement(const GridSize_T qubit, const GridSize_T layer) {
  id_to_matrix_.Remove(IndexOf(qubit, layer));
  parts_array_[IndexOf(qubit, layer)] = Part::kEmpty;
}

void Circuit::AddSwap(const GridSize_T qubit, const GridSize_T layer) {
  // TODO: Check if swap is valid?
  id_to_matrix_.Remove(IndexOf(qubit, layer));
  parts_array_[IndexOf(qubit, layer)] = Part::kSwap;
}

void Circuit::SetNumQubits(const GridSize_T num_qubits) {
  AssertInLimit(num_qubits, num_layers_);
  num_qubits_ = num_qubits;
}

void Circuit::SetNumLayers(GridSize_T num_layers) {
  AssertInLimit(num_qubits_, num_layers);
  num_layers_ = num_layers;
}

void Circuit::SetSize(const GridSize_T num_qubits, const GridSize_T num_layers) {
  AssertInLimit(num_qubits, num_layers);
  num_qubits_ = num_qubits;
  num_layers_ = num_layers;
}

void Circuit::Clear() {
  parts_array_.fill(Part::kEmpty);
  id_to_matrix_.Clear();
}

Circuit::Part Circuit::GetPartTypeAt(const GridSize_T qubit, const GridSize_T layer) const {
  AssertInLimit(qubit, layer);
  return GetPartTypeUnsafe(qubit, layer);
}

const Circuit::Matrix_T *Circuit::GetMatrixAt(const GridSize_T qubit, const GridSize_T layer) const {
  AssertInLimit(qubit, layer);
  return GetMatrixUnsafe(qubit, layer);
}

Circuit::Part Circuit::GetPartTypeUnsafe(const GridSize_T qubit, const GridSize_T layer) const {
  return parts_array_[IndexOf(qubit, layer)];
}

const Circuit::Matrix_T *Circuit::GetMatrixUnsafe(const GridSize_T qubit,
                                                const GridSize_T layer) const {
  if (id_to_matrix_.Contains(IndexOf(qubit, layer))) {
    return id_to_matrix_.Get(IndexOf(qubit, layer));
  }

  return nullptr;
}

std::vector<Circuit::Part> Circuit::GetPartsInLayer(const GridSize_T layer) const {
  AssertInRange(0, layer);

  const auto arr_begin = parts_array_.begin();

  const auto first = arr_begin + IndexOf(0, layer);
  const auto last = first + num_qubits_;

  return {first, last};
}

std::vector<const Circuit::Matrix_T *> Circuit::GetMatricesInLayer(const GridSize_T layer) const {
  AssertInRange(0, layer);

  std::vector<const Matrix_T *> out(num_qubits_, nullptr);

  for (GridSize_T qubit = 0; qubit < num_qubits_; ++qubit) {
    if (parts_array_[IndexOf(qubit, layer)] == Part::kMatrix2x2) {
      out[qubit] = id_to_matrix_.Get(IndexOf(qubit, layer));
    }
  }

  return out;
}
