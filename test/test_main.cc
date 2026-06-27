//
// Created by Eli Michaud on 6/27/2026.
//

#include <gtest/gtest.h>

#include "math/matrix.h"

TEST(MatrixTests, HasCorrectSize_Static) {
  constexpr Matrix2D<std::complex<float>, 2, 2> matrix = {{{
      {std::complex<float>{3.2, -90.1}, {500, -0.1}},
      {std::complex<float>{80.0009, 0}, {0, 1.90}},
  }}};

  EXPECT_EQ(matrix.NumCols(), 2);
  EXPECT_EQ(matrix.NumRows(), 2);
}

TEST(MatrixTests, HasCorrectSize_Dynamic) {
  const Matrix2D<std::complex<float>> matrix{2, 2};

  EXPECT_EQ(matrix.NumCols(), 2);
  EXPECT_EQ(matrix.NumRows(), 2);
}