//
// Created by Eli Michaud on 6/27/2026.
//

#include <gtest/gtest.h>

#include "math/matrix.h"
#include "types.h"

TEST(MatrixTests, IsInitAtCorrectSize_Static) {
  constexpr Matrix2D<Complex, 2, 3> matrix = {{{
      {1, 2, 3},
      {4, 5, 6},
  }}};

  EXPECT_EQ(matrix.NumRows(), 2);
  EXPECT_EQ(matrix.NumCols(), 3);
}

TEST(MatrixTests, IsInitAtCorrectSize_Dynamic) {
  const Matrix2D<std::complex<float>> matrix{2, 3};

  EXPECT_EQ(matrix.NumRows(), 2);
  EXPECT_EQ(matrix.NumCols(), 3);
}

TEST(MatrixTests, CanAddElements_Dynamic) {
  Matrix2D<std::complex<float>> matrix{2, 2};
  EXPECT_NO_THROW(matrix.At(0, 0) = std::complex<float>{1});
  EXPECT_NO_THROW(matrix.At(0, 1) = std::complex<float>{2});
  EXPECT_NO_THROW(matrix.At(1, 0) = std::complex<float>{3});
  EXPECT_NO_THROW(matrix.At(1, 1) = std::complex<float>{4});
}

TEST(MatrixTests, CanRetrieveElements_Static) {
  constexpr Matrix2D<std::complex<float>, 2, 3> matrix = {{{
      {1, 2, 3},
      {4, 5, 6},
  }}};

  constexpr auto a = matrix.At(0, 0);
  constexpr auto b = matrix.At(0, 1);
  constexpr auto c = matrix.At(0, 2);
  constexpr auto d = matrix.At(1, 0);
  constexpr auto e = matrix.At(1, 1);
  constexpr auto f = matrix.At(1, 2);

  EXPECT_EQ(a, std::complex<float>{1});
  EXPECT_EQ(b, std::complex<float>{2});
  EXPECT_EQ(c, std::complex<float>{3});
  EXPECT_EQ(d, std::complex<float>{4});
  EXPECT_EQ(e, std::complex<float>{5});
  EXPECT_EQ(f, std::complex<float>{6});
}

TEST(MatrixTests, CanRetrieveElements_Dynamic) {
  Matrix2D<std::complex<float>> matrix{2, 2};
  matrix.At(0, 0) = std::complex<float>{1};
  matrix.At(0, 1) = std::complex<float>{2};
  matrix.At(1, 0) = std::complex<float>{3};
  matrix.At(1, 1) = std::complex<float>{4};

  const auto a = matrix.At(0, 0);
  const auto b = matrix.At(0, 1);
  const auto c = matrix.At(1, 0);
  const auto d = matrix.At(1, 1);

  EXPECT_EQ(a, std::complex<float>{1});
  EXPECT_EQ(b, std::complex<float>{2});
  EXPECT_EQ(c, std::complex<float>{3});
  EXPECT_EQ(d, std::complex<float>{4});
}

TEST(MatrixBinaryOps, CanEqualityOps_Static) {
  constexpr Matrix2D<std::complex<float>, 2, 3> matrix_a = {{{
      {1, 2, 3},
      {4, 5, 6},
  }}};

  constexpr Matrix2D<std::complex<float>, 2, 3> matrix_a_2 = {{{
      {1, 2, 3},
      {4, 5, 6},
  }}};

  constexpr Matrix2D<std::complex<float>, 2, 2> matrix_b = {{{
      {6, 5},
      {3, 2},
  }}};

  constexpr Matrix2D<std::complex<float>, 2, 2> matrix_b_2 = {{{
      {6, 5},
      {3, 2},
  }}};

  EXPECT_NO_THROW((void)(matrix_a == matrix_a));
  EXPECT_NO_THROW((void)(matrix_b == matrix_b));
  EXPECT_NO_THROW((void)(matrix_a == matrix_a_2));
  EXPECT_NO_THROW((void)(matrix_b == matrix_b_2));
  EXPECT_NO_THROW((void)(matrix_a == matrix_b));
  EXPECT_NO_THROW((void)(matrix_b == matrix_a));
}

