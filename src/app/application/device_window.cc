//
// Created by Eli Michaud on 6/24/2026.
//

#include "device_window.h"

#include <GLFW/glfw3.h>

#include <iostream>
#include <webgpu/webgpu.hpp>

#include "application.h"

bool DeviceWindow::Init() {
  if (is_initialized_) return true;

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window_ = glfwCreateWindow(kDefaultWidth, kDefaultHeight, kWindowTitle, nullptr, nullptr);

  if (!window_) return false;

  width_ = kDefaultWidth;
  height_ = kDefaultHeight;

  glfwSetWindowUserPointer(window_, this);
  glfwSetFramebufferSizeCallback(window_, [](GLFWwindow *window, const int w, const int h) {
    if (w == 0 || h == 0) return; //Window was minimized...
    if (const auto ptr = reinterpret_cast<DeviceWindow *>(glfwGetWindowUserPointer(window)); ptr) {
      ptr->OnResize(w, h); // Callback calls the resize callback
    }
  });

  is_initialized_ = true;
  return true;
}

void DeviceWindow::Terminate() {
  if (is_initialized_) {
    glfwDestroyWindow(window_);
  }

  is_initialized_ = false;
}

void DeviceWindow::OnResize(const int width, const int height) {
  if (width <= 0 || height <= 0) {
    throw std::runtime_error("Error resizing window: Window size must be strictly positive!");
  }

  width_ = static_cast<std::uint16_t>(width);
  height_ = static_cast<std::uint16_t>(height);

  // Reconfigure surface
  const WGPUSurfaceConfiguration surface_config = Application::Instance().BuildSurfaceConfig(*this);
  wgpuSurfaceConfigure(*surface_ptr_, &surface_config);
}
