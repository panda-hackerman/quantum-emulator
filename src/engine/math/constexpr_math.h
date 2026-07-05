/**
 * @file constexpr_math.h
 * @author Eli Michaud
 * @since 7/4/2026
 */

#ifndef QUANTUMEMULATOR_CONSTEXPR_MATH_H
#define QUANTUMEMULATOR_CONSTEXPR_MATH_H

template <typename T>
constexpr T FloorLog2(const T input) {
  return input == 1 ? 0 : 1 + FloorLog2(input >> 1);
}

template <typename T>
constexpr T CeilLog2(const T input) {
  return input == 1 ? 0 : FloorLog2(input - 1) + 1;
}

#endif // QUANTUMEMULATOR_CONSTEXPR_MATH_H
