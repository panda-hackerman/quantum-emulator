//
// Created by Eli Michaud on 6/5/2026.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include <webgpu/webgpu.hpp>

#include "GLFW/glfw3.h"

constexpr int kWindowWidth = 640;
constexpr int kWindowHeight = 480;

class Application {
private:
  bool is_running_ = false;
  GLFWwindow *window_ = nullptr;
  wgpu::Device device_ = nullptr;
  wgpu::Queue queue_ = nullptr;
  wgpu::Surface surface_ = nullptr;

  wgpu::TextureView GetNextTextureView();

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
