//
// Created by Eli Michaud on 6/22/2026.
//

#include "editor_window_manager.h"

#include <imgui.h>

#include "../application/application.h"
#include "../resources/editorconfig_handler.h"
#include "../theme.h"
#include "editor_windows.h"
#include "imgui_internal.h"
#include "implot.h"

void EditorWindowManager::Init() {
  if (is_initialized_) return;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();

  SetImGuiStyle();

  editorconfig::Init();

  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = editorconfig::kPath;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

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

  // Stuff to hide or disable tab bar on docking windows
  // ImGuiWindowClass hide_tab_bar;
  // hide_tab_bar.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_HiddenTabBar;
  //
  // ImGuiWindowClass no_tab_bar;
  // no_tab_bar.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;

  /* SETUP DOCKING*/ {
    const ImGuiID dock_space_id = ImGui::GetID("Docking Space");
    const ImGuiViewport *viewport = ImGui::GetMainViewport();

    if (ImGui::DockBuilderGetNode(dock_space_id) == nullptr) {

      ImGui::DockBuilderAddNode(dock_space_id, ImGuiDockNodeFlags_DockSpace);
      ImGui::DockBuilderSetNodeSize(dock_space_id, viewport->Size);

      ImGuiID dock_id_left = 0;
      ImGuiID dock_id_main_right = 0;

      ImGuiID dock_id_left_top = 0;
      ImGuiID dock_id_left_bottom = 0;

      ImGui::DockBuilderSplitNode(dock_space_id, ImGuiDir_Left, 0.31f, &dock_id_left,
                                  &dock_id_main_right);
      ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Up, 0.45f, &dock_id_left_top,
                                  &dock_id_left_bottom);

      ImGui::DockBuilderDockWindow(editor::circuit::main_window.name, dock_id_main_right);
      ImGui::DockBuilderDockWindow(editor::circuit::palette_window.name, dock_id_left_top);
      ImGui::DockBuilderDockWindow(editor::circuit::info_window.name, dock_id_left_bottom);

      ImGui::DockBuilderFinish(dock_space_id);
    }

    ImGui::DockSpaceOverViewport(dock_space_id, viewport, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingOverCentralNode);
  }

  for (EditorWindow &window : windows_) {
    if (!window.open) continue;

    ImGuiWindowClass window_class;
    // window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoDockingOverCentralNode;

    if (window.no_tab_bar) {
      window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoTabBar;
    } else if (window.hide_tab_bar) {
      window_class.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_AutoHideTabBar;
    }

    ImGui::SetNextWindowClass(&window_class);

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

  // ImGui::ShowDemoWindow();
  // ImGui::ShowDebugLogWindow();
}

void EditorWindowManager::SetupWindows() {
  windows_.clear();

  editor::circuit::SetExampleCircuit();

  // Build windows
  windows_.emplace_back(editor::circuit::main_window);
  windows_.emplace_back(editor::circuit::info_window);
  windows_.emplace_back(editor::circuit::palette_window);
}

