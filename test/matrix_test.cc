//
// Created by Eli Michaud on 7/28/2026.
//

#include <gtest/gtest.h>

#include <random>

#include "math/matrix.h"
#include "types.h"

template <typename T, std::size_t Rows, std::size_t Cols>
struct MatrixTestType {
  using Type = T;
  static constexpr std::size_t kNumRows = Rows;
  static constexpr std::size_t kNumCols = Cols;

  // For checking against matrices of different types
  using RandomOtherType = std::conditional_t<!std::is_same_v<T, float>, float, double>;
  static constexpr std::size_t kRandomOtherRows = kNumRows + 1;
  static constexpr std::size_t kRandomOtherCols = kNumCols - 1;

  static constexpr std::string GetName() {
    const std::string row_str = std::to_string(kNumRows);
    const std::string col_str = std::to_string(kNumCols);

    std::string type_name = "Matrix (" + row_str + "x" + col_str + ") of ";

    if constexpr (std::is_same_v<Type, float>)
      return type_name += "float";
    else if constexpr (std::is_same_v<Type, double>)
      return type_name += "double";
    else if constexpr (std::is_same_v<Type, int>)
      return type_name += "int";
    else if constexpr (std::is_same_v<Type, std::complex<float>>)
      return type_name += "std::complex<float>";
    else if constexpr (std::is_same_v<Type, std::complex<double>>)
      return type_name += "std::complex<double>";
    else if constexpr (std::is_same_v<Type, Complex>)
      return type_name += "Complex";

    return type_name += typeid(Type).name();
  }
};

struct MatrixTestTypeNameGenerator {
  template <typename TestType>
  static std::string GetName(int) {
    return TestType::GetName();
  }
};

template <typename TestType>
class BasicMatrixTests : public ::testing::Test {
private:
  static constexpr int kRandomSeed = 67;
  std::mt19937 gen_{kRandomSeed};
  std::normal_distribution<> dis_{10.0, 5.0};

protected:
  using Base_T = TestType::Type;
  static constexpr std::size_t kNumRows = TestType::kNumRows;
  static constexpr std::size_t kNumCols = TestType::kNumCols;

  // Base truth matrices
  Matrix2D<Base_T, kNumRows, kNumCols> matrix_a_static = {};
  Matrix2D<Base_T, kNumRows, kNumCols> matrix_b_static = {};
  Matrix2D<Base_T> matrix_a_dynamic{kNumRows, kNumCols};
  Matrix2D<Base_T> matrix_b_dynamic{kNumRows, kNumCols};

  Matrix2D<Base_T, kNumRows, kNumCols> matrix_zero_static = {};
  Matrix2D<Base_T> matrix_zero_dynamic{kNumRows, kNumCols};

  Base_T GenerateRandomEntry() { return static_cast<Base_T>(dis_(gen_)); }

  void SetUp() override {
    gen_ = std::mt19937{kRandomSeed};
    dis_ = std::normal_distribution{10.0, 5.0};

    auto matrix_a_static_builder = Matrix2D<Base_T, kNumRows, kNumCols>::Builder();
    auto matrix_b_static_builder = Matrix2D<Base_T, kNumRows, kNumCols>::Builder();
    auto matrix_a_dynamic_builder = Matrix2D<Base_T>::Builder(kNumRows, kNumCols);
    auto matrix_b_dynamic_builder = Matrix2D<Base_T>::Builder(kNumRows, kNumCols);

    // Generate matrices
    for (std::size_t row = 0; row < kNumRows; ++row) {
      for (std::size_t col = 0; col < kNumCols; ++col) {
        const Base_T val_a = GenerateRandomEntry();
        const Base_T val_b = GenerateRandomEntry();

        const Base_T val_a_plus_a = val_a + val_a;
        const Base_T val_a_plus_b = val_a + val_b;

        const Base_T val_a_minus_a = val_a - val_a;
        const Base_T val_a_minus_b = val_a - val_b;
        const Base_T val_b_minus_a = val_b - val_a;

        matrix_a_static_builder.Put(row, col, val_a);
        matrix_a_dynamic_builder.Put(row, col, val_a);

        matrix_b_static_builder.Put(row, col, val_b);
        matrix_b_dynamic_builder.Put(row, col, val_b);
      }
    }

    // Ground truth matrices
    matrix_a_static = matrix_a_static_builder.Build();
    matrix_b_static = matrix_b_static_builder.Build();
    matrix_a_dynamic = matrix_a_dynamic_builder.Build();
    matrix_b_dynamic = matrix_b_dynamic_builder.Build();
  }
};

template <typename TestType>
class MatrixBinaryOps : public ::testing::Test {
private:
  static constexpr int kRandomSeed = 67;
  std::mt19937 gen_{kRandomSeed};
  std::normal_distribution<> dis_{10.0, 5.0};

protected:
  using Base_T = TestType::Type;
  static constexpr std::size_t kNumRows = TestType::kNumRows;
  static constexpr std::size_t kNumCols = TestType::kNumCols;

  using RandomOtherType = TestType::RandomOtherType;
  static constexpr std::size_t kRandomOtherRows = TestType::kRandomOtherRows;
  static constexpr std::size_t kRandomOtherCols = TestType::kRandomOtherCols;

