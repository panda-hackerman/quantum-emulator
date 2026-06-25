//
// Created by Eli Michaud on 6/22/2026.
//

#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <webgpu/webgpu.hpp>

#include "custom_windows.h"
#include "imgui.h"
#include "quantum_circuit/circuit.h"

struct EditorWindow {
  const char *name;
  std::function<void()> on_draw;
  ImGuiWindowFlags flags = ImGuiWindowFlags_None;
  bool can_close = true;
  bool open = true;
};

class EditorWindowManager {
private:
  bool is_initialized_ = false;
  std::vector<EditorWindow> windows_;

  Circuit circuit_ = Circuit::BuildExampleCircuit();
  CircuitEditorWindow circuit_window_{.circuit = &circuit_,
                                      .data = {
                                          .num_qubits = circuit_.NumQubits(),
                                          .num_layers = circuit_.CircuitDepth(),
                                      }};

public:
#ifndef __ESCRIPTEN__
  static constexpr const char *kImGuiIniPath = "data/layouts/imgui.ini";
#else
  static constexpr const char *kImGuiIniPath = nullptr; // No ini file on web...
#endif

  void Init();
  void ResetWindows();
  void DrawWindows();
  void Terminate();

  ~EditorWindowManager() { Terminate(); }
};

#endif // WINDOW_MANAGER_H
