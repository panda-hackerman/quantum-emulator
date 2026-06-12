//
// Created by Eli Michaud on 6/12/2026.
//

#ifndef QMATH_H
#define QMATH_H
#include <cassert>
#include <complex>
#include <span>

class IComplexMatrix {
public:
  using Complex = std::complex<float>;

  virtual ~IComplexMatrix() = default;

  /// Get the entry at the specified index
  [[nodiscard]] virtual Complex &At(std::size_t row, std::size_t col) const = 0;

  [[nodiscard]] virtual std::size_t NumRows() const noexcept = 0; ///< @return The number of rows
  [[nodiscard]] virtual std::size_t NumCols() const noexcept = 0; ///< @return The number of columns

  /// Set the entry at the specified index to the given value
  virtual void Set(std::size_t row, std::size_t col, Complex entry) = 0;
};

class ComplexMatrix final : IComplexMatrix {
private:
  const unsigned num_rows_;
  const unsigned num_cols_;

  Complex *entries_;

protected:
  [[nodiscard]] std::size_t GetIndex(const std::size_t row, const std::size_t col) const noexcept {
    return (col - 1) + num_cols_ * (row - 1);
  }

public:
  class MatrixRow : public std::span<Complex> {
    friend class ComplexMatrix;
    static Complex *GetItr(const ComplexMatrix *matrix, std::size_t row);
    MatrixRow(const ComplexMatrix *matrix, std::size_t row);
  };

  ComplexMatrix(const unsigned rows, const unsigned cols) :
      num_rows_(rows),
      num_cols_(cols),
      entries_(new Complex[static_cast<std::size_t>(rows) * static_cast<std::size_t>(cols)]) {

    const std::size_t size = rows * cols;
    assert(size > 0 && "Cannot have a matrix with size 0");

    for (std::size_t i = 0; i < size; ++i) {
      entries_[i] = {0, 0};
    }
  }

  [[nodiscard]] std::size_t NumRows() const noexcept override { return num_rows_; }
  [[nodiscard]] std::size_t NumCols() const noexcept override { return num_cols_; }

  void Set(std::size_t row, std::size_t col, Complex entry) override;

  [[nodiscard]] Complex &At(std::size_t row, std::size_t col) const override;
  [[nodiscard]] MatrixRow At(std::size_t row) const;
  MatrixRow operator[](const std::size_t row) const { return At(row); }

  ~ComplexMatrix() override;
};

#endif // QMATH_H
