//
// Created by Eli Michaud on 6/22/2026.
//

#include "editor_window_manager.h"

#include <imgui.h>

#include <filesystem>

#include "custom_windows.h"
#include "imgui_internal.h"
#include "implot.h"
#include "quantum_circuit/circuit.h"

void EditorWindowManager::Init() {
  if (is_initialized_) return;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();

  SetImGuiStyle();

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

  SetupWindows();

  is_initialized_ = true;
}

void EditorWindowManager::Terminate() {
  if (is_initialized_) return;

  ImGui::DestroyContext();
  ImPlot::DestroyContext();

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
      // ImGui::DockBuilderDockWindow("Properties", dock_id_left_top); //TODO: Figure out layout
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

void EditorWindowManager::SetupWindows() {
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
      .on_draw = [&] { circuit_palette_.Draw(); },
      .can_close = false,
  });

  windows_.emplace_back(EditorWindow{
    .name = "Circuit Info",
    .on_draw = [&]{ circuit_info_.Draw(); },
    .can_close = false,
  });
}

void SetImGuiStyle() {
  ImGuiStyle &style = ImGui::GetStyle();
  ImGuiIO &io = ImGui::GetIO();

  // Font
  io.Fonts->AddFontDefaultVector(); // ProggyForever font
  io.Fonts->AddFontFromFileTTF(RESOURCE_DIR "/fonts/cmr10.ttf", 10);

  // Default dark colors
  ImGui::StyleColorsDark();

  // DPI Aware
  io.ConfigDpiScaleFonts = true;
  io.ConfigDpiScaleViewports = true;

  // Rounding values
  constexpr float rounding = 8.f;
  style.TabRounding = rounding;
  style.FrameRounding = rounding;
  style.GrabRounding = rounding;
  style.WindowRounding = rounding;
  style.PopupRounding = rounding;
}
