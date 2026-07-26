//
// Created by Eli Michaud on 7/7/2026.
//

#ifndef BITWISE_MATH_H
#define BITWISE_MATH_H
#include <span>

/// Bit-twiddling magic
namespace bit {

/**
 * Find 2 to the nth power
 * @return 2^n
 */
template <typename T>
constexpr auto TwoPowN(T n) {
  return (static_cast<T>(1) << n);
}

/**
 * Get a bit from a number
 * @param input The number to get the bit from
 * @param n The index of the bit
 * @return The nth bit
 */
template <typename T, typename IndexType = std::size_t>
constexpr auto GetBit(T input, IndexType n) {
  return (input >> n) & 1;
}

/**
 * Returns input with its bits rearranged to match the new positions. That is,
 * indices [0, 1, 2, ..., 3] maps to new_positions. Put another way, the kth bit of i is swapped
 * with position new_positions[k].
 *
 * @remark Implementation based on McGuffin et al. \cite mcguffin_2025.
 *
 * @param input The number to swap
 * @param new_positions The list of new positions
 * @return The input with its positions swapped
 */
template <typename T, typename IndexType = std::size_t>
constexpr auto RearrangeBits(T input, std::span<const IndexType> new_positions) {
  T out = {};

  for (std::size_t pos = 0; pos < new_positions.size(); ++pos) {
    auto new_pos = new_positions[pos];
    if (new_pos < 0) continue;

    out |= GetBit(input, pos) << new_pos;
  }

  return out;
}

} // namespace bit

#endif // BITWISE_MATH_H