  // Base truth matrices
  Matrix2D<Base_T, kNumRows, kNumCols> matrix_a_static = {};
  Matrix2D<Base_T, kNumRows, kNumCols> matrix_b_static = {};
  Matrix2D<Base_T> matrix_a_dynamic{kNumRows, kNumCols};
  Matrix2D<Base_T> matrix_b_dynamic{kNumRows, kNumCols};

  Matrix2D<Base_T, kNumRows, kNumCols> matrix_zero_static = {};
  Matrix2D<Base_T> matrix_zero_dynamic{kNumRows, kNumCols};

  // Equal matrices
  Matrix2D<Base_T, kNumRows, kNumCols> matrix_a_static_equal = {};
  Matrix2D<Base_T, kNumRows, kNumCols> matrix_b_static_equal = {};
  Matrix2D<Base_T> matrix_a_dynamic_equal{kNumRows, kNumCols};
  Matrix2D<Base_T> matrix_b_dynamic_equal{kNumRows, kNumCols};

  // Matrices of different types/size
  Matrix2D<Base_T, kRandomOtherRows, kRandomOtherCols> m_same_type_diff_size_static = {};
  Matrix2D<RandomOtherType, kNumRows, kNumCols> m_diff_type_same_size_static = {};
  Matrix2D<RandomOtherType, kRandomOtherRows, kRandomOtherCols> m_diff_type_diff_size_static = {};

  Matrix2D<Base_T> m_same_type_diff_size_dynamic{kRandomOtherRows, kRandomOtherCols};
  Matrix2D<RandomOtherType> m_diff_type_same_size_dynamic{kNumRows, kNumCols};
  Matrix2D<RandomOtherType> m_diff_type_diff_size_dynamic{kRandomOtherRows, kRandomOtherCols};

  // Addition
  Matrix2D<Base_T, kNumRows, kNumCols> matrix_a_plus_a_static = {};
  Matrix2D<Base_T, kNumRows, kNumCols> matrix_a_plus_b_static = {};

  Matrix2D<Base_T> matrix_a_plus_a_dynamic{kNumRows, kNumCols};
  Matrix2D<Base_T> matrix_a_plus_b_dynamic{kNumRows, kNumCols};

  // Subtraction
  Matrix2D<Base_T, kNumRows, kNumCols> matrix_a_minus_b_static = {};
  Matrix2D<Base_T, kNumRows, kNumCols> matrix_b_minus_a_static = {};

  Matrix2D<Base_T> matrix_a_minus_b_dynamic{kNumRows, kNumCols};
  Matrix2D<Base_T> matrix_b_minus_a_dynamic{kNumRows, kNumCols};

  Base_T GenerateRandomEntry() { return static_cast<Base_T>(dis_(gen_)); }