TEST(MatrixBinaryOps, CanEqualityOps_Dynamic) {
  Matrix2D<std::complex<float>> matrix_a{2, 3};
  matrix_a.At(0, 0) = std::complex<float>{1};
  matrix_a.At(0, 1) = std::complex<float>{2};
  matrix_a.At(0, 2) = std::complex<float>{3};
  matrix_a.At(1, 0) = std::complex<float>{4};
  matrix_a.At(1, 1) = std::complex<float>{5};
  matrix_a.At(1, 2) = std::complex<float>{6};

  Matrix2D<std::complex<float>> matrix_a_2{2, 3};
  matrix_a_2.At(0, 0) = std::complex<float>{1};
  matrix_a_2.At(0, 1) = std::complex<float>{2};
  matrix_a_2.At(0, 2) = std::complex<float>{3};
  matrix_a_2.At(1, 0) = std::complex<float>{4};
  matrix_a_2.At(1, 1) = std::complex<float>{5};
  matrix_a_2.At(1, 2) = std::complex<float>{6};

  Matrix2D<std::complex<float>> matrix_b{2, 2};
  matrix_b.At(0, 0) = std::complex<float>{6};
  matrix_b.At(0, 1) = std::complex<float>{5};
  matrix_b.At(1, 0) = std::complex<float>{3};
  matrix_b.At(1, 1) = std::complex<float>{2};

  Matrix2D<std::complex<float>> matrix_b_2{2, 2};
  matrix_b_2.At(0, 0) = std::complex<float>{6};
  matrix_b_2.At(0, 1) = std::complex<float>{5};
  matrix_b_2.At(1, 0) = std::complex<float>{3};
  matrix_b_2.At(1, 1) = std::complex<float>{2};

  EXPECT_NO_THROW((void)(matrix_a == matrix_a));
  EXPECT_NO_THROW((void)(matrix_b == matrix_b));
  EXPECT_NO_THROW((void)(matrix_a == matrix_a_2));
  EXPECT_NO_THROW((void)(matrix_b == matrix_b_2));
  EXPECT_NO_THROW((void)(matrix_a == matrix_b));
  EXPECT_NO_THROW((void)(matrix_b == matrix_a));
}

TEST(MatrixBinaryOps, CanEqualityOps_DynamicStatic) {
  constexpr Matrix2D<std::complex<float>, 2, 3> matrix_a = {{{
      {1, 2, 3},
      {4, 5, 6},
  }}};

  constexpr Matrix2D<std::complex<float>, 2, 3> matrix_a_2 = {{{
      {1, 2, 3},
      {4, 5, 6},
  }}};

  Matrix2D<std::complex<float>> matrix_b{2, 2};
  matrix_b.At(0, 0) = std::complex<float>{6};
  matrix_b.At(0, 1) = std::complex<float>{5};
  matrix_b.At(1, 0) = std::complex<float>{3};
  matrix_b.At(1, 1) = std::complex<float>{2};

  Matrix2D<std::complex<float>> matrix_b_2{2, 2};
  matrix_b.At(0, 0) = std::complex<float>{6};
  matrix_b.At(0, 1) = std::complex<float>{5};
  matrix_b.At(1, 0) = std::complex<float>{3};
  matrix_b.At(1, 1) = std::complex<float>{2};

  EXPECT_NO_THROW((void)(matrix_a == matrix_a));
  EXPECT_NO_THROW((void)(matrix_b == matrix_b));
  EXPECT_NO_THROW((void)(matrix_a == matrix_a_2));
  EXPECT_NO_THROW((void)(matrix_b == matrix_b_2));
  EXPECT_NO_THROW((void)(matrix_a == matrix_b));
  EXPECT_NO_THROW((void)(matrix_b == matrix_a));
}

