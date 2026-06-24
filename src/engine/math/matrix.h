//
// Created by Eli Michaud on 6/13/2026.
//

#ifndef MATRIX_H
#define MATRIX_H

#include <algorithm>
#include <array>
#include <iomanip>
#include <ranges>
#include <span>
#include <stdexcept>
#include <vector>

#include "util/print_util.h"

/// Relating to matrices
namespace matrix {
inline constexpr std::size_t kDynamicSize = 0;

/// Convert a 2D array into a flat (1D) array.
template <typename T, std::size_t R, std::size_t C>
static constexpr std::array<T, R * C> Flatten2D(const std::array<std::array<T, C>, R> &entries) {
  auto r = std::ranges::join_view(entries);

  return [&]<std::size_t... Is>(std::index_sequence<Is...>) -> std::array<T, R * C> {
    auto it = std::ranges::begin(r);
    return {(Is, *it++)...};
  }(std::make_index_sequence<R * C>{});
}
} // namespace matrix

/**
 * Interface for a 2D matrix of values.
 * @tparam T The type of the entries of the matrix.
 */
template <typename T>
class IMatrix2D {
public:
  virtual ~IMatrix2D() = default;

  [[nodiscard]] virtual constexpr std::size_t NumRows() const noexcept = 0;
  [[nodiscard]] virtual constexpr std::size_t NumCols() const noexcept = 0;

  [[nodiscard]] virtual constexpr T &At(std::size_t row, std::size_t col) = 0;
  [[nodiscard]] virtual constexpr const T &At(std::size_t row, std::size_t col) const = 0;

  constexpr std::ostream &Print(std::ostream &os) const;
};

/**
 * Matrix data, with a non-dynamic size (e.g., known at compile-time).
 * @tparam T The type of the entries of the matrix
 * @tparam Rows Number of rows
 * @tparam Cols Number of columns
 * @note Stored as flattened std::array
 */
template <class T, std::size_t Rows, std::size_t Cols>
struct MatrixDataType {
  std::array<T, Rows * Cols> entries;

  constexpr explicit(false) MatrixDataType(const std::array<T, Rows * Cols> &entries) :
      entries{entries} {}

  constexpr virtual ~MatrixDataType() = default;
  [[nodiscard]] virtual constexpr T *Data() noexcept { return entries.data(); }
  [[nodiscard]] virtual constexpr const T *Data() const noexcept { return entries.data(); }

  constexpr MatrixDataType(const MatrixDataType &other) : entries(other.entries) {}
  constexpr MatrixDataType(MatrixDataType &&other) noexcept : entries(std::move(other.entries)) {}

  constexpr MatrixDataType &operator=(const MatrixDataType &other) {
    if (this != &other) entries = other.entries;
    return *this;
  }

  constexpr MatrixDataType &operator=(MatrixDataType &&other) noexcept {
    if (this != &other) entries = std::move(other.entries);
    return *this;
  }
};

/**
 * Matrix data, with a dynamic size (e.g., not known at compile-time)
 * @tparam T The type of the entries of the matrix
 * @note Stores a pointer, and the data is allocated on the heap. This class manages its own memory.
 */
template <class T>
struct MatrixDataType<T, matrix::kDynamicSize, matrix::kDynamicSize> {
  std::vector<T> entries;

  constexpr MatrixDataType(const std::size_t rows, const std::size_t cols) : entries(rows * cols) {}

  constexpr virtual ~MatrixDataType() = default;
  [[nodiscard]] virtual constexpr T *Data() noexcept { return entries.data(); }
  [[nodiscard]] virtual constexpr const T *Data() const noexcept { return entries.data(); }

  constexpr MatrixDataType(const MatrixDataType &other) : entries(other.entries) {}
  constexpr MatrixDataType(MatrixDataType &&other) noexcept : entries(std::move(other.entries)) {}

  constexpr MatrixDataType &operator=(const MatrixDataType &other) {
    if (this != &other) entries = other.entries;
    return *this;
  }

  constexpr MatrixDataType &operator=(MatrixDataType &&other) noexcept {
    if (this != &other) entries = std::move(other.entries);
    return *this;
  }
};

/**
 * A 2-dimensional matrix.
 * @tparam T The type of the entries of the matrix
 * @tparam Rows The number of rows
 * @tparam Cols The number of columns
 */
template <typename T, std::size_t Rows = matrix::kDynamicSize,
          std::size_t Cols = matrix::kDynamicSize>
  requires((Rows != matrix::kDynamicSize && Cols != matrix::kDynamicSize) ||
           (Rows == matrix::kDynamicSize && Cols == matrix::kDynamicSize))
