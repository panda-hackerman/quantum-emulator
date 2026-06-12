//
// Created by Eli Michaud on 6/12/2026.
//

#include "complex_matrix.h"

#include <format>

inline IComplexMatrix::Complex *ComplexMatrix::MatrixRow::GetItr(const ComplexMatrix *matrix,
                                                          const std::size_t row) {
  return matrix->entries_ + matrix->GetIndex(row, 1);
}

ComplexMatrix::MatrixRow::MatrixRow(const ComplexMatrix *matrix, const std::size_t row) :
    std::span<Complex>(GetItr(matrix, row), matrix->num_cols_) {
}

IComplexMatrix::Complex &ComplexMatrix::At(const std::size_t row, const std::size_t col) const {
  if (row > num_rows_ || col > num_cols_ || row < 1 || col < 1) {
    throw std::out_of_range(std::format("Index ({}, {}) is out of bounds for matrix of size {}x{}",
                                        row, col, num_rows_, num_cols_));
  }

  return entries_[GetIndex(row, col)];
}

ComplexMatrix::MatrixRow ComplexMatrix::At(const std::size_t row) const {
  if (row > num_rows_ || row < 1) {
    throw std::out_of_range(
        std::format("Row {} is out of bounds for matrix of size {}x{}", row, num_rows_, num_cols_));
  }

  return MatrixRow{this, row};
}

void ComplexMatrix::Set(std::size_t row, std::size_t col, const Complex entry) {
  if (row > num_rows_ || col > num_cols_ || row < 1 || col < 1) {
    throw std::out_of_range(std::format("Index ({}, {}) is out of bounds for matrix of size {}x{}",
                                        row, col, num_rows_, num_cols_));
  }

  entries_[GetIndex(row, col)] = entry;
}

ComplexMatrix::~ComplexMatrix() {
  if (entries_) {
    delete[] entries_;
    entries_ = nullptr;
  }
}