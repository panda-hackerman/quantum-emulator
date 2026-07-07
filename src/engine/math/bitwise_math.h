//
// Created by Eli Michaud on 7/7/2026.
//

#ifndef BITWISE_MATH_H
#define BITWISE_MATH_H

/// Bit-twiddling magic
namespace bit {

/**
 * Find 2 to the nth power
 * @return 2^n
 */
template <typename T>
constexpr auto TwoPowN(T n) {
  return (1 << n);
}

/**
 * Get a bit from a number
 * @param input The number to get the bit from
 * @param n The index of the bit
 * @return The nth bit
 */
template <typename T>
constexpr auto GetBit(T input, T n) {
  return (input >> n) & 1;
}

} // namespace bit

#endif // BITWISE_MATH_H