  void SetUp() override {
    gen_ = std::mt19937{kRandomSeed};
    dis_ = std::normal_distribution{10.0, 5.0};

    auto matrix_a_static_builder = Matrix2D<Base_T, kNumRows, kNumCols>::Builder();
    auto matrix_b_static_builder = Matrix2D<Base_T, kNumRows, kNumCols>::Builder();
    auto matrix_a_dynamic_builder = Matrix2D<Base_T>::Builder(kNumRows, kNumCols);
    auto matrix_b_dynamic_builder = Matrix2D<Base_T>::Builder(kNumRows, kNumCols);

    // Addition
    auto matrix_a_plus_a_static_builder = Matrix2D<Base_T, kNumRows, kNumCols>::Builder();
    auto matrix_a_plus_b_static_builder = Matrix2D<Base_T, kNumRows, kNumCols>::Builder();

    auto matrix_a_plus_a_dynamic_builder = Matrix2D<Base_T>::Builder(kNumRows, kNumCols);
    auto matrix_a_plus_b_dynamic_builder = Matrix2D<Base_T>::Builder(kNumRows, kNumCols);

    // Subtraction
    auto matrix_a_minus_b_static_builder = Matrix2D<Base_T, kNumRows, kNumCols>::Builder();
    auto matrix_b_minus_a_static_builder = Matrix2D<Base_T, kNumRows, kNumCols>::Builder();

    auto matrix_a_minus_b_dynamic_builder = Matrix2D<Base_T>::Builder(kNumRows, kNumCols);
    auto matrix_b_minus_a_dynamic_builder = Matrix2D<Base_T>::Builder(kNumRows, kNumCols);

    // Generate matrices
    for (std::size_t row = 0; row < kNumRows; ++row) {
      for (std::size_t col = 0; col < kNumCols; ++col) {
        const Base_T val_a = GenerateRandomEntry();
        const Base_T val_b = GenerateRandomEntry();

        const Base_T val_a_plus_a = val_a + val_a;
        const Base_T val_a_plus_b = val_a + val_b;

        const Base_T val_a_minus_a = val_a - val_a;
        const Base_T val_a_minus_b = val_a - val_b;
        const Base_T val_b_minus_a = val_b - val_a;

        matrix_a_static_builder.Put(row, col, val_a);
        matrix_a_dynamic_builder.Put(row, col, val_a);

        matrix_b_static_builder.Put(row, col, val_b);
        matrix_b_dynamic_builder.Put(row, col, val_b);

        // A + A
        matrix_a_plus_a_static_builder.Put(row, col, val_a_plus_a);
        matrix_a_plus_a_dynamic_builder.Put(row, col, val_a_plus_a);

        // A + B
        matrix_a_plus_b_static_builder.Put(row, col, val_a_plus_b);
        matrix_a_plus_b_dynamic_builder.Put(row, col, val_a_plus_b);

        // A - B
        matrix_a_minus_b_static_builder.Put(row, col, val_a_minus_b);
        matrix_a_minus_b_dynamic_builder.Put(row, col, val_a_minus_b);

        // B - A
        matrix_b_minus_a_static_builder.Put(row, col, val_b_minus_a);
        matrix_b_minus_a_dynamic_builder.Put(row, col, val_a_minus_b);
      }
    }

    // Ground truth matrices
    matrix_a_static = matrix_a_static_builder.Build();
    matrix_b_static = matrix_b_static_builder.Build();
    matrix_a_dynamic = matrix_a_dynamic_builder.Build();
    matrix_b_dynamic = matrix_b_dynamic_builder.Build();

    // Equal matrices
    matrix_a_static_equal = matrix_a_static_builder.Build();
    matrix_b_static_equal = matrix_b_static_builder.Build();
    matrix_a_dynamic_equal = matrix_a_dynamic_builder.Build();
    matrix_b_dynamic_equal = matrix_b_dynamic_builder.Build();

    // Addition
    matrix_a_plus_a_static = matrix_a_plus_a_static_builder.Build();
    matrix_a_plus_b_static = matrix_a_plus_b_static_builder.Build();
    matrix_a_plus_a_dynamic = matrix_a_plus_a_dynamic_builder.Build();
    matrix_a_plus_b_dynamic = matrix_a_plus_b_dynamic_builder.Build();

    // Subtraction
    matrix_a_minus_b_static = matrix_a_minus_b_static_builder.Build();
    matrix_b_minus_a_static = matrix_b_minus_a_static_builder.Build();
    matrix_a_minus_b_dynamic = matrix_a_minus_b_dynamic_builder.Build();
    matrix_b_minus_a_dynamic = matrix_b_minus_a_dynamic_builder.Build();

    // -- Different type/ size -- //
    std::mt19937 gen_other{kRandomSeed};
    std::normal_distribution<> dis_other{10.0, 5.0};

    auto m_same_type_diff_size_static_builder =
        Matrix2D<Base_T, kRandomOtherRows, kRandomOtherCols>::Builder();
    auto m_diff_type_same_size_static_builder =
        Matrix2D<RandomOtherType, kNumRows, kNumCols>::Builder();
    auto m_diff_type_diff_size_static_builder =
        Matrix2D<RandomOtherType, kRandomOtherRows, kRandomOtherCols>::Builder();

    auto m_same_type_diff_size_dynamic_builder =
        Matrix2D<Base_T>::Builder(kRandomOtherRows, kRandomOtherCols);
    auto m_diff_type_same_size_dynamic_builder =
        Matrix2D<RandomOtherType>::Builder(kNumRows, kNumCols);
    auto m_diff_type_diff_size_dynamic_builder =
        Matrix2D<RandomOtherType>::Builder(kRandomOtherRows, kRandomOtherCols);

    for (std::size_t row = 0; row < kNumRows; ++row) {
      for (std::size_t col = 0; col < kNumCols; ++col) {
        auto entry = static_cast<RandomOtherType>(dis_other(gen_other));

        m_diff_type_same_size_static_builder.Put(row, col, entry);
        m_diff_type_same_size_dynamic_builder.Put(row, col, entry);
      }
    }

    for (std::size_t row = 0; row < kRandomOtherRows; ++row) {
      for (std::size_t col = 0; col < kRandomOtherCols; ++col) {
        auto entry_same = GenerateRandomEntry();
        auto entry_other = static_cast<RandomOtherType>(dis_other(gen_other));

        m_same_type_diff_size_static_builder.Put(row, col, entry_same);
        m_same_type_diff_size_dynamic_builder.Put(row, col, entry_same);

        m_diff_type_diff_size_static_builder.Put(row, col, entry_other);
        m_diff_type_diff_size_dynamic_builder.Put(row, col, entry_other);
      }
    }
  }
};

using MatrixTestTypes =
    ::testing::Types<MatrixTestType<float, 2, 3>, MatrixTestType<double, 2, 3>,
                     MatrixTestType<int, 2, 3>, MatrixTestType<std::complex<float>, 2, 3>,
                     MatrixTestType<std::complex<double>, 2, 3>, MatrixTestType<Complex, 2, 3>,
                     MatrixTestType<int, 5, 5>, MatrixTestType<std::complex<float>, 5, 5>>;

TYPED_TEST_SUITE(BasicMatrixTests, MatrixTestTypes, MatrixTestTypeNameGenerator);
TYPED_TEST_SUITE(MatrixBinaryOps, MatrixTestTypes, MatrixTestTypeNameGenerator);

/* Builder works */

