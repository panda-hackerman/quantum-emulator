//
// Created by Eli Michaud on 6/22/2026.
//

#include "editor_window_manager.h"

#include <imgui.h>

#include <filesystem>

#include "custom_windows.h"
#include "imgui_internal.h"
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

  ResetWindows();

  is_initialized_ = true;
}

void EditorWindowManager::Terminate() {
  if (is_initialized_) return;

  ImGui::DestroyContext();

  is_initialized_ = false;
}

void EditorWindowManager::DrawWindows() {

  /* SETUP DOCKING*/ {
    const ImGuiID dock_space_id = ImGui::GetID("Docking Space");
    const ImGuiViewport *viewport = ImGui::GetMainViewport();

    if (ImGui::DockBuilderGetNode(dock_space_id) == nullptr) {
      ImGui::DockBuilderAddNode(dock_space_id, ImGuiDockNodeFlags_DockSpace);
      ImGui::DockBuilderSetNodeSize(dock_space_id, viewport->Size);

      ImGuiID dock_id_left = 0;
      ImGuiID dock_id_main = dock_space_id;
      ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Left, 0.20f, &dock_id_left, &dock_id_main);
      ImGuiID dock_id_left_top = 0;
      ImGuiID dock_id_left_bottom = 0;
      ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Up, 0.50f, &dock_id_left_top,
                                  &dock_id_left_bottom);
      ImGui::DockBuilderDockWindow("Circuit Diagram", dock_id_main);
      // ImGui::DockBuilderDockWindow("Properties", dock_id_left_top);
      ImGui::DockBuilderDockWindow("Circuits", dock_id_left_bottom);
      ImGui::DockBuilderFinish(dock_space_id);
    }

    ImGui::DockSpaceOverViewport(dock_space_id, viewport, ImGuiDockNodeFlags_PassthruCentralNode);
  }

  for (EditorWindow &window : windows_) {
    if (!window.open) continue;

    bool *is_open = window.can_close ? &window.open : nullptr;
    ImGui::Begin(window.name, is_open, window.flags);

    try {
      std::invoke(window.on_draw);
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

void EditorWindowManager::ResetWindows() {
  windows_.clear();
  circuit_ = Circuit::BuildExampleCircuit();

  // Build windows
  windows_.emplace_back(EditorWindow{
      .name = "Circuit Diagram",
      .on_draw = [&] { circuit_window_.Draw(); },
      .flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize,
      .can_close = false,
  });

  windows_.emplace_back(EditorWindow{
      .name = "Circuits",
      .on_draw = [] {
            constexpr std::size_t kNumElems = gates::kIdToGateMap.Size();
            constexpr ImVec2 kButtonSize{60, 60};

            const ImGuiStyle &style = ImGui::GetStyle();
            const float window_visible_x2 = ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x;

            int i = 0;
            for (const auto &[id, part] : gates::kIdToGateMap) {
              ImGui::PushID(id);

              const char *name = part.name == nullptr ? "" : part.name;
              ImGui::Button(name, kButtonSize);

              const float last_button_x2 = ImGui::GetItemRectMax().x;
              const float next_button_x2 = last_button_x2 + style.ItemSpacing.x + kButtonSize.x;

              if (++i < kNumElems && next_button_x2 < window_visible_x2) {
                ImGui::SameLine();
              }

              ImGui::PopID();
            }
          },
      .can_close = false,
  });
}
