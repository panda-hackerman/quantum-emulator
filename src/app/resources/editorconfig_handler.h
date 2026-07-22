//
// Created by Eli Michaud on 7/7/2026.
//

#ifndef EDITORCONFIG_HANDLER_H
#define EDITORCONFIG_HANDLER_H

#ifndef __EMSCRIPTEN__
#include <filesystem>
#include <iostream>

#include "imgui.h"
#endif // __EMSCRIPTEN__
#include "editorconfig_circuit.h"
#include "imgui_internal.h"

/// Editor Config (i.e. imgui.ini, by default) settings
namespace editorconfig {

#ifndef __EMSCRIPTEN__
static constexpr const char *kPath = "data/editorconfig.ini";
#else  // __EMSCRIPTEN__
static constexpr const char *kPath = nullptr; // No ini file on web...
#endif // __EMSCRIPTEN__

inline bool InitFilePath() {
#ifndef __EMSCRIPTEN__
  namespace fs = std::filesystem;

  try {
    const fs::path relative_path(kPath);
    fs::path absolute_path = absolute(relative_path);
    absolute_path.remove_filename();

    if (!fs::exists(absolute_path)) {
      fs::create_directories(absolute_path);
      ImGui::SaveIniSettingsToDisk(kPath);
    }

    return true;
  } catch (std::exception &ex) {
    std::cerr << "Couldn't create editor config directory: " << ex.what();
    return false;
  }
#else
  return true;
#endif
}

inline void Init() {

  // Add custom settings
  const ImGuiSettingsHandler circuit_handler = circuit::BuildHandler();
  ImGui::AddSettingsHandler(&circuit_handler);

  // Create folders for file path
  InitFilePath();
}

inline void SaveToDiskManual() {
  const ImGuiContext *ctx = ImGui::GetCurrentContext();
  const ImGuiIO *io = &ImGui::GetIO();

  const auto filename = io->IniFilename;

  if (ctx->SettingsLoaded && io->IniFilename != nullptr) {
    ImGui::SaveIniSettingsToDisk(filename);
  }
}

} // namespace econfig

#endif // EDITORCONFIG_HANDLER_H