TEST(MatrixBinaryOps, CheckEqualityOps_Static) {
  constexpr Matrix2D<std::complex<float>, 2, 3> matrix_a = {{{
      {1, 2, 3},
      {4, 5, 6},
  }}};

  constexpr Matrix2D<std::complex<float>, 2, 3> matrix_a_same = {{{
      {1, 2, 3},
      {4, 5, 6},
  }}};

  constexpr Matrix2D<std::complex<float>, 2, 3> matrix_a_diff = {{{
      {1, 2, 3},
      {4, 5, 1000000},
  }}};

  constexpr Matrix2D<std::complex<float>, 2, 2> matrix_b = {{{
      {6, 5},
      {3, 2},
  }}};

  constexpr Matrix2D<std::complex<float>, 2, 2> matrix_b_same = {{{
      {6, 5},
      {3, 2},
  }}};

  constexpr Matrix2D<std::complex<float>, 2, 2> matrix_b_diff = {{{
      {6, 5},
      {3, 1000000},
  }}};

  // Matrix A
  EXPECT_EQ(matrix_a, matrix_a);
  EXPECT_EQ(matrix_a, matrix_a_same);
  EXPECT_NE(matrix_a, matrix_a_diff);

  // Matrix B
  EXPECT_EQ(matrix_b, matrix_b);
  EXPECT_EQ(matrix_b, matrix_b_same);
  EXPECT_NE(matrix_b, matrix_b_diff);

  // A != B
  EXPECT_NE(matrix_a, matrix_b);
  EXPECT_NE(matrix_b, matrix_a);
}

TEST(MatrixBinaryOps, CheckEqualityOps_Dynamic) {
  Matrix2D<std::complex<float>> matrix_a{2, 3};
  matrix_a.At(0, 0) = std::complex<float>{1};
  matrix_a.At(0, 1) = std::complex<float>{2};
  matrix_a.At(0, 2) = std::complex<float>{3};
  matrix_a.At(1, 0) = std::complex<float>{4};
  matrix_a.At(1, 1) = std::complex<float>{5};
  matrix_a.At(1, 2) = std::complex<float>{6};

  Matrix2D<std::complex<float>> matrix_a_same{2, 3};
  matrix_a_same.At(0, 0) = std::complex<float>{1};
  matrix_a_same.At(0, 1) = std::complex<float>{2};
  matrix_a_same.At(0, 2) = std::complex<float>{3};
  matrix_a_same.At(1, 0) = std::complex<float>{4};
  matrix_a_same.At(1, 1) = std::complex<float>{5};
  matrix_a_same.At(1, 2) = std::complex<float>{6};

  Matrix2D<std::complex<float>> matrix_a_diff{2, 3};
  matrix_a_diff.At(0, 0) = std::complex<float>{1};
  matrix_a_diff.At(0, 1) = std::complex<float>{2};
  matrix_a_diff.At(0, 2) = std::complex<float>{3};
  matrix_a_diff.At(1, 0) = std::complex<float>{4};
  matrix_a_diff.At(1, 1) = std::complex<float>{5};
  matrix_a_diff.At(1, 2) = std::complex<float>{1000000};

  Matrix2D<std::complex<float>> matrix_b{2, 2};
  matrix_b.At(0, 0) = std::complex<float>{6};
  matrix_b.At(0, 1) = std::complex<float>{5};
  matrix_b.At(1, 0) = std::complex<float>{3};
  matrix_b.At(1, 1) = std::complex<float>{2};

  Matrix2D<std::complex<float>> matrix_b_same{2, 2};
  matrix_b_same.At(0, 0) = std::complex<float>{6};
  matrix_b_same.At(0, 1) = std::complex<float>{5};
  matrix_b_same.At(1, 0) = std::complex<float>{3};
  matrix_b_same.At(1, 1) = std::complex<float>{2};

  Matrix2D<std::complex<float>> matrix_b_diff{2, 2};
  matrix_b_diff.At(0, 0) = std::complex<float>{6};
  matrix_b_diff.At(0, 1) = std::complex<float>{5};
  matrix_b_diff.At(1, 0) = std::complex<float>{3};
  matrix_b_diff.At(1, 1) = std::complex<float>{1000000};

  // Matrix A
  EXPECT_EQ(matrix_a, matrix_a);
  EXPECT_EQ(matrix_a, matrix_a_same);
  EXPECT_NE(matrix_a, matrix_a_diff);

  // Matrix B
  EXPECT_EQ(matrix_b, matrix_b);
  EXPECT_EQ(matrix_b, matrix_b_same);
  EXPECT_NE(matrix_b, matrix_b_diff);

  // A != B
  EXPECT_NE(matrix_a, matrix_b);
  EXPECT_NE(matrix_b, matrix_a);
}

