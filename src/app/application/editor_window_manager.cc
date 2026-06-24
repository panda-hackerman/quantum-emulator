//
// Created by Eli Michaud on 6/22/2026.
//

#include "editor_window_manager.h"

#include <imgui.h>

#include <filesystem>

#include "quantum_circuit/circuit.h"

void EditorWindowManager::Init() {
  if (is_initialized_) return;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  if constexpr (kImGuiIniPath != nullptr) {
    if (!std::filesystem::exists(kImGuiIniPath)) {
      ImGui::SaveIniSettingsToDisk(kImGuiIniPath);
    }
  }

  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = kImGuiIniPath;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

#ifdef __EMSCRIPTEN__
  io.MouseDrawCursor = true; // Show mouse cursor on web platform
#endif

  circuit.SetCircuitPart(0, 0, gates::kTGate);

  ResetWindows();

  is_initialized_ = true;
}

void EditorWindowManager::Terminate() {
  if (is_initialized_) return;

  ImGui::DestroyContext();

  is_initialized_ = false;
}

void EditorWindowManager::DrawWindows() {
  for (EditorWindow &window : windows_) {
    if (!window.open) continue;

    bool *is_open = window.can_close ? &window.open : nullptr;

    ImGui::Begin(window.name, is_open, window.flags);

    try {
      window.on_draw();
    } catch (const std::exception &e) {
      throw std::runtime_error("Ran into an error while drawing window '" +
                               std::string(window.name) + "': " + e.what());
    } catch (...) {
      throw std::runtime_error("Ran into an unexpected error while drawing window: " +
                               std::string(window.name));
    }

    ImGui::End();
  }
}

constexpr void EditorWindowManager::ResetWindows() {
  windows_.clear();

  windows_.emplace_back(EditorWindow{
      "Hello, World!",
      [] {
        static float f = 0.0f;
        static int counter = 0;
        static bool show_demo_window = true;
        static bool show_another_window = false;
        static auto clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        ImGui::Text("This is some useful text.");
        ImGui::Checkbox("Demo Window", &show_demo_window);
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        ImGui::ColorEdit3("clear color", reinterpret_cast<float *>(&clear_color));

        if (ImGui::Button("Button")) counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        const ImGuiIO &io = ImGui::GetIO();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate,
                    io.Framerate);
      },
  });

  windows_.emplace_back(EditorWindow{
      .name = "Circuit Diagram",
      .on_draw =
          [&] {
            static int num_qubits = 3;
            static int num_layers = 3;

            ImGui::PushItemWidth(100);
            ImGui::InputInt("Num Qubits", &num_qubits);
            ImGui::SameLine();
            ImGui::InputInt("Num Layers", &num_layers);

            if (num_qubits < gates::kMinCircuitQubits) {
              num_qubits = gates::kMinCircuitQubits;
            } else if (num_qubits > gates::kMaxCircuitQubits) {
              num_qubits = gates::kMaxCircuitQubits;
            }

            if (num_layers < gates::kMinCircuitDepth) {
              num_layers = gates::kMinCircuitDepth;
            } else if (num_layers > gates::kMaxCircuitDepth) {
              num_layers = gates::kMaxCircuitDepth;
            }

            const Circuit::GridSize_T q = static_cast<Circuit::GridSize_T>(num_qubits);
            const Circuit::GridSize_T l = static_cast<Circuit::GridSize_T>(num_layers);

            if (num_qubits != circuit.NumQubits() || num_layers != circuit.CircuitDepth()) {
              circuit.SetNewSize(q, l);
            }

            if (ImGui::BeginTable("Circuit Diagram", num_layers, ImGuiTableFlags_Borders)) {
              for (Circuit::GridSize_T qubit = 0; qubit < num_qubits; ++qubit) {
                ImGui::TableNextRow();
                for (Circuit::GridSize_T layer = 0; layer < num_layers; ++layer) {

                  ImGui::TableSetColumnIndex(layer);

              // const gates::GateID id = circuit.GetIdAt(qubit, layer);

                  const CircuitPart &part = circuit.GetCircuitPart(qubit, layer);

                  ImGui::Text("%s", part.name);
                }
              }

              ImGui::EndTable();
            }
          },
      .can_close = false,
  });
}
