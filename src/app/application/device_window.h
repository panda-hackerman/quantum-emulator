//
// Created by Eli Michaud on 6/23/2026.
//

#ifndef DEVICE_WINDOW_H
#define DEVICE_WINDOW_H

#include <cstdint>

// Forward definitions
struct WGPUSurfaceImpl;
struct GLFWwindow;
typedef WGPUSurfaceImpl *WGPUSurface;

/// Wrapper around the device native (GLFW) window.
class DeviceWindow {
private:
  GLFWwindow *window_ = nullptr;
  WGPUSurface *surface_ptr_ = nullptr;

  bool is_initialized_ = false;
  std::uint16_t width_ = 0;
  std::uint16_t height_ = 0;

  void OnResize(int width, int height);

public:
  static constexpr int kDefaultWidth = 640;
  static constexpr int kDefaultHeight = 480;
  static constexpr const char *kWindowTitle = "Quantum Emulator";

  explicit DeviceWindow(WGPUSurface *surface) : surface_ptr_{surface} {}

  bool Init();
  void Terminate();

  [[nodiscard]] GLFWwindow *GetNative() const noexcept { return window_; }
  [[nodiscard]] std::uint16_t GetWidth() const noexcept { return width_; }
  [[nodiscard]] std::uint16_t GetHeight() const noexcept { return height_; }

  ~DeviceWindow() { Terminate(); }
};

#endif // DEVICE_WINDOW_H