TYPED_TEST(BasicMatrixTests, BuilderWorks) {
  using Base = TestFixture::Base_T;
  constexpr std::size_t num_rows = this->kNumRows;
  constexpr std::size_t num_cols = this->kNumCols;

  using StaticMatrixType = Matrix2D<Base, num_rows, num_cols>;
  using DynamicMatrixType = Matrix2D<Base>;

  auto static_builder = StaticMatrixType::Builder();
  auto dynamic_builder = DynamicMatrixType::Builder(num_rows, num_cols);

  std::array<std::array<Base, num_cols>, num_rows> validation_array{};

  // Build
  for (std::size_t row = 0; row < num_rows; ++row) {
    for (std::size_t col = 0; col < num_cols; ++col) {
      auto entry = this->GenerateRandomEntry();

      validation_array[row][col] = entry;
      static_builder.Put(row, col, entry);
      dynamic_builder.Put(row, col, entry);
    }
  }

  StaticMatrixType static_matrix = static_builder.Build();
  DynamicMatrixType dynamic_matrix = dynamic_builder.Build();

  // Validate
  for (std::size_t row = 0; row < num_rows; ++row) {
    for (std::size_t col = 0; col < num_cols; ++col) {
      auto entry = validation_array[row][col];

      EXPECT_EQ(validation_array[row][col], static_matrix.At(row, col));
      EXPECT_EQ(validation_array[row][col], dynamic_matrix.At(row, col));
    }
  }
}

/* Check that matrix starts at correct size */

TYPED_TEST(BasicMatrixTests, IsInitAtCorrectSize) {
  auto &matrix_a_s = this->matrix_a_static;
  auto &matrix_b_s = this->matrix_b_static;
  auto &matrix_a_d = this->matrix_a_dynamic;
  auto &matrix_b_d = this->matrix_b_dynamic;

  EXPECT_EQ(matrix_a_s.NumRows(), this->kNumRows);
  EXPECT_EQ(matrix_b_s.NumRows(), this->kNumRows);
  EXPECT_EQ(matrix_a_d.NumRows(), this->kNumRows);
  EXPECT_EQ(matrix_b_d.NumRows(), this->kNumRows);

  EXPECT_EQ(matrix_a_s.NumCols(), this->kNumCols);
  EXPECT_EQ(matrix_b_s.NumCols(), this->kNumCols);
  EXPECT_EQ(matrix_a_d.NumCols(), this->kNumCols);
  EXPECT_EQ(matrix_b_d.NumCols(), this->kNumCols);
}

/* We can add elements without throwing an error */

TYPED_TEST(BasicMatrixTests, AddElements_NoThrow) {
  const std::size_t num_rows = this->kNumRows;
  const std::size_t num_cols = this->kNumCols;

  auto &matrix = this->matrix_a_dynamic;

  for (std::size_t row = 0; row < num_rows; ++row) {
    for (std::size_t col = 0; col < num_cols; ++col) {
      EXPECT_NO_THROW(matrix.At(row, col) = this->GenerateRandomEntry());
    }
  }
}

/* We can retrieve elements once they're added, and get the correct values back */

TYPED_TEST(BasicMatrixTests, AddAndRetrieve_CorrectValues) {
  auto &matrix_s = this->matrix_a_static;
  auto &matrix_d = this->matrix_a_dynamic;

  for (std::size_t row = 0; row < this->kNumRows; ++row) {
    for (std::size_t col = 0; col < this->kNumCols; ++col) {
      auto entry = this->GenerateRandomEntry();

      // Set
      matrix_s.At(row, col) = entry;
      matrix_d.At(row, col) = entry;

      // Check
      EXPECT_EQ(matrix_s.At(row, col), entry);
      EXPECT_EQ(matrix_d.At(row, col), entry);
    }
  }
}

/* We can't retrieve elements that are out of bounds */

TYPED_TEST(BasicMatrixTests, RetrieveOutOfBounds_Throws) {
  auto &matrix_s = this->matrix_a_static;
  auto &matrix_d = this->matrix_a_dynamic;

  const std::size_t num_rows = this->kNumRows;
  const std::size_t num_cols = this->kNumCols;

  EXPECT_ANY_THROW((void)matrix_s.At(num_rows - 1, num_cols));
  EXPECT_ANY_THROW((void)matrix_s.At(num_rows, num_cols - 1));
  EXPECT_ANY_THROW((void)matrix_s.At(num_rows, num_cols));

  EXPECT_ANY_THROW((void)matrix_d.At(num_rows - 1, num_cols));
  EXPECT_ANY_THROW((void)matrix_d.At(num_rows, num_cols - 1));
  EXPECT_ANY_THROW((void)matrix_d.At(num_rows, num_cols));
}

/* Equality operators don't throw; operators work for matrices of the same and different sizes */

