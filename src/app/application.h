//
// Created by Eli Michaud on 6/5/2026.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include <utility>

#include "GLFW/glfw3.h"
#include "util/device_adapter_util.h"

constexpr int kWindowWidth = 640;
constexpr int kWindowHeight = 480;

class Application {
private:
  using TextureViewPair = std::pair<WGPUSurfaceTexture, WGPUTextureView>;

  GLFWwindow *window_ = nullptr;
  WGPUDevice device_ = nullptr;
  WGPUQueue queue_ = nullptr;
  WGPUSurface surface_ = nullptr;

  TextureViewPair GetNextTextureView();

public:
  bool Init();
  void Terminate();
  void Tick();
  [[nodiscard]] bool ShouldContinue() const;

  ~Application() { Terminate(); }
};

#endif // APPLICATION_H
