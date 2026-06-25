//
// Created by Eli Michaud on 6/22/2026.
//

#include "editor_window_manager.h"

#include <imgui.h>

#include <filesystem>
#include <utility>

#include "quantum_circuit/circuit.h"

void EditorWindowManager::Init() {
  if (is_initialized_) return;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  // TODO: Move to dedicated style builder/ file
  ImGuiStyle &style = ImGui::GetStyle();
  style.TabRounding = 8.f;
  style.FrameRounding = 8.f;
  style.GrabRounding = 8.f;
  style.WindowRounding = 8.f;
  style.PopupRounding = 8.f;

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

  circuit_.SetCircuitPart(0, 0, gates::kTGate);

  ResetWindows();

  is_initialized_ = true;
}

void EditorWindowManager::Terminate() {
  if (is_initialized_) return;

  ImGui::DestroyContext();

  is_initialized_ = false;
}

void EditorWindowManager::DrawWindows() {

  // ImGui::ShowDemoWindow();

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
      .name = "Circuit Diagram",
      .on_draw =
          [&] {
            static int num_qubits = 3;
            static int num_layers = 3;

            ImGui::PushItemWidth(100);
            ImGui::InputInt("Num Qubits", &num_qubits);
            ImGui::SameLine();
            ImGui::InputInt("Num Layers", &num_layers);

            // Update Size
            num_qubits =
                std::clamp<int>(num_qubits, gates::kMinCircuitQubits, gates::kMaxCircuitQubits);
            num_layers =
                std::clamp<int>(num_layers, gates::kMinCircuitDepth, gates::kMaxCircuitDepth);

            if (std::cmp_not_equal(num_qubits, circuit_.NumQubits()) ||
                std::cmp_not_equal(num_layers, circuit_.CircuitDepth())) {
              circuit_.SetNewSize(static_cast<Circuit::GridSize_T>(num_qubits),
                                  static_cast<Circuit::GridSize_T>(num_layers));
            }

            // Table
            constexpr ImGuiTableFlags table_flags = ImGuiTableFlags_Borders |
                                                    ImGuiTableFlags_NoHostExtendX |
                                                    ImGuiTableFlags_SizingFixedFit;

            // Actually draw
            if (ImGui::BeginTable("Circuit Diagram", num_layers + 1, table_flags)) {
              // Setup Columns
              ImGui::TableSetupColumn(
                  "", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoResize);

              for (Circuit::GridSize_T i = 0; std::cmp_less(i, num_layers); ++i) {
                ImGui::TableSetupColumn(
                    std::format("Layer {}", i).data(),
                    ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoResize);
              }

              ImGui::TableHeadersRow();
              // Loop over actual grid
              for (Circuit::GridSize_T qubit = 0; std::cmp_less(qubit, num_qubits); ++qubit) {
                ImGui::PushID(qubit);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::AlignTextToFramePadding();
                ImGui::Text("q%d", qubit);

                for (Circuit::GridSize_T l = 1; std::cmp_less_equal(l, num_layers); ++l) {
                  const Circuit::GridSize_T layer = l - 1;

                  ImGui::PushID(layer);

                  ImGui::TableSetColumnIndex(l);

                  const int button_id = layer + (num_layers * qubit);
                  const CircuitPart &part = circuit_.GetCircuitPart(qubit, layer);

                  const char *name = part.name == nullptr ? "" : part.name;
                  const std::string label = std::format("{}##{}", name, button_id);
                  ImGui::Button(label.data(), ImVec2{60, 60});
                  ImGui::PopID();
                }
                ImGui::PopID();
              }

              ImGui::EndTable();
            }
          },
      .flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize |
               ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize,
      .can_close = false,
  });
}