TYPED_TEST(MatrixBinaryOps, EqualityOps_NoThrow) {
  // Static == Static
  EXPECT_NO_THROW((void)(this->matrix_a_static == this->matrix_a_static)); // A == A
  EXPECT_NO_THROW((void)(this->matrix_a_static == this->matrix_b_static)); // A == B
  EXPECT_NO_THROW((void)(this->matrix_b_static == this->matrix_a_static)); // B == A
  EXPECT_NO_THROW((void)(this->matrix_b_static == this->matrix_b_static)); // B == B

  EXPECT_NO_THROW((void)(this->matrix_a_static != this->matrix_a_static)); // A != A
  EXPECT_NO_THROW((void)(this->matrix_a_static != this->matrix_b_static)); // A != B
  EXPECT_NO_THROW((void)(this->matrix_b_static != this->matrix_a_static)); // B != A
  EXPECT_NO_THROW((void)(this->matrix_b_static != this->matrix_b_static)); // B != B

  // Dynamic == Dynamic
  EXPECT_NO_THROW((void)(this->matrix_a_dynamic == this->matrix_a_dynamic)); // A == A
  EXPECT_NO_THROW((void)(this->matrix_a_dynamic == this->matrix_b_dynamic)); // A == B
  EXPECT_NO_THROW((void)(this->matrix_b_dynamic == this->matrix_a_dynamic)); // B == A
  EXPECT_NO_THROW((void)(this->matrix_b_dynamic == this->matrix_b_dynamic)); // B == B

  EXPECT_NO_THROW((void)(this->matrix_a_dynamic != this->matrix_a_dynamic)); // A != A
  EXPECT_NO_THROW((void)(this->matrix_a_dynamic != this->matrix_b_dynamic)); // A != B
  EXPECT_NO_THROW((void)(this->matrix_b_dynamic != this->matrix_a_dynamic)); // B != A
  EXPECT_NO_THROW((void)(this->matrix_b_dynamic != this->matrix_b_dynamic)); // B != B

  // Static == Dynamic
  EXPECT_NO_THROW((void)(this->matrix_a_static == this->matrix_a_dynamic)); // A == A
  EXPECT_NO_THROW((void)(this->matrix_a_static == this->matrix_b_dynamic)); // A == B
  EXPECT_NO_THROW((void)(this->matrix_b_static == this->matrix_a_dynamic)); // B == A
  EXPECT_NO_THROW((void)(this->matrix_b_static == this->matrix_b_dynamic)); // B == B

  EXPECT_NO_THROW((void)(this->matrix_a_static != this->matrix_a_dynamic)); // A != A
  EXPECT_NO_THROW((void)(this->matrix_a_static != this->matrix_b_dynamic)); // A != B
  EXPECT_NO_THROW((void)(this->matrix_b_static != this->matrix_a_dynamic)); // B != A
  EXPECT_NO_THROW((void)(this->matrix_b_static != this->matrix_b_dynamic)); // B != B

  // Dynamic == Static
  EXPECT_NO_THROW((void)(this->matrix_a_dynamic == this->matrix_a_static)); // A == A
  EXPECT_NO_THROW((void)(this->matrix_a_dynamic == this->matrix_b_static)); // A == B
  EXPECT_NO_THROW((void)(this->matrix_b_dynamic == this->matrix_a_static)); // B == A
  EXPECT_NO_THROW((void)(this->matrix_b_dynamic == this->matrix_b_static)); // B == B

  EXPECT_NO_THROW((void)(this->matrix_a_dynamic != this->matrix_a_static)); // A != A
  EXPECT_NO_THROW((void)(this->matrix_a_dynamic != this->matrix_b_static)); // A != B
  EXPECT_NO_THROW((void)(this->matrix_b_dynamic != this->matrix_a_static)); // B != A
  EXPECT_NO_THROW((void)(this->matrix_b_dynamic != this->matrix_b_static)); // B != B
}

TYPED_TEST(MatrixBinaryOps, EqualityOps_OtherSize_NoThrow) {

  // Static/ Static
  EXPECT_NO_THROW((void)(this->matrix_a_static == this->m_same_type_diff_size_static));
  EXPECT_NO_THROW((void)(this->m_same_type_diff_size_static == this->matrix_a_static));

  EXPECT_NO_THROW((void)(this->matrix_a_static != this->m_same_type_diff_size_static));
  EXPECT_NO_THROW((void)(this->m_same_type_diff_size_static != this->matrix_a_static));

  // Dynamic/ Dynamic
  EXPECT_NO_THROW((void)(this->matrix_a_dynamic == this->m_same_type_diff_size_dynamic));
  EXPECT_NO_THROW((void)(this->m_same_type_diff_size_dynamic == this->matrix_a_dynamic));

  EXPECT_NO_THROW((void)(this->matrix_a_dynamic != this->m_same_type_diff_size_dynamic));
  EXPECT_NO_THROW((void)(this->m_same_type_diff_size_dynamic != this->matrix_a_dynamic));

  // Static/ Dynamic
  EXPECT_NO_THROW((void)(this->matrix_a_static == this->m_same_type_diff_size_dynamic));
  EXPECT_NO_THROW((void)(this->m_same_type_diff_size_static == this->matrix_a_dynamic));

  EXPECT_NO_THROW((void)(this->matrix_a_static != this->m_same_type_diff_size_dynamic));
  EXPECT_NO_THROW((void)(this->m_same_type_diff_size_static != this->matrix_a_dynamic));

  // Dynamic/ Static
  EXPECT_NO_THROW((void)(this->matrix_a_dynamic == this->m_same_type_diff_size_static));
  EXPECT_NO_THROW((void)(this->m_same_type_diff_size_dynamic == this->matrix_a_static));

  EXPECT_NO_THROW((void)(this->matrix_a_dynamic != this->m_same_type_diff_size_static));
  EXPECT_NO_THROW((void)(this->m_same_type_diff_size_dynamic != this->matrix_a_static));
}

/* Equality operators give the correct output */

