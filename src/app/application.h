//
// Created by Eli Michaud on 6/5/2026.
//

#ifndef APPLICATION_H
#define APPLICATION_H

// #include <webgpu.h>
#include <webgpu/webgpu.hpp>

#include "GLFW/glfw3.h"

constexpr int kWindowWidth = 640;
constexpr int kWindowHeight = 480;
constexpr WGPUColor kWindowClearColor = {100 / 255.0, 149 / 255.0, 237 / 255.0, 1};

class Application {
private:
  bool is_running_ = false;
  GLFWwindow *window_ = nullptr;
  WGPUInstance instance_ = nullptr;
  WGPUDevice device_ = nullptr;
  WGPUQueue queue_ = nullptr;
  WGPUSurface surface_ = nullptr;

  WGPUTextureView GetNextTextureView();

  void BeginFrame();
  void DrawFrame();
  void EndFrame();

public:
  bool Init(); ///< Called on initialization
  void Terminate(); ///< Terminates all processes (called automatically in destructor)
  void Tick(); ///< Called every update

  /**
   * True if the program should continue running.
   * @return False if the GLFW window should close, otherwise true.
   */
  [[nodiscard]] bool ShouldContinue() const { return !glfwWindowShouldClose(window_); }

  ~Application() { Terminate(); }
};

#endif // APPLICATION_H
