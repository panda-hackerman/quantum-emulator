//
// Created by Eli Michaud on 6/12/2026.
//

#ifndef GATES_H
#define GATES_H
#include <complex>

#include "math/matrix.h"

namespace gates {

template <std::size_t Rows, std::size_t Cols>
using StaticComplexMatrix = Matrix2D<std::complex<float>, Rows, Cols>;

constexpr float kSqrt2 = 1.414214f; // = std::sqrt(2) (which bafflingly isn't constexpr)
constexpr std::complex<float> kIm = {0, 1}; ///< i = std::sqrt(-1)
constexpr std::complex<float> kNorm = {1 / kSqrt2, 0}; ///< normalization constant, i.e, 1/sqrt(2).
constexpr std::complex<float> kRot4 = {1 / kSqrt2, 1 / kSqrt2}; ///< = exp (i * pi / 4) = 45 deg

static constexpr StaticComplexMatrix<2, 2> kIdentity{{{
    {1, 0},
    {0, 1},
}}};

constexpr StaticComplexMatrix<2, 2> kPauliX = {{{
    {0, 1},
    {1, 0},
}}};

constexpr StaticComplexMatrix<2, 2> kPauliY = {{{
    {0, -kIm},
    {kIm, 0},
}}};

constexpr StaticComplexMatrix<2, 2> kPauliZ = {{{
    {1, 0},
    {0, -1},
}}};

constexpr StaticComplexMatrix<2, 2> kHadamard = {{{
    {kNorm, kNorm},
    {kNorm, -kNorm},
}}};

constexpr StaticComplexMatrix<2, 2> kTGate = {{{
    {1, 0},
    {0, kRot4},
}}};

/// CX_{j, j-1}: Controlled not gate where the bit to be flipped (j-1) is below the control bit (j)
constexpr StaticComplexMatrix<4, 4> kControlledNotBelow = {{{
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 0, 1},
    {0, 0, 1, 0},
}}};

/// CX_{j, j+1}: Controlled not gate where the bit to be flipped (j-1) is below the control bit (j)
constexpr StaticComplexMatrix<4, 4> kControlledNotAbove = {{{
    {1, 0, 0, 0},
    {0, 0, 0, 1},
    {0, 0, 1, 0},
    {0, 1, 0, 0},
}}};

} // namespace gates

#endif // GATES_H