TEST(MatrixBinaryOps, CheckEqualityOps_DynamicStatic) {

  // A
  Matrix2D<std::complex<float>> matrix_a{2, 3};
  matrix_a.At(0, 0) = std::complex<float>{1};
  matrix_a.At(0, 1) = std::complex<float>{2};
  matrix_a.At(0, 2) = std::complex<float>{3};
  matrix_a.At(1, 0) = std::complex<float>{4};
  matrix_a.At(1, 1) = std::complex<float>{5};
  matrix_a.At(1, 2) = std::complex<float>{6};

  constexpr Matrix2D<std::complex<float>, 2, 3> matrix_a_same = {{{
      {1, 2, 3},
      {4, 5, 6},
  }}};

  constexpr Matrix2D<std::complex<float>, 2, 3> matrix_a_diff = {{{
      {1, 2, 3},
      {4, 5, 1000000},
  }}};

  // B
  constexpr Matrix2D<std::complex<float>, 2, 2> matrix_b = {{{
      {6, 5},
      {3, 2},
  }}};

  Matrix2D<std::complex<float>> matrix_b_same{2, 2};
  matrix_b_same.At(0, 0) = std::complex<float>{6};
  matrix_b_same.At(0, 1) = std::complex<float>{5};
  matrix_b_same.At(1, 0) = std::complex<float>{3};
  matrix_b_same.At(1, 1) = std::complex<float>{2};

  Matrix2D<std::complex<float>> matrix_b_diff{2, 2};
  matrix_b_diff.At(0, 0) = std::complex<float>{6};
  matrix_b_diff.At(0, 1) = std::complex<float>{5};
  matrix_b_diff.At(1, 0) = std::complex<float>{3};
  matrix_b_diff.At(1, 1) = std::complex<float>{1000000};

  // Matrix A
  EXPECT_EQ(matrix_a, matrix_a);
  EXPECT_EQ(matrix_a, matrix_a_same);
  EXPECT_NE(matrix_a, matrix_a_diff);

  // Matrix B
  EXPECT_EQ(matrix_b, matrix_b);
  EXPECT_EQ(matrix_b, matrix_b_same);
  EXPECT_NE(matrix_b, matrix_b_diff);

  // A != B
  EXPECT_NE(matrix_a, matrix_b);
  EXPECT_NE(matrix_b, matrix_a);
}

TEST(MatrixBinaryOps, CanAddAndSubtract_Static) {
  constexpr Matrix2D<std::complex<float>, 2, 3> matrix_a = {{{
      {1, 2, 3},
      {4, 5, 6},
  }}};

  constexpr Matrix2D<std::complex<float>, 2, 3> matrix_b = {{{
      {6, 5, 4},
      {3, 2, 1},
  }}};

  EXPECT_NO_THROW(auto result = matrix_a.Plus(matrix_b));
  EXPECT_NO_THROW(auto result = matrix_b.Plus(matrix_a));
  EXPECT_NO_THROW(auto result = matrix_a.Minus(matrix_b));
  EXPECT_NO_THROW(auto result = matrix_b.Minus(matrix_a));
}

TEST(MatrixBinaryOps, CanAddAndSubtract_Dynamic) {
  Matrix2D<std::complex<float>> matrix_a{2, 3};
  matrix_a.At(0, 0) = std::complex<float>{1};
  matrix_a.At(0, 1) = std::complex<float>{2};
  matrix_a.At(0, 2) = std::complex<float>{3};
  matrix_a.At(1, 0) = std::complex<float>{4};
  matrix_a.At(1, 1) = std::complex<float>{5};
  matrix_a.At(1, 2) = std::complex<float>{6};

  Matrix2D<std::complex<float>> matrix_b{2, 3};
  matrix_b.At(0, 0) = std::complex<float>{6};
  matrix_b.At(0, 1) = std::complex<float>{5};
  matrix_b.At(0, 2) = std::complex<float>{4};
  matrix_b.At(1, 0) = std::complex<float>{3};
  matrix_b.At(1, 1) = std::complex<float>{2};
  matrix_b.At(1, 2) = std::complex<float>{1};

  EXPECT_NO_THROW(auto result = matrix_a.Plus(matrix_b));
  EXPECT_NO_THROW(auto result = matrix_b.Plus(matrix_a));
  EXPECT_NO_THROW(auto result = matrix_a.Minus(matrix_b));
  EXPECT_NO_THROW(auto result = matrix_b.Minus(matrix_a));
}

