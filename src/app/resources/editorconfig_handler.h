//
// Created by Eli Michaud on 7/7/2026.
//

#ifndef EDITORCONFIG_HANDLER_H
#define EDITORCONFIG_HANDLER_H

#ifndef __EMSCRIPTEN__
#include <filesystem>
#include <iostream>

#include "editorconfig_circuit.h"
#include "imgui.h"
#include "imgui_internal.h"
#endif // __EMSCRIPTEN__

/// Editor Config (i.e. imgui.ini, by default) settings
namespace editorconfig {

#ifndef __EMSCRIPTEN__
inline constexpr const char *kPath = "data/editorconfig.ini"; ///< Relative ath to editorconfig file
inline constexpr bool kCircuitPersistsAfterReload = true; ///< True if circuit settings are saved
#else  // __EMSCRIPTEN__
inline constexpr const char *kPath = nullptr; // No ini file on web...
inline constexpr bool kCircuitPersistsAfterReload = false;
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
#ifndef __EMSCRIPTEN__
  // Add custom settings
  if constexpr (kCircuitPersistsAfterReload) {
    const ImGuiSettingsHandler circuit_handler = circuit::BuildHandler();
    ImGui::AddSettingsHandler(&circuit_handler);
  }
#endif

  // Create folders for file path
  InitFilePath();
}

inline void SaveToDiskManual() {
#ifndef __EMSCRIPTEN__
  const ImGuiContext *ctx = ImGui::GetCurrentContext();
  const ImGuiIO *io = &ImGui::GetIO();

  const auto filename = io->IniFilename;

  if (ctx->SettingsLoaded && io->IniFilename != nullptr) {
    ImGui::SaveIniSettingsToDisk(filename);
  }
#endif
}

} // namespace editorconfig

#endif // EDITORCONFIG_HANDLER_H
