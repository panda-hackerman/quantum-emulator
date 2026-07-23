//
// Created by Eli Michaud on 6/5/2026.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include <webgpu/webgpu.hpp>

#include "../editor_windows/editor_window_manager.h"
#include "../resources/texture.h"
#include "device_window.h"
#include "simulator/state_vector.h"
#include "../editor_windows/circuit_info_processor.h"

/// The application (singleton)
class Application {
private:
  bool is_running_ = false;
  WGPUInstance instance_ = nullptr;
  WGPUDevice device_ = nullptr;
  WGPUQueue queue_ = nullptr;
  WGPUSurface surface_ = nullptr;
  WGPUTextureFormat preferred_format_ = WGPUTextureFormat_Undefined;

  DeviceWindow window_{&surface_};
  EditorWindowManager window_manager_{};
  TextureManager texture_manager_{};

  WGPUTextureView GetNextTextureView();

  void BeginFrame();
  void DrawFrame();
  void EndFrame();

  Application() = default;

public:
  Circuit circuit = {Circuit::kMinQubits, Circuit::kMinDepth};
  StateVector current_state_vector{};
  ComputedCircuitInfo current_circuit_info{};

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

  EditorWindowManager &GetWindowManager() { return window_manager_; }
  const TextureManager &GetTextureManager() { return texture_manager_; }

  /**
   * True if the program should continue running.
   * @return False if the GLFW window should close, otherwise true.
   */
  [[nodiscard]] bool ShouldContinue() const noexcept;

  /**
   * Build a surface config based on the current window dimensions.
   * Used when the window is resized
   * @param window The window
   * @return The WGPU surface configuration
   */
  [[nodiscard]] WGPUSurfaceConfiguration BuildSurfaceConfig(const DeviceWindow &window) const;
};

#endif // APPLICATION_H