TEST(MatrixBinaryOps, CanAddAndSubtract_DynamicStatic) {
  constexpr Matrix2D<std::complex<float>, 2, 3> matrix_a = {{{
      {1, 2, 3},
      {4, 5, 6},
  }}};

  Matrix2D<std::complex<float>> matrix_b{2, 3};
  matrix_b.At(0, 0) = std::complex<float>{6};
  matrix_b.At(0, 1) = std::complex<float>{5};
  matrix_b.At(0, 2) = std::complex<float>{4};
  matrix_b.At(1, 0) = std::complex<float>{3};
  matrix_b.At(1, 1) = std::complex<float>{2};
  matrix_b.At(1, 2) = std::complex<float>{1};

  EXPECT_NO_THROW(auto result = matrix_a.Plus(matrix_b));
  EXPECT_NO_THROW(auto result = matrix_b.Plus(matrix_a));
  EXPECT_NO_THROW(auto result = matrix_a.Minus(matrix_b));
  EXPECT_NO_THROW(auto result = matrix_b.Minus(matrix_a));
}

TEST(MatrixBinaryOps, CorrectValueAddSubtract_Static) {
  constexpr Matrix2D<std::complex<float>, 2, 3> matrix_a = {{{
      {-70, -5, -2},
      {-100, -80, 71},
  }}};

  constexpr Matrix2D<std::complex<float>, 2, 3> matrix_b = {{{
      {58, -92, 39},
      {-46, -32, 66},
  }}};

  // Results
  constexpr Matrix2D<std::complex<float>, 2, 3> correct_result_addition = {{{
      {-12, -97, 37},
      {-146, -112, 137},
  }}};

  constexpr Matrix2D<std::complex<float>, 2, 3> correct_result_a_minus_b = {{{
      {-128, 87, -41},
      {-54, -48, 5},
  }}};

  constexpr Matrix2D<std::complex<float>, 2, 3> correct_result_b_minus_a = {{{
      {128, -87, 41},
      {54, 48, -5},
  }}};

  EXPECT_EQ(correct_result_addition, matrix_a.Plus(matrix_b));
  EXPECT_EQ(correct_result_addition, matrix_b.Plus(matrix_a));
  EXPECT_EQ(correct_result_a_minus_b, matrix_a.Minus(matrix_b));
  EXPECT_EQ(correct_result_b_minus_a, matrix_b.Minus(matrix_a));
}

TEST(MatrixBinaryOps, CorrectValueAddSubtract_Dynamic) {
  Matrix2D<std::complex<float>> matrix_a{2, 2};
  matrix_a.At(0, 0) = std::complex<float>{10};
  matrix_a.At(0, 1) = std::complex<float>{20};
  matrix_a.At(1, 0) = std::complex<float>{40};
  matrix_a.At(1, 1) = std::complex<float>{50};

  Matrix2D<std::complex<float>> matrix_b{2, 2};
  matrix_b.At(0, 0) = std::complex<float>{-6};
  matrix_b.At(0, 1) = std::complex<float>{5};
  matrix_b.At(1, 0) = std::complex<float>{3};
  matrix_b.At(1, 1) = std::complex<float>{-2};

  Matrix2D<std::complex<float>> correct_result_addition{2, 2};
  correct_result_addition.At(0, 0) = std::complex<float>{4};
  correct_result_addition.At(0, 1) = std::complex<float>{25};
  correct_result_addition.At(1, 0) = std::complex<float>{43};
  correct_result_addition.At(1, 1) = std::complex<float>{48};

  Matrix2D<std::complex<float>> correct_result_a_minus_b{2, 2};
  correct_result_a_minus_b.At(0, 0) = std::complex<float>{16};
  correct_result_a_minus_b.At(0, 1) = std::complex<float>{15};
  correct_result_a_minus_b.At(1, 0) = std::complex<float>{37};
  correct_result_a_minus_b.At(1, 1) = std::complex<float>{52};

  Matrix2D<std::complex<float>> correct_result_b_minus_a{2, 2};
  correct_result_b_minus_a.At(0, 0) = std::complex<float>{-16};
  correct_result_b_minus_a.At(0, 1) = std::complex<float>{-15};
  correct_result_b_minus_a.At(1, 0) = std::complex<float>{-37};
  correct_result_b_minus_a.At(1, 1) = std::complex<float>{-52};

  EXPECT_EQ(correct_result_addition, matrix_a.Plus(matrix_b));
  EXPECT_EQ(correct_result_addition, matrix_b.Plus(matrix_a));
  EXPECT_EQ(correct_result_a_minus_b, matrix_a.Minus(matrix_b));
  EXPECT_EQ(correct_result_b_minus_a, matrix_b.Minus(matrix_a));
}