TYPED_TEST(MatrixBinaryOps, EqualityOps_CorrectValues) {
  /* Static == Static */ {
    EXPECT_EQ(this->matrix_a_static, this->matrix_a_static);          // A == A
    EXPECT_EQ(this->matrix_a_static, this->matrix_a_static_equal);    // A == A~
    EXPECT_EQ(this->matrix_a_static_equal, this->matrix_a_static);    // A~ == A

    EXPECT_EQ(this->matrix_b_static, this->matrix_b_static);          // B == B
    EXPECT_EQ(this->matrix_b_static, this->matrix_b_static_equal);    // B == B~
    EXPECT_EQ(this->matrix_b_static_equal, this->matrix_b_static);    // B~ == B

    EXPECT_NE(this->matrix_a_static, this->matrix_b_static);          // A != B
    EXPECT_NE(this->matrix_b_static, this->matrix_a_static);          // B != A
  }

  /* Dynamic == Dynamic */ {
    EXPECT_EQ(this->matrix_a_dynamic, this->matrix_a_dynamic);        // A == A
    EXPECT_EQ(this->matrix_a_dynamic, this->matrix_a_dynamic_equal);  // A == A~
    EXPECT_EQ(this->matrix_a_dynamic_equal, this->matrix_a_dynamic);  // A~ == A

    EXPECT_EQ(this->matrix_b_dynamic, this->matrix_b_dynamic);        // B == B
    EXPECT_EQ(this->matrix_b_dynamic, this->matrix_b_dynamic_equal);  // B == B~
    EXPECT_EQ(this->matrix_b_dynamic_equal, this->matrix_b_dynamic);  // B~ == B

    EXPECT_NE(this->matrix_a_dynamic, this->matrix_b_dynamic);        // A != B
    EXPECT_NE(this->matrix_b_dynamic, this->matrix_a_dynamic);        // B != A
  }

  /* Static == Dynamic */ {
    EXPECT_EQ(this->matrix_a_static, this->matrix_a_dynamic);         // A == A
    EXPECT_EQ(this->matrix_a_static, this->matrix_a_dynamic_equal);   // A == A~
    EXPECT_EQ(this->matrix_a_static_equal, this->matrix_a_dynamic);   // A~ == A

    EXPECT_EQ(this->matrix_b_static, this->matrix_b_dynamic);         // B == B
    EXPECT_EQ(this->matrix_b_static, this->matrix_b_dynamic_equal);   // B == B~
    EXPECT_EQ(this->matrix_b_static_equal, this->matrix_b_dynamic);   // B~ == B

    EXPECT_NE(this->matrix_a_static, this->matrix_b_dynamic);         // A != B
    EXPECT_NE(this->matrix_b_static, this->matrix_a_dynamic);         // B != A
  }

  /* Dynamic == Static */ {
    EXPECT_EQ(this->matrix_a_dynamic, this->matrix_a_static); // A == A
    EXPECT_EQ(this->matrix_a_dynamic, this->matrix_a_static_equal); // A == A~
    EXPECT_EQ(this->matrix_a_dynamic_equal, this->matrix_a_static); // A~ == A

    EXPECT_EQ(this->matrix_b_dynamic, this->matrix_b_static); // B == B
    EXPECT_EQ(this->matrix_b_dynamic, this->matrix_b_static_equal); // B == B~
    EXPECT_EQ(this->matrix_b_dynamic_equal, this->matrix_b_static); // B~ == B

    EXPECT_NE(this->matrix_a_dynamic, this->matrix_b_static); // A != B
    EXPECT_NE(this->matrix_b_dynamic, this->matrix_a_static); // B != A
  }
}

TYPED_TEST(MatrixBinaryOps, EqualityOps_OtherSize_CorrectValues) {
  /* Static == Static */
  EXPECT_NE(this->matrix_a_static, this->m_same_type_diff_size_static);
  EXPECT_NE(this->m_same_type_diff_size_static, this->matrix_a_static);

  /* Dynamic == Dynamic */
  EXPECT_NE(this->matrix_a_dynamic, this->m_same_type_diff_size_dynamic);
  EXPECT_NE(this->m_same_type_diff_size_dynamic, this->matrix_a_dynamic);

  /* Static == Dynamic */
  EXPECT_NE(this->matrix_a_static, this->m_same_type_diff_size_dynamic);
  EXPECT_NE(this->m_same_type_diff_size_static, this->matrix_a_dynamic);

  /* Dynamic == Static */
  EXPECT_NE(this->matrix_a_dynamic, this->m_same_type_diff_size_static);
  EXPECT_NE(this->m_same_type_diff_size_dynamic, this->matrix_a_static);
}

/* Addition and subtraction operations don't throw */

TYPED_TEST(MatrixBinaryOps, MatrixAddition_NoThrow) {

  // Static + Static
  EXPECT_NO_THROW((void)(this->matrix_a_static.Plus(this->matrix_a_static)));
  EXPECT_NO_THROW((void)(this->matrix_a_static.Plus(this->matrix_b_static)));
  EXPECT_NO_THROW((void)(this->matrix_b_static.Plus(this->matrix_a_static)));

  // Dynamic + Dynamic
  EXPECT_NO_THROW((void)(this->matrix_a_dynamic.Plus(this->matrix_a_dynamic)));
  EXPECT_NO_THROW((void)(this->matrix_a_dynamic.Plus(this->matrix_b_dynamic)));
  EXPECT_NO_THROW((void)(this->matrix_b_dynamic.Plus(this->matrix_a_dynamic)));

  // Static + Dynamic
  EXPECT_NO_THROW((void)(this->matrix_a_static.Plus(this->matrix_a_dynamic)));
  EXPECT_NO_THROW((void)(this->matrix_a_static.Plus(this->matrix_b_dynamic)));
  EXPECT_NO_THROW((void)(this->matrix_b_static.Plus(this->matrix_a_dynamic)));

  // Dynamic + Static
  EXPECT_NO_THROW((void)(this->matrix_a_dynamic.Plus(this->matrix_a_static)));
  EXPECT_NO_THROW((void)(this->matrix_a_dynamic.Plus(this->matrix_b_static)));
  EXPECT_NO_THROW((void)(this->matrix_b_dynamic.Plus(this->matrix_a_static)));
}

