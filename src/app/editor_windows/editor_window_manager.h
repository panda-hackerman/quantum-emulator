//
// Created by Eli Michaud on 6/22/2026.
//

#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <webgpu/webgpu.hpp>
#include "custom_windows.h"

struct EditorWindow {
  const char *name;
  std::function<void()> on_draw;
  ImGuiWindowFlags flags = ImGuiWindowFlags_None;
  bool can_close = true;
  bool open = true;
};

class EditorWindowManager {
private:
  std::vector<EditorWindow> windows_;

  bool is_initialized_ = false;
  bool circuit_info_dirty_ = true;

public:
  void Init();
  void SetupWindows();
  void DrawWindows();
  void Terminate();

  [[nodiscard]] bool IsInitialized() const { return  is_initialized_; }

  ~EditorWindowManager() { Terminate(); }
};

void SetImGuiStyle();

#endif // WINDOW_MANAGER_H