TEST(MatrixBinaryOps, CorrectValueAddSubtract_DynamicStaticA) {
  constexpr Matrix2D<std::complex<float>, 2, 2> matrix_a = {{{
      {10, 20},
      {40, 50},
  }}};

  Matrix2D<std::complex<float>> matrix_b{2, 2};
  matrix_b.At(0, 0) = std::complex<float>{-6};
  matrix_b.At(0, 1) = std::complex<float>{5};
  matrix_b.At(1, 0) = std::complex<float>{3};
  matrix_b.At(1, 1) = std::complex<float>{-2};

  Matrix2D<std::complex<float>> correct_result_addition{2, 2};
  correct_result_addition.At(0, 0) = std::complex<float>{10 + -6};
  correct_result_addition.At(0, 1) = std::complex<float>{20 + 5};
  correct_result_addition.At(1, 0) = std::complex<float>{40 + 3};
  correct_result_addition.At(1, 1) = std::complex<float>{50 + -2};

  Matrix2D<std::complex<float>> correct_result_a_minus_b{2, 2};
  correct_result_a_minus_b.At(0, 0) = std::complex<float>{16};
  correct_result_a_minus_b.At(0, 1) = std::complex<float>{15};
  correct_result_a_minus_b.At(1, 0) = std::complex<float>{37};
  correct_result_a_minus_b.At(1, 1) = std::complex<float>{52};

  Matrix2D<std::complex<float>> correct_result_b_minus_a{2, 2};
  correct_result_b_minus_a.At(0, 0) = std::complex<float>{-16};
  correct_result_b_minus_a.At(0, 1) = std::complex<float>{-15};
  correct_result_b_minus_a.At(1, 0) = std::complex<float>{-37};
  correct_result_b_minus_a.At(1, 1) = std::complex<float>{-52};

  EXPECT_EQ(correct_result_addition, matrix_a.Plus(matrix_b));
  EXPECT_EQ(correct_result_addition, matrix_b.Plus(matrix_a));
  EXPECT_EQ(correct_result_a_minus_b, matrix_a.Minus(matrix_b));
  EXPECT_EQ(correct_result_b_minus_a, matrix_b.Minus(matrix_a));
}

TEST(MatrixBinaryOps, CorrectValueAddSubtract_DynamicStaticB) {
  Matrix2D<std::complex<float>> matrix_a{2, 2};
  matrix_a.At(0, 0) = std::complex<float>{10};
  matrix_a.At(0, 1) = std::complex<float>{20};
  matrix_a.At(1, 0) = std::complex<float>{40};
  matrix_a.At(1, 1) = std::complex<float>{50};

  constexpr Matrix2D<std::complex<float>, 2, 2> matrix_b = {{{
      {-6, 5},
      {3, -2},
  }}};

  constexpr Matrix2D<std::complex<float>, 2, 2> correct_result_addition = {{{
      {4, 25},
      {43, 48},
  }}};

  constexpr Matrix2D<std::complex<float>, 2, 2> correct_result_a_minus_b = {{{
      {16, 15},
      {37, 52},
  }}};

  constexpr Matrix2D<std::complex<float>, 2, 2> correct_result_b_minus_a = {{{
      {-16, -15},
      {-37, -52},
  }}};

  EXPECT_EQ(correct_result_addition, matrix_a.Plus(matrix_b));
  EXPECT_EQ(correct_result_addition, matrix_b.Plus(matrix_a));
  EXPECT_EQ(correct_result_a_minus_b, matrix_a.Minus(matrix_b));
  EXPECT_EQ(correct_result_b_minus_a, matrix_b.Minus(matrix_a));
}