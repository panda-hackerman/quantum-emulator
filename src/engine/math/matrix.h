//
// Created by Eli Michaud on 6/13/2026.
//

#ifndef MATRIX_H
#define MATRIX_H

#include <array>
#include <complex>
#include <iomanip>
#include <ranges>
#include <span>
#include <stdexcept>
#include <vector>

#include "util/print_util.h"

namespace matrix {
inline constexpr std::size_t kDynamicSize = 0;

template <typename T, std::size_t N, typename Range>
static constexpr std::array<T, N> RangeToArray(Range &&r) {
  return [&]<std::size_t... Is>(std::index_sequence<Is...>) -> std::array<T, N> {
    auto it = std::ranges::begin(r);
    return {(Is, *it++)...};
  }(std::make_index_sequence<N>{});
}

template <typename T, std::size_t Rows, std::size_t Cols, typename Range>
static constexpr std::array<T, Rows * Cols> Flatten2DArray(
    std::array<std::array<std::complex<float>, Cols>, Rows> &entries) {

  constexpr std::size_t size = Rows * Cols;
  auto r = std::ranges::join_view(entries);

  return [&]<std::size_t... Is>(std::index_sequence<Is...>) -> std::array<T, size> {
    auto it = std::ranges::begin(r);
    return {(Is, *it++)...};
  }(std::make_index_sequence<size>{});
}

} // namespace matrix

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

template <class T, std::size_t Rows, std::size_t Cols>
  requires(Rows != matrix::kDynamicSize && Cols != matrix::kDynamicSize)
struct MatrixDataType {
  std::array<T, Rows * Cols> entries;

  constexpr explicit(false) MatrixDataType(const std::array<T, Rows * Cols> &entries) :
      entries{entries} {}

  constexpr virtual ~MatrixDataType() = default;
  [[nodiscard]] virtual constexpr T *Data() noexcept { return entries.data(); }
  [[nodiscard]] virtual constexpr const T *Data() const noexcept { return entries.data(); }
};

template <class T>
struct MatrixDataType<T, matrix::kDynamicSize, matrix::kDynamicSize> {
  T *entries;

  constexpr virtual ~MatrixDataType() { delete[] entries; }
  [[nodiscard]] virtual constexpr T *Data() noexcept { return entries; }
  [[nodiscard]] virtual constexpr const T *Data() const noexcept { return entries; }
};

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
  const std::size_t num_rows_;
  const std::size_t num_cols_;

  [[nodiscard]]
  constexpr std::size_t GetIndex(const std::size_t row, const std::size_t col) const noexcept {
    return col + NumCols() * row;
  }

public:
  constexpr ~Matrix2D() override = default;

  constexpr Matrix2D(const std::array<std::array<T, Cols>, Rows> &entries) :
      DataType{matrix::RangeToArray<T, Rows * Cols>(std::ranges::join_view(entries))},
      num_rows_{Rows},
      num_cols_{Cols} {}

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
