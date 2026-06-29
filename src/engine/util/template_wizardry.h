//
// Created by Eli Michaud on 6/26/2026.
//

#ifndef TEMPLATE_WIZARDRY_H
#define TEMPLATE_WIZARDRY_H
#include <type_traits>

/// Template helpers
namespace tmp {
// NOLINTBEGIN

/// Check if a type is an instantiation of a template class. Example usage:
/// @code is_instance_of_v<vector<int>, vector> = true.
template <class T, template <class...> class U>
inline constexpr bool is_instance_of_v = std::false_type{};

/// Check if a type is an instantiation of a template class. Example usage:
/// @code is_instance_of_v<vector<int>, vector> = true.
template <template <class...> class U, class... Vs>
inline constexpr bool is_instance_of_v<U<Vs...>, U> = std::true_type{};

/**
 * The theoretical maximum number of unique values this type can store (assuming all possible bit
 * strings results in a valid T). Equivalent to 2^N, where N is the size of T in bits.
 */
template <typename T>
inline constexpr std::size_t max_unique_v = 1 << (8 * sizeof(T));

/// The type returned from adding L + R
template <typename L, typename R>
using addition_result = std::invoke_result<decltype([](L lhs, R rhs) { return lhs + rhs; }), L, R>;

template <typename L, typename R>
using addition_result_t = addition_result<L, R>::type;

template <typename L, typename R>
inline constexpr bool can_add_v = std::is_invocable_v<decltype([](L lhs, R rhs) { return lhs + rhs; }), L, R>;

/// The type returned from subtracting L - R
template <typename L, typename R>
using subtract_result = std::invoke_result<decltype([](L lhs, R rhs) { return lhs - rhs; }), L, R>;

template <typename L, typename R>
using subtract_result_t = subtract_result<L, R>::type;

template <typename L, typename R>
inline constexpr bool can_subtract_v = std::is_invocable_v<decltype([](L lhs, R rhs) { return lhs - rhs; }), L, R>;

/// The type returned from multiplying L * R
template <typename L, typename R>
using multiply_result = std::invoke_result<decltype([](L lhs, R rhs) { return lhs * rhs; }), L, R>;

template <typename L, typename R>
using multiply_result_t = multiply_result<L, R>::type;

template <typename L, typename R>
inline constexpr bool can_multiply_v = std::is_invocable_v<decltype([](L lhs, R rhs) { return lhs * rhs; }), L, R>;

/// The type returned from dividing L / R
template <typename L, typename R>
using divide_result = std::invoke_result<decltype([](L lhs, R rhs) { return lhs / rhs; }), L, R>;

template <typename L, typename R>
using divide_result_t = divide_result<L, R>::type;

template <typename L, typename R>
inline constexpr bool can_divide_v = std::is_invocable_v<decltype([](L lhs, R rhs) { return lhs / rhs; }), L, R>;

// NOLINTEND
} // namespace tmp

#endif // TEMPLATE_WIZARDRY_H
