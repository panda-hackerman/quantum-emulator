/**
 * @file matrix.h
 *
 * @brief Classes for representing matrices.
 * @details Class definition and implementation of a 2D Matrix, including common operations.
 *
 * @author Eli Michaud
 * @since 6/13/2026
 *
 */

#ifndef MATRIX_H
#define MATRIX_H

#include <algorithm>
#include <array>
#include <iomanip>
#include <span>
#include <stdexcept>
#include <vector>

#include "util/print_util.h"
#include "util/template_wizardry.h"

// -- MATRIX CLASS DEFINITIONS --

/// Relating to matrices
namespace matrix {
inline constexpr std::size_t kDynamicSize = 0;

// NOLINTBEGIN(*-identifier-naming)

/// True if the parameter is equal to kDynamicSize
template <std::size_t S>
inline constexpr bool is_dynamic_v = S == kDynamicSize;

/// True if any of the parameters equal to kDynamicSize
template <std::size_t... S>
inline constexpr bool are_dynamic_v = ((S == kDynamicSize) || ...);

/// True if this is a valid matrix size.
template <std::size_t Rows, std::size_t Cols>
inline constexpr bool valid_matrix_v = (is_dynamic_v<Rows> == is_dynamic_v<Cols>);
// NOLINTEND(*-identifier-naming)

} // namespace matrix

/**
 * Interface for a 2D matrix of values.
 * @tparam Type The type of the entries of the matrix.
 */
template <typename Type>
class IMatrix2D {
public:
  using Value_T = Type;

  virtual ~IMatrix2D() = default;

  [[nodiscard]] virtual constexpr std::size_t NumRows() const noexcept = 0;
  [[nodiscard]] virtual constexpr std::size_t NumCols() const noexcept = 0;

  [[nodiscard]] virtual constexpr Type &At(std::size_t row, std::size_t col) = 0;
  [[nodiscard]] virtual constexpr const Type &At(std::size_t row, std::size_t col) const = 0;

  template <typename T>
  friend bool operator==(const IMatrix2D &lhs, const IMatrix2D<T> &rhs) {
    if (lhs.NumRows() != rhs.NumRows() || lhs.NumCols() != rhs.NumCols()) {
      return false;
    }

    for (std::size_t r = 0; r < lhs.NumRows(); ++r) {
      for (std::size_t c = 0; c < lhs.NumCols(); ++c) {
        if (!(lhs.At(r, c) == rhs.At(r, c))) return false;
      }
    }

    return true;
  }

  template <typename T>
  friend bool operator!=(const IMatrix2D &lhs, const IMatrix2D<T> &rhs) {
    return !(lhs == rhs);
  }

  constexpr std::ostream &Print(std::ostream &os) const;
};

/**
 * Matrix data, with a non-dynamic size (e.g., known at compile-time).
 * @tparam Type The type of the entries of the matrix
 * @tparam Rows Number of rows
 * @tparam Cols Number of columns
 * @note Stored as flattened std::array
 */
template <class Type, std::size_t Rows, std::size_t Cols>
struct MatrixDataType {
  std::array<Type, Rows * Cols> entries;

  constexpr explicit(false) MatrixDataType(const std::array<Type, Rows * Cols> &entries) :
      entries{entries} {}

  constexpr virtual ~MatrixDataType() = default;
  [[nodiscard]] virtual constexpr Type *Data() noexcept { return entries.data(); }
  [[nodiscard]] virtual constexpr const Type *Data() const noexcept { return entries.data(); }

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
 * @tparam Type The type of the entries of the matrix
 * @note Stores a pointer, and the data is allocated on the heap. This class manages its own memory.
 */
template <class Type>
struct MatrixDataType<Type, matrix::kDynamicSize, matrix::kDynamicSize> {
  std::vector<Type> entries; // FIXME: Concerning pointer invalidation?
  std::size_t num_rows;
  std::size_t num_cols;

  constexpr MatrixDataType(const std::size_t rows, const std::size_t cols) :
      entries(rows * cols), num_rows{rows}, num_cols{cols} {}

  constexpr virtual ~MatrixDataType() = default;
  [[nodiscard]] virtual constexpr Type *Data() noexcept { return entries.data(); }
  [[nodiscard]] virtual constexpr const Type *Data() const noexcept { return entries.data(); }

