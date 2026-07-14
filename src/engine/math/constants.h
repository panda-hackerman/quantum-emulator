/**
 * @file constants.h
 * @author Eli Michaud
 * @since 7/3/2026
 */

#ifndef QUANTUMEMULATOR_CONSTANTS_H
#define QUANTUMEMULATOR_CONSTANTS_H

#include "matrix.h"
#include "types.h"

template <typename T, std::size_t Size>
using RowVector = Matrix2D<T, 1, Size>;

template <typename T, std::size_t Size>
using ColVector = Matrix2D<T, Size, 1>;

template <std::size_t Qubits>
using Gate = Matrix2D<Complex, 1 << Qubits, 1 << Qubits>;

namespace matrix {

/// Equivalent to std::sqrt(2) (which bafflingly isn't constexpr)
inline constexpr Complex_Base kSqrt2 = (Complex_Base)1.4142135623731;

/// The imaginary number i = std::sqrt(-1)
inline constexpr Complex kIm = {0, 1};

/// The normalization constant = 1/sqrt(2)
inline constexpr Complex kNorm = {1 / kSqrt2, 0};

/// = exp (i * pi / 4) = 45° rotation
inline constexpr Complex kRot4 = {1 / kSqrt2, 1 / kSqrt2};

/* --- BLOCH SPHERE CANONICAL POINTS --- */
inline constexpr RowVector<Complex, 2> kBraZero = {{{{1, 0}}}}; ///< = |0>
inline constexpr RowVector<Complex, 2> kBraOne = {{{{0, 1}}}};  ///< = |1>
inline constexpr ColVector<Complex, 2> kKetZero = {{{{1}, {0}}}}; ///< = <0|
inline constexpr ColVector<Complex, 2> kKetOne = {{{{0}, {1}}}};  ///< = <0|

/* - MATRICES FOR COMMON QUANTUM GATES - */

/// Identity Matrix
inline constexpr Gate<1> kIdentity = {{{
    {1, 0},
    {0, 1},
}}};

/// Matrix for Pauli X gate (aka NOT)
inline constexpr Gate<1> kPauliX = {{{
    {0, 1},
    {1, 0},
}}};

/// Matrix for Pauli Y gate
inline constexpr Gate<1> kPauliY = {{{
    {0, -kIm},
    {kIm, 0},
}}};

/// Matrix for Pauli Z gate
inline constexpr Gate<1> kPauliZ = {{{
    {1, 0},
    {0, -1},
}}};

/// Matrix for the Hadamard gate
inline constexpr Gate<1> kHadamard = {{{
    {kNorm, kNorm},
    {kNorm, -kNorm},
}}};

/// Matrix for the T gate
inline constexpr Gate<1> kPi8ths = {{{
    {1, 0},
    {0, kRot4},
}}};

/// Matrix for the S gate
inline constexpr Gate<1> kPhase = {{{
    {1, 0},
    {0, kIm},
}}};

} // namespace matrix

#endif // QUANTUMEMULATOR_CONSTANTS_H