TYPED_TEST(MatrixBinaryOps, MatrixSubtraction_NoThrow) {

  // Static + Static
  EXPECT_NO_THROW((void)(this->matrix_a_static.Minus(this->matrix_a_static)));
  EXPECT_NO_THROW((void)(this->matrix_a_static.Minus(this->matrix_b_static)));
  EXPECT_NO_THROW((void)(this->matrix_b_static.Minus(this->matrix_a_static)));

  // Dynamic + Dynamic
  EXPECT_NO_THROW((void)(this->matrix_a_dynamic.Minus(this->matrix_a_dynamic)));
  EXPECT_NO_THROW((void)(this->matrix_a_dynamic.Minus(this->matrix_b_dynamic)));
  EXPECT_NO_THROW((void)(this->matrix_b_dynamic.Minus(this->matrix_a_dynamic)));

  // Static + Dynamic
  EXPECT_NO_THROW((void)(this->matrix_a_static.Minus(this->matrix_a_dynamic)));
  EXPECT_NO_THROW((void)(this->matrix_a_static.Minus(this->matrix_b_dynamic)));
  EXPECT_NO_THROW((void)(this->matrix_b_static.Minus(this->matrix_a_dynamic)));

  // Dynamic + Static
  EXPECT_NO_THROW((void)(this->matrix_a_dynamic.Minus(this->matrix_a_static)));
  EXPECT_NO_THROW((void)(this->matrix_a_dynamic.Minus(this->matrix_b_static)));
  EXPECT_NO_THROW((void)(this->matrix_b_dynamic.Minus(this->matrix_a_static)));
}

/* Addition and subtraction operations fail with incompatable sizes */

TYPED_TEST(MatrixBinaryOps, MatrixAddition_AndSubtraction_WrongSize_Throws) {
  // Dynamic +- Dynamic
  EXPECT_ANY_THROW((void)(this->matrix_a_dynamic.Plus(this->m_same_type_diff_size_dynamic)));
  EXPECT_ANY_THROW((void)(this->m_same_type_diff_size_dynamic.Plus(this->matrix_a_dynamic)));

  EXPECT_ANY_THROW((void)(this->matrix_a_dynamic.Minus(this->m_same_type_diff_size_dynamic)));
  EXPECT_ANY_THROW((void)(this->m_same_type_diff_size_dynamic.Minus(this->matrix_a_dynamic)));

  // Static +- Dynamic
  EXPECT_ANY_THROW((void)(this->matrix_a_static.Plus(this->m_same_type_diff_size_dynamic)));
  EXPECT_ANY_THROW((void)(this->m_same_type_diff_size_static.Plus(this->matrix_a_dynamic)));

  EXPECT_ANY_THROW((void)(this->matrix_a_static.Minus(this->m_same_type_diff_size_dynamic)));
  EXPECT_ANY_THROW((void)(this->m_same_type_diff_size_static.Minus(this->matrix_a_dynamic)));

  // Dynamic +- Static
  EXPECT_ANY_THROW((void)(this->matrix_a_dynamic.Plus(this->m_same_type_diff_size_static)));
  EXPECT_ANY_THROW((void)(this->m_same_type_diff_size_dynamic.Plus(this->matrix_a_static)));

  EXPECT_ANY_THROW((void)(this->matrix_a_dynamic.Minus(this->m_same_type_diff_size_static)));
  EXPECT_ANY_THROW((void)(this->m_same_type_diff_size_dynamic.Minus(this->matrix_a_static)));
}

/* Addition and subtraction operations give the correct values */