  constexpr MatrixDataType(const MatrixDataType &other) :
      entries(other.entries), num_rows{other.num_rows}, num_cols{other.num_cols} {}
  constexpr MatrixDataType(MatrixDataType &&other) noexcept :
      entries(std::move(other.entries)), num_rows{other.num_rows}, num_cols{other.num_cols} {}

  MatrixDataType &operator=(const MatrixDataType &other) {
    if (this != &other) {
      entries = other.entries;
      num_rows = other.num_rows;
      num_cols = other.num_cols;
    }

    return *this;
  }

  MatrixDataType &operator=(MatrixDataType &&other) noexcept {
    if (this != &other) {
      entries = std::move(other.entries);
      num_rows = other.num_rows;
      num_cols = other.num_cols;
    }

    return *this;
  }
};

/**
 * A 2-dimensional matrix.
 * @tparam Type The type of the entries of the matrix
 * @tparam Rows The number of rows
 * @tparam Cols The number of columns
 */
template <typename Type, std::size_t Rows = matrix::kDynamicSize,
          std::size_t Cols = matrix::kDynamicSize>
  requires(matrix::valid_matrix_v<Rows, Cols>)
class Matrix2D : public IMatrix2D<Type>,
                 protected MatrixDataType<Type, Rows, Cols> {

  template <typename T, std::size_t R, std::size_t C>
    requires(matrix::valid_matrix_v<R, C>)
  friend class Matrix2D;

protected:
  /// Helper class to build a dynamic/ non-dynamic matrix.
  template <typename T, std::size_t R, std::size_t C>
  struct Builder {
    std::array<std::array<T, C>, R> data = {};

    constexpr Builder(std::size_t, std::size_t) {}

    constexpr void Put(std::size_t r, std::size_t c, const T &element) { data[r][c] = element; }

    [[nodiscard]] constexpr Matrix2D<T, R, C> Build() { return Matrix2D<T, R, C>(data); }
  };

  /// Helper class to build a dynamic/ non-dynamic matrix.
  template <typename T>
  struct Builder<T, matrix::kDynamicSize, matrix::kDynamicSize> {
    std::vector<std::vector<T>> data;
    const std::size_t num_rows;
    const std::size_t num_cols;

    constexpr Builder(const std::size_t num_rows, const std::size_t num_cols) :
        data(num_rows, std::vector<T>(num_cols)), num_rows{num_rows}, num_cols{num_cols} {}

    constexpr void Put(std::size_t r, std::size_t c, const T &element) { data[r][c] = element; }

    [[nodiscard]] constexpr Matrix2D<T> Build() const {
      Matrix2D<T> matrix_out{num_rows, num_cols};

      for (std::size_t r = 0; r < num_rows; ++r) {
        for (std::size_t c = 0; c < num_cols; ++c) {
          matrix_out.At(r, c) = data[r][c];
        }
      }

      return matrix_out;
    }
  };

  /// Convert 2D coordinates to the flattened index
  [[nodiscard]] constexpr std::size_t GetIndex(const std::size_t row,
                                               const std::size_t col) const noexcept {
    /* On MSVC 17, calling NumCols (or NumRows) in certain constexpr contexts results in an
     * internal compiler error. To fix this we simply don't call it on non-dynamic matrices. */
    const std::size_t num_cols = !kIsDynamic ? Cols : NumCols();
    return col + num_cols * row;
  }

  [[nodiscard]] constexpr Type &AtImpl(const std::size_t row, const std::size_t col) {
    return DataHolderType::Data()[GetIndex(row, col)];
  }

  [[nodiscard]] constexpr const Type &AtImpl(const std::size_t row, const std::size_t col) const {
    return DataHolderType::Data()[GetIndex(row, col)];
  }

public:
  /// If this matrix is "dynamic" or not (a non-dynamic matrix can be constexpr)
  static constexpr bool kIsDynamic = (Rows == matrix::kDynamicSize && Cols == matrix::kDynamicSize);

  /// Child class of MatrixRow. If matrix is not dynamic, we can use a span with fixed size.
  template <typename S>
  using RowSpan_T = std::conditional_t<kIsDynamic, std::span<S>, std::span<S, Cols>>;

  /**
   * A single row of a matrix.
   * @tparam S The type of the elements in the row.
   * This is either the same as the type of elements in the matrix (T), or const T.
   */
  template <typename S>
    requires(std::is_same_v<Type, S> || std::is_same_v<Type, std::remove_const_t<S>>)
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

  using Row_T = MatrixRow<Type>;
  using RowConst_T = MatrixRow<const Type>;
  using DataHolderType = MatrixDataType<Type, Rows, Cols>;

  constexpr ~Matrix2D() override = default;

  explicit(false) Matrix2D(const std::size_t num_rows, const std::size_t num_cols)
    requires(kIsDynamic)
      : DataHolderType{num_rows, num_cols} {}

  constexpr explicit(false) Matrix2D(const std::array<std::array<Type, Cols>, Rows> &entries = {})
    requires(!kIsDynamic)
      : DataHolderType{tmp::Flatten2D(entries)} {}

  [[nodiscard]] constexpr std::size_t NumRows() const noexcept override {
    if constexpr (kIsDynamic)
      return DataHolderType::num_rows;
    else
      return Rows;
  }

  [[nodiscard]] constexpr std::size_t NumCols() const noexcept override {
    if constexpr (kIsDynamic)
      return DataHolderType::num_cols;
    else
      return Cols;
  }

  [[nodiscard]] constexpr Type &At(const std::size_t row, const std::size_t col) override {
    if (row >= NumRows() || col >= NumCols()) XOutOfRange();
    return AtImpl(row, col);
  }

  [[nodiscard]] constexpr const Type &At(const std::size_t row,
                                         const std::size_t col) const override {
    if (row >= NumRows() || col >= NumCols()) XOutOfRange();
    return AtImpl(row, col);
  }

  [[nodiscard]] constexpr Row_T At(const std::size_t row) {
    if (row >= NumRows()) XOutOfRange();
    return Row_T::Create(this, row);
  }

  [[nodiscard]] virtual constexpr RowConst_T At(const std::size_t row) const {
    if (row >= NumRows()) XOutOfRange();
    return RowConst_T::Create(this, row);
  }

  [[nodiscard]] constexpr Row_T operator[](const std::size_t row) {
    return Row_T::Create(this, row);
  }

  [[nodiscard]] constexpr RowConst_T operator[](const std::size_t row) const {
    return RowConst_T::Create(this, row);
  }

  template <typename T, std::size_t R, std::size_t C>
  friend bool operator==(const Matrix2D &lhs, const Matrix2D<T, R, C> &rhs) {
    return static_cast<const IMatrix2D<Type> &>(lhs) == static_cast<const IMatrix2D<T> &>(rhs);
  }

  template <typename T, std::size_t R, std::size_t C>
  friend bool operator!=(const Matrix2D &lhs, const Matrix2D<T, R, C> &rhs) {
    return !(lhs == rhs);
  }

  Matrix2D<Type> WithNewSize(const std::size_t new_rows, const std::size_t new_cols) {
    if (new_cols == NumCols() && new_rows == NumRows()) {
      return *this;
    }

    Matrix2D<Type> out{new_rows, new_cols};

    const std::size_t min_row = std::min(NumRows(), new_rows);
    const std::size_t min_col = std::min(NumCols(), new_cols);

    for (std::size_t r = 0; r < min_row; ++r) {
      for (std::size_t c = 0; c < min_col; ++c) {
        out.At(r, c) = At(r, c);
      }
    }

    return out;
  }

  // MATRIX OPERATIONS
  [[nodiscard]] constexpr Matrix2D<Type, Cols, Rows> Transposed() const noexcept {
    const std::size_t num_rows_og = !kIsDynamic ? Rows : NumRows(); /// # Rows before transpose
    const std::size_t num_cols_og = !kIsDynamic ? Cols : NumCols(); /// # Cols before transpose

    Builder<Type, Cols, Rows> builder{num_cols_og, num_rows_og};

    for (std::size_t r = 0; r < num_rows_og; ++r) {
      for (std::size_t c = 0; c < num_cols_og; ++c) {
        builder.Put(c, r, AtImpl(r, c));
      }
    }

    return builder.Build();
  }

  [[nodiscard]] constexpr Matrix2D Conjugate() const noexcept
    requires(tmp::is_instance_of_v<Type, std::complex>)
  {
    const std::size_t num_rows = !kIsDynamic ? Rows : NumRows();
    const std::size_t num_cols = !kIsDynamic ? Cols : NumCols();

    Builder<Type, Rows, Cols> builder{num_rows, num_cols};

    for (std::size_t r = 0; r < num_rows; ++r) {
      for (std::size_t c = 0; c < num_cols; ++c) {
        builder.Put(r, c, std::conj(AtImpl(r, c)));
      }
    }

    return builder.Build();
  }

  [[nodiscard]] constexpr Matrix2D<Type, Cols, Rows> ConjTranspose() const noexcept
    requires(tmp::is_instance_of_v<Type, std::complex>)
  {
    const std::size_t num_rows_og = !kIsDynamic ? Rows : NumRows(); /// # Rows before transpose
    const std::size_t num_cols_og = !kIsDynamic ? Cols : NumCols(); /// # Cols before transpose

    Builder<Type, Cols, Rows> builder{num_cols_og, num_rows_og};

    for (std::size_t r = 0; r < num_rows_og; ++r) {
      for (std::size_t c = 0; c < num_cols_og; ++c) {
        const std::size_t idx = c + (num_cols_og * r);
        const Type element = std::conj(DataHolderType::Data()[idx]);
        builder.Put(c, r, element);
      }
    }

    return builder.Build();
  }

  /**
   * Calculate the trace of a square matrix
   * @param zero_val The value to treat as zero (if it's different from the default initialization
   * value)
   * @return The sum of the diagonal entries of the matrix
   */
  constexpr Type Trace(Type zero_val = {}) const noexcept(!kIsDynamic)
    requires(Rows == Cols) && requires(Type a, Type b) { a + b; }
  {
    const std::size_t num_rows = !kIsDynamic ? Rows : NumRows(); /// # Rows before transpose
    const std::size_t num_cols = !kIsDynamic ? Cols : NumCols(); /// # Cols before transpose

    if constexpr (kIsDynamic) {
      if (num_rows != num_cols) {
        throw std::invalid_argument("Can only calculate the trace of a square matrix!");
      }
    }

    Type count = zero_val;

    for (std::size_t r = 0; r < num_rows; ++r) {
      const std::size_t idx = r + (num_rows * r);
      count = count + DataHolderType::Data()[idx];
    }

    return count;
  }

  /**
   * Adds this matrix to another matrix
   * @param rhs The right-hand side of the addition
   * @return A new matrix; the sum (this + rhs)
   */
  template <typename OtherType, std::size_t OtherRows, std::size_t OtherCols>
  [[nodiscard]] constexpr auto Plus(const Matrix2D<OtherType, OtherRows, OtherCols> &rhs) const
      noexcept(!matrix::are_dynamic_v<Rows, Cols, OtherRows, OtherCols>) {

    using OtherMatrix_T = Matrix2D<OtherType, OtherRows, OtherCols>;
    constexpr bool other_dynamic = OtherMatrix_T::kIsDynamic;

    // Actual size of the matrices
    const std::size_t num_rows = !kIsDynamic ? Rows : NumRows();
    const std::size_t num_cols = !kIsDynamic ? Cols : NumCols();
    const std::size_t other_num_rows = !other_dynamic ? OtherRows : rhs.NumRows();
    const std::size_t other_num_cols = !other_dynamic ? OtherCols : rhs.NumCols();

    if constexpr (kIsDynamic || other_dynamic) {
      if (num_rows != other_num_rows || num_cols != other_num_cols) {
        throw std::invalid_argument("Only matrices of the same size can be added!");
      }
    }

    // Size of the matrix template params (0 if the matrix should be dynamic)
    constexpr std::size_t result_rows_v = kIsDynamic || other_dynamic ? matrix::kDynamicSize : Rows;
    constexpr std::size_t result_cols_v = kIsDynamic || other_dynamic ? matrix::kDynamicSize : Cols;

    using Result_T = tmp::addition_result_t<Type, OtherType>;
    Builder<Result_T, result_rows_v, result_cols_v> builder{num_rows, num_cols};

    for (std::size_t r = 0; r < num_rows; ++r) {
      for (std::size_t c = 0; c < num_cols; ++c) {
        const Type &a = AtImpl(r, c);
        const OtherType &b = rhs.AtImpl(r, c);
        builder.Put(r, c, a + b);
      }
    }

    return builder.Build();
  }

  /**
   * Subtracts another matrix from this matrix
   * @param rhs The right-hand side of the subtraction
   * @return A new matrix; the difference (this - rhs)
   */
  template <typename OtherType, std::size_t OtherRows, std::size_t OtherCols>
  [[nodiscard]] constexpr auto Minus(const Matrix2D<OtherType, OtherRows, OtherCols> &rhs) const
      noexcept(!matrix::are_dynamic_v<Rows, Cols, OtherRows, OtherCols>) {

    using OtherMatrix_T = Matrix2D<OtherType, OtherRows, OtherCols>;
    constexpr bool other_dynamic = OtherMatrix_T::kIsDynamic;

    const std::size_t num_rows = !kIsDynamic ? Rows : NumRows();
    const std::size_t num_cols = !kIsDynamic ? Cols : NumCols();
    const std::size_t other_num_rows = !other_dynamic ? OtherRows : rhs.NumRows();
    const std::size_t other_num_cols = !other_dynamic ? OtherCols : rhs.NumCols();

    if constexpr (kIsDynamic || other_dynamic) {
      if (num_rows != other_num_rows || num_cols != other_num_cols) {
        throw std::invalid_argument("Only matrices of the same size can be added!");
      }
    }

    constexpr std::size_t result_rows_v = kIsDynamic || other_dynamic ? matrix::kDynamicSize : Rows;
    constexpr std::size_t result_cols_v = kIsDynamic || other_dynamic ? matrix::kDynamicSize : Cols;

    using Result_T = tmp::subtract_result_t<Type, OtherType>;
    Builder<Result_T, result_rows_v, result_cols_v> builder{num_rows, num_cols};

    for (std::size_t r = 0; r < num_rows; ++r) {
      for (std::size_t c = 0; c < num_cols; ++c) {
        const Type &a = AtImpl(r, c);
        const OtherType &b = rhs.AtImpl(r, c);
        builder.Put(r, c, a - b);
      }
    }

    return builder.Build();
  }

  /**
   * Multiplies this matrix by a scalar.
   * @param scalar The scalar
   * @return A new matrix; the result of the multiplication (scalar * this)
   */
  template <typename ScalarType>
  auto ScalarMult(ScalarType scalar) const
      -> Matrix2D<tmp::multiply_result_t<ScalarType, Type>, Rows, Cols> {
    const std::size_t num_rows = !kIsDynamic ? Rows : NumRows();
    const std::size_t num_cols = !kIsDynamic ? Cols : NumCols();

    using Result_T = tmp::multiply_result_t<ScalarType, Type>;
    Builder<Result_T, Rows, Cols> builder{num_rows, num_cols};

    for (std::size_t r = 0; r < num_rows; ++r) {
      for (std::size_t c = 0; c < num_cols; ++c) {
        builder.Put(r, c, scalar * AtImpl(r, c));
      }
    }

    return builder.Build();
  }

  /**
   * Multiply two matrices together
   * @param rhs The right-hand side of the multiplication
   * @return A new matrix; the product (this * rhs)
   */
  template <typename OtherType, std::size_t OtherRows, std::size_t OtherCols>
  constexpr auto Mult(const Matrix2D<OtherType, OtherRows, OtherCols> &rhs) const
      noexcept(!matrix::are_dynamic_v<OtherRows, Cols>)
    requires(OtherRows == Cols || matrix::are_dynamic_v<OtherRows, Cols>)
  {

    constexpr bool other_dynamic = matrix::is_dynamic_v<OtherCols>;

    const std::size_t num_rows = !kIsDynamic ? Rows : NumRows();
    const std::size_t num_cols = !kIsDynamic ? Cols : NumCols();

    const std::size_t other_num_rows = !other_dynamic ? OtherRows : rhs.NumRows();
    const std::size_t other_num_cols = !other_dynamic ? OtherCols : rhs.NumCols();

    if constexpr (matrix::are_dynamic_v<Rows, Cols, OtherCols>) {
      if (num_cols != other_num_rows) {
        throw std::invalid_argument("For matrix multiplication, the number of columns in "
                                    "the left-hand side must be equal to the number of rows in the "
                                    "right-hand side!");
      }
    }

    constexpr bool output_dynamic = kIsDynamic || other_dynamic;
    constexpr std::size_t result_rows_v = output_dynamic ? matrix::kDynamicSize : Rows;
    constexpr std::size_t result_cols_v = output_dynamic ? matrix::kDynamicSize : OtherCols;

    using Result_T = tmp::multiply_result_t<Type, OtherType>;
    Builder<Result_T, result_rows_v, result_cols_v> builder{num_rows, other_num_cols};

    for (std::size_t r = 0; r < num_rows; ++r) {
      for (std::size_t c = 0; c < other_num_cols; ++c) {
        Result_T elem = AtImpl(r, 0) * rhs.AtImpl(0, c);

        for (std::size_t e = 1; e < num_cols; ++e) {
          elem = elem + AtImpl(r, e) * rhs.AtImpl(e, c);
        }

        builder.Put(r, c, elem);
      }
    }

    return builder.Build();
  }

  /**
   * Computes the Kronecker product of two matrices (aka the tensor product)
   */
  template <typename OtherType, std::size_t OtherRows, std::size_t OtherCols>
  constexpr auto Tensor(const Matrix2D<OtherType, OtherRows, OtherCols> &rhs) const {
    constexpr bool other_dynamic = matrix::are_dynamic_v<OtherRows, OtherCols>;

    // Actual matrix sizes
    const std::size_t num_rows = !kIsDynamic ? Rows : NumRows();
    const std::size_t num_cols = !kIsDynamic ? Cols : NumCols();
    const std::size_t other_num_rows = !other_dynamic ? OtherRows : rhs.NumRows();
    const std::size_t other_num_cols = !other_dynamic ? OtherCols : rhs.NumCols();
    const std::size_t result_rows = num_rows * other_num_rows;
    const std::size_t result_cols = num_cols * other_num_cols;

    // Size of the matrix template parameter (0 if the output should be a dynamic matrix)
    constexpr bool output_dynamic = kIsDynamic || other_dynamic;
    constexpr std::size_t rows_type_v = output_dynamic ? matrix::kDynamicSize : result_rows;
    constexpr std::size_t cols_type_v = output_dynamic ? matrix::kDynamicSize : result_cols;

    using Result_T = tmp::multiply_result_t<Type, OtherType>;
    Builder<Result_T, rows_type_v, cols_type_v> builder{result_rows, result_cols};

    // The mega-loop !
    for (std::size_t row_a = 0; row_a < num_rows; ++row_a) {
      for (std::size_t row_b = 0; row_b < other_num_rows; ++row_b) {
        for (std::size_t col_a = 0; col_a < num_cols; ++col_a) {
          for (std::size_t col_b = 0; col_b < other_num_cols; ++col_b) {
            const Type &a_ij = AtImpl(row_a, col_a);
            const OtherType &b_ij = rhs.AtImpl(row_b, col_b);

            const std::size_t row = row_b + (row_a * other_num_rows);
            const std::size_t col = col_b + (col_a * other_num_cols);

            builder.Put(row, col, a_ij * b_ij);
          }
        }
      }
    }

    return builder.Build();
  }

private:
  [[noreturn]] static void XOutOfRange() { throw std::out_of_range("Invalid index for matrix."); }
};

template <typename T>
constexpr std::ostream &IMatrix2D<T>::Print(std::ostream &os) const {
  std::vector<std::string> strings;
  strings.reserve(NumRows() * NumCols());

  std::vector<std::size_t> width_by_col(NumCols(), 0);

  // Calculate strings
  for (int r = 0; r < NumRows(); ++r) {
    for (int c = 0; c < NumCols(); ++c) {
      std::string str = PrettyPrint(At(r, c));

      if (const std::size_t str_size = str.size(); str_size > width_by_col.at(c)) {
        width_by_col.at(c) = str_size;
      }

      strings.emplace_back(std::move(str));
    }
  }

  // Print strings
  for (int r = 0; r < NumRows(); ++r) {
    os << "[ ";
    for (std::size_t c = 0; c < NumCols(); ++c) {
      const std::size_t index = c + NumCols() * r;

      os << std::setw(width_by_col.at(c)) << std::left << strings.at(index) << "\t";
    }
    os << "]\n";
  }

  return os;
}

#endif // MATRIX_H
