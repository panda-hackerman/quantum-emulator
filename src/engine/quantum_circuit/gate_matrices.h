//
// Created by Eli Michaud on 6/12/2026.
//

#ifndef GATES_H
#define GATES_H
#include <complex>

#include "math/matrix.h"
#include "types.h"

namespace matrix {

template <std::size_t Qubits>
using Gate = Matrix2D<Complex, 1 << Qubits, 1 << Qubits>;

inline constexpr float kSqrt2 = 1.414214f; // = std::sqrt(2) (which bafflingly isn't constexpr)
inline constexpr Complex kIm = {0, 1}; ///< i = std::sqrt(-1)
inline constexpr Complex kNorm = {1 / kSqrt2, 0}; ///< normalization constant: 1/sqrt(2)
inline constexpr Complex kRot4 = {1 / kSqrt2, 1 / kSqrt2}; ///< = exp (i * pi / 4) = 45°

inline constexpr Gate<1> kIdentity = {{{
    {1, 0},
    {0, 1},
}}};

inline constexpr Gate<1> kPauliX = {{{
    {0, 1},
    {1, 0},
}}};

inline constexpr Gate<1> kPauliY = {{{
    {0, -kIm},
    {kIm, 0},
}}};

inline constexpr Gate<1> kPauliZ = {{{
    {1, 0},
    {0, -1},
}}};

inline constexpr Gate<1> kHadamard = {{{
    {kNorm, kNorm},
    {kNorm, -kNorm},
}}};

inline constexpr Gate<1> kPi8ths = {{{
    {1, 0},
    {0, kRot4},
}}};

/// CX_{j, j-1}: Controlled not gate where the bit to be flipped (j-1) is below the control bit (j)
inline constexpr Gate<2> kControlledNotBelow = {{{
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 0, 1},
    {0, 0, 1, 0},
}}};

/// CX_{j, j+1}: Controlled not gate where the bit to be flipped (j-1) is below the control bit (j)
inline constexpr Gate<2> kControlledNotAbove = {{{
    {1, 0, 0, 0},
    {0, 0, 0, 1},
    {0, 0, 1, 0},
    {0, 1, 0, 0},
}}};

} // namespace matrix

#endif // GATES_H
