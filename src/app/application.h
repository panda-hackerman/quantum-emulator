//
// Created by Eli Michaud on 6/5/2026.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include <webgpu.h>

#include "GLFW/glfw3.h"

constexpr int kWindowWidth = 640;
constexpr int kWindowHeight = 480;

class Application {
private:
  bool is_running_ = false;
  GLFWwindow *window_ = nullptr;
  WGPUInstance instance_ = nullptr;
  WGPUDevice device_ = nullptr;
  WGPUQueue queue_ = nullptr;
  WGPUSurface surface_ = nullptr;

  WGPUTextureView GetNextTextureView();

  bool InitGUI();
  void TerminateGUI();

public:
  bool Init();
  void Terminate();
  void Tick();
  [[nodiscard]] bool ShouldContinue() const;

  ~Application() { Terminate(); }
};

#endif // APPLICATION_H
