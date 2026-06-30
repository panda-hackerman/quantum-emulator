//
// Created by Eli Michaud on 6/5/2026.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include <webgpu/webgpu.hpp>

#include "../editor_windows/editor_window_manager.h"
#include "device_window.h"

constexpr WGPUColor kWindowClearColor = {100 / 255.0, 149 / 255.0, 237 / 255.0, 1};

/// The application (singleton)
class Application {
private:
  bool is_running_ = false;
  WGPUInstance instance_ = nullptr;
  WGPUDevice device_ = nullptr;
  WGPUQueue queue_ = nullptr;
  WGPUSurface surface_ = nullptr;
  DeviceWindow window_{&surface_};
  EditorWindowManager window_manager_{};

  WGPUTextureFormat preferred_format_ = WGPUTextureFormat_Undefined;

  void BeginFrame();
  void DrawFrame();
  void EndFrame();

  WGPUTextureView GetNextTextureView();

  Application() = default;

public:
  bool Init(); ///< Called on initialization
  void Terminate(); ///< Terminates all processes (called automatically in destructor)
  void Tick(); ///< Called every update

  ~Application() { Terminate(); }

  Application(Application const &) = delete;
  void operator=(Application const &) = delete;

  static Application &Instance() {
    static Application instance;
    return instance;
  }

  /**
   * True if the program should continue running.
   * @return False if the GLFW window should close, otherwise true.
   */
  [[nodiscard]] bool ShouldContinue() const noexcept;
  [[nodiscard]] WGPUSurfaceConfiguration BuildSurfaceConfig(const DeviceWindow &window) const;
};

#endif // APPLICATION_H
