//
// Created by Eli Michaud on 6/9/2026.
//

#ifndef WGPU_STRING_VIEW_UTIL_H
#define WGPU_STRING_VIEW_UTIL_H
#include <webgpu.h>

#include <string>

// WGPUStringView <--> std::string_view

/// Convert a WGPUStringView to a std::string_view
constexpr std::string_view ToStringView(const WGPUStringView wgpu_sv) noexcept {
  if (wgpu_sv.data == nullptr) {
    return {};
  }

  if (wgpu_sv.length == WGPU_STRLEN) {
    return {wgpu_sv.data};
  } else {
    return {wgpu_sv.data, wgpu_sv.length};
  }
}

constexpr WGPUStringView ToWGPUString(const std::string_view sv) noexcept {
  return {sv.data(), sv.size()};
}

constexpr WGPUStringView ToWGPUString(const char *c_str) noexcept {
  return {c_str, WGPU_STRLEN};
}

// STRING LITERALS

/// Turns a C-style string into a WGPUStringView
constexpr WGPUStringView operator""_w(const char *str, const std::size_t len) noexcept {
  return {str, len};
}

// COUT OVERLOAD
inline std::ostream &operator<<(std::ostream &out, const WGPUStringView &sv) {
  return std::operator<<(out, ToStringView(sv));
}

#endif // WGPU_STRING_VIEW_UTIL_H
