//
// Created by Eli Michaud on 7/7/2026.
//

#ifndef EDITORCONFIG_HANDLER_H
#define EDITORCONFIG_HANDLER_H

#ifndef __EMSCRIPTEN__
#include <filesystem>
#include <iostream>

#include "imgui.h"

static constexpr const char *kImGuiIniPath = "data/editorconfig.ini";
#else  // __EMSCRIPTEN__
static constexpr const char *kImGuiIniPath = nullptr; // No ini file on web...
#endif // __EMSCRIPTEN__

inline bool InitFilePath() {
#ifndef __EMSCRIPTEN__
  namespace fs = std::filesystem;

  try {
    const fs::path relative_path(kImGuiIniPath);
    fs::path absolute_path = absolute(relative_path);
    absolute_path.remove_filename();

    if (!fs::exists(absolute_path)) {
      fs::create_directories(absolute_path);
      ImGui::SaveIniSettingsToDisk(kImGuiIniPath);
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

#endif // EDITORCONFIG_HANDLER_H
