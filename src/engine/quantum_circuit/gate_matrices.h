//
// Created by Eli Michaud on 6/12/2026.
//

#ifndef GATES_H
#define GATES_H
#include <complex>

#include "math/matrix.h"

namespace matrix {

template <std::size_t Rows = kDynamicSize, std::size_t Cols = kDynamicSize>
using ComplexMatrix = Matrix2D<std::complex<float>, Rows, Cols>;

template <std::size_t Qubits>
using GateMatrix = ComplexMatrix<1 << Qubits, 1 << Qubits>;

inline constexpr float kSqrt2 = 1.414214f; // = std::sqrt(2) (which bafflingly isn't constexpr)
inline constexpr std::complex<float> kIm = {0, 1}; ///< i = std::sqrt(-1)
inline constexpr std::complex<float> kNorm = {1 / kSqrt2, 0}; ///< normalization constant: 1/sqrt(2)
inline constexpr std::complex<float> kRot4 = {1 / kSqrt2, 1 / kSqrt2}; ///< = exp (i * pi / 4) = 45°

inline constexpr GateMatrix<1> kIdentity = {{{
    {1, 0},
    {0, 1},
}}};

inline constexpr GateMatrix<1> kPauliX = {{{
    {0, 1},
    {1, 0},
}}};

inline constexpr GateMatrix<1> kPauliY = {{{
    {0, -kIm},
    {kIm, 0},
}}};

inline constexpr GateMatrix<1> kPauliZ = {{{
    {1, 0},
    {0, -1},
}}};

inline constexpr GateMatrix<1> kHadamard = {{{
    {kNorm, kNorm},
    {kNorm, -kNorm},
}}};

inline constexpr GateMatrix<1> kPi8ths = {{{
    {1, 0},
    {0, kRot4},
}}};

/// CX_{j, j-1}: Controlled not gate where the bit to be flipped (j-1) is below the control bit (j)
inline constexpr GateMatrix<2> kControlledNotBelow = {{{
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 0, 1},
    {0, 0, 1, 0},
}}};

/// CX_{j, j+1}: Controlled not gate where the bit to be flipped (j-1) is below the control bit (j)
inline constexpr GateMatrix<2> kControlledNotAbove = {{{
    {1, 0, 0, 0},
    {0, 0, 0, 1},
    {0, 0, 1, 0},
    {0, 1, 0, 0},
}}};

} // namespace matrix

#endif // GATES_H