class Matrix2D : public IMatrix2D<T>,
                 protected MatrixDataType<T, Rows, Cols> {
public:
  static constexpr bool kIsDynamic = Rows == matrix::kDynamicSize && Cols == matrix::kDynamicSize;

  template <typename S>
  using RowSpan_T = std::conditional_t<kIsDynamic, std::span<S>, std::span<S, Cols>>;

  /**
   * A single row of a matrix.
   * @tparam S The type of the elements in the row.
   * This is either the same as the type of elements in the matrix (T), or const T.
   */
  template <typename S>
    requires(std::is_same_v<T, S> || std::is_same_v<T, std::remove_const_t<S>>)
  class MatrixRow : public RowSpan_T<S> {
  private:
    constexpr explicit(false) MatrixRow(S *first, std::size_t count) : RowSpan_T<S>{first, count} {}

  public:
    static constexpr MatrixRow Create(const Matrix2D *m, const std::size_t row) {
      S *first = m->Data() + m->GetIndex(row, 0);
      const std::size_t count = m->NumCols();

      return MatrixRow{first, count};
    }
  };

  using Row_T = MatrixRow<T>;
  using RowConst_T = MatrixRow<const T>;
  using DataType = MatrixDataType<T, Rows, Cols>;

protected:
  std::size_t num_rows_;
  std::size_t num_cols_;

  [[nodiscard]]
  constexpr std::size_t GetIndex(const std::size_t row, const std::size_t col) const noexcept {
    return col + NumCols() * row;
  }

public:
  constexpr ~Matrix2D() override = default;

  constexpr explicit(false) Matrix2D(const std::size_t num_rows, const std::size_t num_cols)
    requires(kIsDynamic)
      : DataType{num_rows, num_cols}, num_rows_{num_rows}, num_cols_{num_cols} {}

  constexpr explicit(false) Matrix2D(const std::array<std::array<T, Cols>, Rows> &entries)
    requires(!kIsDynamic)
      : DataType{matrix::Flatten2D(entries)}, num_rows_{Rows}, num_cols_{Cols} {}

  [[nodiscard]] constexpr std::size_t NumRows() const noexcept override { return num_rows_; }
  [[nodiscard]] constexpr std::size_t NumCols() const noexcept override { return num_cols_; }

  [[nodiscard]] constexpr T &At(const std::size_t row, const std::size_t col) override {
    if (row >= NumRows() || col >= NumCols()) XOutOfRange();
    return DataType::Data()[GetIndex(row, col)];
  }

  [[nodiscard]] constexpr const T &At(const std::size_t row, const std::size_t col) const override {
    if (row >= NumRows() || col >= NumCols()) XOutOfRange();
    return DataType::Data()[GetIndex(row, col)];
  }

  [[nodiscard]] constexpr Row_T At(const std::size_t row) {
    if (row >= NumRows()) XOutOfRange();
    return Row_T::Create(this, row);
  }

  [[nodiscard]] virtual constexpr RowConst_T At(const std::size_t row) const {
    if (row >= NumRows()) XOutOfRange();
    return RowConst_T::Create(this, row);
  }

  [[nodiscard]] constexpr Row_T operator[](const std::size_t row) { return At(row); }
  [[nodiscard]] constexpr RowConst_T operator[](const std::size_t row) const { return At(row); }

  friend bool operator==(const Matrix2D &lhs, const Matrix2D &rhs) {
    if (lhs.num_rows_ != rhs.num_rows_ || lhs.num_cols_ != rhs.num_cols_) {
      return false; // Must be the same size
    }

    for (std::size_t r = 0; r < lhs.num_rows_; ++r) {
      for (std::size_t c = 0; c < lhs.num_cols_; ++c) {
        if (lhs.At(r, c) != rhs.At(r, c)) return false;
      }
    }

    return true;
  }

  friend bool operator!=(const Matrix2D &lhs, const Matrix2D &rhs) { return !(lhs == rhs); }

  Matrix2D<T> WithNewSize(const std::size_t new_rows, const std::size_t new_cols) {
    if (new_cols == num_cols_ && new_rows == num_rows_) {
      return *this;
    }

    Matrix2D<T> out{new_rows, new_cols};

    const std::size_t min_row = std::min(num_rows_, new_rows);
    const std::size_t min_col = std::min(num_cols_, new_cols);

    for (std::size_t r = 0; r < min_row; ++r) {
      for (std::size_t c = 0; c < min_col; ++c) {
        out.At(r, c) = At(r, c);
      }
    }

    return out;
  }

protected:
  [[noreturn]] static void XOutOfRange() { throw std::out_of_range("Invalid index for matrix."); }
};

template <typename T>
constexpr std::ostream &IMatrix2D<T>::Print(std::ostream &os) const {
  std::vector<std::string> strings;
  strings.reserve(NumRows() * NumCols());

  std::vector<std::size_t> width_by_col(NumCols(), 0);

  for (int r = 0; r < NumRows(); ++r) {
    for (int c = 0; c < NumCols(); ++c) {
      std::string str = PrettyPrint(At(r, c));

      if (const std::size_t str_size = str.size(); str_size > width_by_col.at(c)) {
        width_by_col.at(c) = str_size;
      }

      strings.emplace_back(std::move(str));
    }
  }

  for (int r = 0; r < NumRows(); ++r) {
    os << "[ ";
    for (std::size_t c = 0; c < NumCols(); ++c) {
      const std::size_t index = c + NumCols() * r;

      os << std::setw(width_by_col.at(c)) << std::left << strings.at(index) << " ";
    }
    os << "]\n";
  }

  return os;
}

#endif // MATRIX_H