void SetImGuiStyle() {
  ImGuiIO *io = &ImGui::GetIO();
  ImGuiStyle *style = &ImGui::GetStyle();
  ImVec4 *colors = style->Colors;

  // Font
  io->Fonts->AddFontFromFileTTF(theme::kFontPath, theme::kDefaultFontSize);

  // DPI Aware
  io->ConfigDpiScaleFonts = true;
  io->ConfigDpiScaleViewports = true;

  // Set Theme
  ImGui::StyleColorsLight();

  style->WindowPadding = theme::kWindowPadding;
  style->WindowRounding = theme::kWindowRounding;
  style->FramePadding = theme::kFramePadding;
  style->FrameRounding = theme::kFrameRounding;
  style->ItemSpacing = theme::kItemSpacing;
  style->ItemInnerSpacing = theme::kItemInnerSpacing;
  style->IndentSpacing = theme::kIndentSpacing;
  style->ScrollbarSize = theme::kScrollbarSize;
  style->ScrollbarRounding = theme::kScrollbarRounding;
  style->GrabMinSize = theme::kGrabMinSize;
  style->GrabRounding = theme::kGrabRounding;

  // Set Colors
  colors[ImGuiCol_Text] = theme::kBlackColor;
  colors[ImGuiCol_TextDisabled] = WithTransparency(theme::kBlackColor, 0.60f);

  colors[ImGuiCol_WindowBg] = theme::kWhiteColor;
  colors[ImGuiCol_ChildBg] = theme::kTransparentColor;
  colors[ImGuiCol_PopupBg] = WithTransparency(theme::kWhiteColor, 0.98f);

  colors[ImGuiCol_Border] = WithTransparency(theme::kBlackColor, 0.30f);
  colors[ImGuiCol_BorderShadow] = theme::kTransparentColor;

  colors[ImGuiCol_FrameBg] = WithTransparency(theme::kBlack800Color, 0.40f);
  colors[ImGuiCol_FrameBgHovered] = WithTransparency(theme::kGreenHighlightColor, 0.40f);
  colors[ImGuiCol_FrameBgActive] = WithTransparency(theme::kGreenHighlightColor, 0.67f);

  colors[ImGuiCol_TitleBg] = theme::kGreyColorDark;
  colors[ImGuiCol_TitleBgActive] = theme::kGreyColor;
  colors[ImGuiCol_TitleBgCollapsed] = WithTransparency(theme::kBlackColor, 0.51f);
  colors[ImGuiCol_MenuBarBg] = WithTransparency(theme::kBlack800Color, 0.47f);

  colors[ImGuiCol_ScrollbarBg] = theme::kWhiteColorDarkest;
  colors[ImGuiCol_ScrollbarGrab] = theme::kGreyColor;
  colors[ImGuiCol_ScrollbarGrabHovered] = WithTransparency(theme::kBlueHighlightColor, 0.40f);
  colors[ImGuiCol_ScrollbarGrabActive] = WithTransparency(theme::kBlueHighlightColorDark, 0.70f);

  colors[ImGuiCol_CheckMark] = theme::kGreenHighlightColor;
  colors[ImGuiCol_CheckboxSelectedBg] = theme::kGreyColorDark;

  colors[ImGuiCol_SliderGrab] = WithTransparency(theme::kBlueHighlightColor, 0.78f);
  colors[ImGuiCol_SliderGrabActive] = WithTransparency(theme::kBlueHighlightColorDark, 0.60f);

  colors[ImGuiCol_Button] = WithTransparency(theme::kBlueHighlightColor, 0.30f);
  colors[ImGuiCol_ButtonHovered] = WithTransparency(theme::kBlueHighlightColor, 0.60f);
  colors[ImGuiCol_ButtonActive] = WithTransparency(theme::kBlueHighlightColorDark, 0.70f);

  colors[ImGuiCol_Header] = WithTransparency(theme::kBlueHighlightColor, 0.20f);
  colors[ImGuiCol_HeaderHovered] = WithTransparency(theme::kBlueHighlightColor, 0.40f);
  colors[ImGuiCol_HeaderActive] = WithTransparency(theme::kBlueHighlightColorDark, 0.70f);

  colors[ImGuiCol_Separator] = theme::kGreyColorDarkest;
  colors[ImGuiCol_SeparatorHovered] = WithTransparency(theme::kBlueHighlightColor, 0.57f);
  colors[ImGuiCol_SeparatorActive] = WithTransparency(theme::kBlueHighlightColorDark, 0.60f);

  colors[ImGuiCol_ResizeGrip] = theme::kGreyColorDark;
  colors[ImGuiCol_ResizeGripHovered] = WithTransparency(theme::kBlueHighlightColor, 0.30f);
  colors[ImGuiCol_ResizeGripActive] = WithTransparency(theme::kBlueHighlightColorDark, 0.40f);

  colors[ImGuiCol_InputTextCursor] = colors[ImGuiCol_Text];

  // clang-format off
  colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
  colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.90f);
  colors[ImGuiCol_TabSelected] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
  colors[ImGuiCol_TabSelectedOverline] = colors[ImGuiCol_HeaderActive];
  colors[ImGuiCol_TabDimmed] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
  colors[ImGuiCol_TabDimmedSelected] = ImLerp(colors[ImGuiCol_TabSelected], colors[ImGuiCol_TitleBg], 0.40f);
  colors[ImGuiCol_TabDimmedSelectedOverline] = WithTransparency(theme::kBlueHighlightColor, 0.0f);
  // clang-format on

  colors[ImGuiCol_DockingPreview] = WithTransparency(theme::kBlueHighlightColor, 0.14f);
  colors[ImGuiCol_DockingEmptyBg] = theme::kGreyColorDarkest;

  colors[ImGuiCol_PlotLines] = theme::kGreyColorDarkest;
  colors[ImGuiCol_PlotLinesHovered] = theme::kRedHighlightColor;
  colors[ImGuiCol_PlotHistogram] = theme::kTurquoiseHighlightColor;
  colors[ImGuiCol_PlotHistogramHovered] = theme::kRedHighlightColorDark;

  colors[ImGuiCol_TableHeaderBg] = theme::kWhiteColorDarkest;
  colors[ImGuiCol_TableBorderStrong] = theme::kGreyColorDarkest;
  colors[ImGuiCol_TableBorderLight] = theme::kGreyColorDark;
  colors[ImGuiCol_TableRowBg] = theme::kTransparentColor;
  colors[ImGuiCol_TableRowBgAlt] = WithTransparency(theme::kGreyColorDark, 0.09f);

  colors[ImGuiCol_TextLink] = theme::kBlueHighlightColor;
  colors[ImGuiCol_TextSelectedBg] = theme::kBlueHighlightColorDark;

  colors[ImGuiCol_TreeLines] = colors[ImGuiCol_Border];
  colors[ImGuiCol_DragDropTarget] = theme::kGreenHighlightColorDark;
  colors[ImGuiCol_DragDropTargetBg] = WithTransparency(theme::kGreenHighlightColor, 0.20f);
  colors[ImGuiCol_UnsavedMarker] = theme::kAbsoluteWhiteColor;

  colors[ImGuiCol_NavCursor] = colors[ImGuiCol_HeaderHovered];
  colors[ImGuiCol_NavWindowingHighlight] = WithTransparency(theme::kGreyColorDark, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg] = WithTransparency(theme::kGreyColor, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg] = WithTransparency(theme::kGreyColor, 0.35f);
}