TYPED_TEST(MatrixBinaryOps, MatrixAddition_CorrectValues) {

  /* Static + Static */ {
    auto a_plus_a_ss = this->matrix_a_static.Plus(this->matrix_a_static);
    auto a_plus_b_ss = this->matrix_a_static.Plus(this->matrix_b_static);
    auto b_plus_a_ss = this->matrix_b_static.Plus(this->matrix_a_static);

    EXPECT_EQ(a_plus_a_ss, this->matrix_a_plus_a_static);
    EXPECT_EQ(a_plus_b_ss, this->matrix_a_plus_b_static);
    EXPECT_EQ(b_plus_a_ss, this->matrix_a_plus_b_static);

    EXPECT_EQ(a_plus_a_ss, this->matrix_a_plus_a_dynamic);
    EXPECT_EQ(a_plus_b_ss, this->matrix_a_plus_b_dynamic);
    EXPECT_EQ(b_plus_a_ss, this->matrix_a_plus_b_dynamic);
  }

  /* Dynamic + Dynamic */ {
    auto a_plus_a_dd = this->matrix_a_dynamic.Plus(this->matrix_a_dynamic);
    auto a_plus_b_dd = this->matrix_a_dynamic.Plus(this->matrix_b_dynamic);
    auto b_plus_a_dd = this->matrix_b_dynamic.Plus(this->matrix_a_dynamic);

    EXPECT_EQ(a_plus_a_dd, this->matrix_a_plus_a_static);
    EXPECT_EQ(a_plus_b_dd, this->matrix_a_plus_b_static);
    EXPECT_EQ(b_plus_a_dd, this->matrix_a_plus_b_static);

    EXPECT_EQ(a_plus_a_dd, this->matrix_a_plus_a_dynamic);
    EXPECT_EQ(a_plus_b_dd, this->matrix_a_plus_b_dynamic);
    EXPECT_EQ(b_plus_a_dd, this->matrix_a_plus_b_dynamic);
  }

  /* Static + Dynamic */ {
    auto a_plus_a_sd = this->matrix_a_static.Plus(this->matrix_a_dynamic);
    auto a_plus_b_sd = this->matrix_a_static.Plus(this->matrix_b_dynamic);
    auto b_plus_a_sd = this->matrix_b_static.Plus(this->matrix_a_dynamic);

    EXPECT_EQ(a_plus_a_sd, this->matrix_a_plus_a_static);
    EXPECT_EQ(a_plus_b_sd, this->matrix_a_plus_b_static);
    EXPECT_EQ(b_plus_a_sd, this->matrix_a_plus_b_static);

    EXPECT_EQ(a_plus_a_sd, this->matrix_a_plus_a_dynamic);
    EXPECT_EQ(a_plus_b_sd, this->matrix_a_plus_b_dynamic);
    EXPECT_EQ(b_plus_a_sd, this->matrix_a_plus_b_dynamic);
  }

  /* Dynamic + Static */ {
    auto a_plus_a_ds = this->matrix_a_dynamic.Plus(this->matrix_a_static);
    auto a_plus_b_ds = this->matrix_a_dynamic.Plus(this->matrix_b_static);
    auto b_plus_a_ds = this->matrix_b_dynamic.Plus(this->matrix_a_static);

    EXPECT_EQ(a_plus_a_ds, this->matrix_a_plus_a_static);
    EXPECT_EQ(a_plus_b_ds, this->matrix_a_plus_b_static);
    EXPECT_EQ(b_plus_a_ds, this->matrix_a_plus_b_static);

    EXPECT_EQ(a_plus_a_ds, this->matrix_a_plus_a_dynamic);
    EXPECT_EQ(a_plus_b_ds, this->matrix_a_plus_b_dynamic);
    EXPECT_EQ(b_plus_a_ds, this->matrix_a_plus_b_dynamic);
  }
}

TYPED_TEST(MatrixBinaryOps, MatrixSubtraction_CorrectValues) {

  /* Static - Static */ {
    auto a_minus_a_ss = this->matrix_a_static.Minus(this->matrix_a_static);
    auto a_minus_b_ss = this->matrix_a_static.Minus(this->matrix_b_static);

    EXPECT_EQ(a_minus_a_ss, this->matrix_zero_dynamic);
    EXPECT_EQ(a_minus_a_ss, this->matrix_zero_static);

    EXPECT_EQ(a_minus_b_ss, this->matrix_a_minus_b_static);
    EXPECT_EQ(a_minus_b_ss, this->matrix_a_minus_b_dynamic);
  }

  /* Dynamic - Dynamic */ {
    auto a_minus_a_dd = this->matrix_a_dynamic.Minus(this->matrix_a_dynamic);
    auto a_minus_b_dd = this->matrix_a_dynamic.Minus(this->matrix_b_dynamic);

    EXPECT_EQ(a_minus_a_dd, this->matrix_zero_dynamic);
    EXPECT_EQ(a_minus_a_dd, this->matrix_zero_static);

    EXPECT_EQ(a_minus_b_dd, this->matrix_a_minus_b_static);
    EXPECT_EQ(a_minus_b_dd, this->matrix_a_minus_b_dynamic);
  }

  /* Static - Dynamic */ {
    auto a_minus_a_sd = this->matrix_a_static.Minus(this->matrix_a_dynamic);
    auto a_minus_b_sd = this->matrix_a_static.Minus(this->matrix_b_dynamic);

    EXPECT_EQ(a_minus_a_sd, this->matrix_zero_dynamic);
    EXPECT_EQ(a_minus_a_sd, this->matrix_zero_static);

    EXPECT_EQ(a_minus_b_sd, this->matrix_a_minus_b_static);
    EXPECT_EQ(a_minus_b_sd, this->matrix_a_minus_b_dynamic);
  }

  /* Dynamic - Static */ {
    auto a_minus_a_ds = this->matrix_a_dynamic.Minus(this->matrix_a_static);
    auto a_minus_b_ds = this->matrix_a_dynamic.Minus(this->matrix_b_static);

    EXPECT_EQ(a_minus_a_ds, this->matrix_zero_dynamic);
    EXPECT_EQ(a_minus_a_ds, this->matrix_zero_static);

    EXPECT_EQ(a_minus_b_ds, this->matrix_a_minus_b_static);
    EXPECT_EQ(a_minus_b_ds, this->matrix_a_minus_b_dynamic);
  }
}