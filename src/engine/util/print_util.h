//
// Created by Eli Michaud on 6/13/2026.
//

#ifndef PRINT_UTIL_H
#define PRINT_UTIL_H
#include <complex>

using std::literals::string_literals::operator ""s;

/**
 * Pretty print a floating-point number (no trailing zeroes)
 * @tparam T The floating point type
 * @param num The number to format
 * @return The number with no trailing zeroes
 */
template <typename T, std::enable_if_t<std::is_floating_point_v<std::remove_cv_t<T>>, bool> = true>
static std::string PrettyPrint(T num) {
  std::string str = std::to_string(num);
  str.erase(str.find_last_not_of('0') + 1, std::string::npos);
  str.erase(str.find_last_not_of('.') + 1, std::string::npos);

  return str;
}

/// Pretty print an integer
template <typename T, std::enable_if_t<!std::is_floating_point_v<std::remove_cv_t<T>>, bool> = true>
  requires(std::is_arithmetic_v<std::remove_cv_t<T>>)
static std::string PrettyPrint(T num) {
  return std::to_string(num);
}

/// Pretty print a complex number
template <typename T>
static std::string PrettyPrint(const std::complex<T> &num) {

  const T real = num.real();
  const T imag = num.imag();

  if (std::isnan(real) || std::isnan(imag)) {
    return "NaN"s;  // Something has gone wrong (division by 0?)
  }

  if (real == 0.0 && imag == 0.0) {
    return "0"s;
  }

  // Both are non-zero
  if (real != 0.0 && imag != 0.0) {
    const std::string sign_str = imag < 0 ? " - " : " + ";
    const std::string real_str = PrettyPrint(real);

    if (const double abs_i = std::abs(imag); abs_i == 1.0) {
      return real_str + sign_str + "i"s; // 1 + i
    } else {
      return real_str + sign_str + PrettyPrint(abs_i) + "i"s; // 1 + 2i
    }
  }

  // Purely imaginary
  if (real == 0.0) {
    if (imag == 1) return "i"s;
    if (imag == -1) return "-i"s;
    return PrettyPrint(imag) + "i";
  }

  // Purely real
  return PrettyPrint(real);
}

#endif // PRINT_UTIL_H
