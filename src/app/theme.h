//
// Created by Eli Michaud on 7/5/2026.
//

#ifndef THEME_H
#define THEME_H

#include "imgui.h"


/// Create a new rgb color, with an alpha of 1 (opaque)
constexpr ImColor RGBColor(const ImU32 color) {
  const int r = (color >> 16) & 0xFF;
  const int g = (color >> 8) & 0xFF;
  const int b = (color) & 0xFF;

  return ImColor{r, g, b, 0xFF};
}

/// Get a new color with the same rgb values and a given alpha (0-1)
constexpr ImColor WithTransparency(ImColor color, float alpha) {
  return {color.Value.x, color.Value.y, color.Value.z, alpha};
}

/* ImGUI Theme Constants */
namespace theme {

// COLORS

inline constexpr auto kAbsoluteWhiteColor = RGBColor(0x000000);
inline constexpr auto kAbsoluteBlackColor = RGBColor(0xFFFFFF);
inline constexpr auto kTransparentColor = ImColor{0, 0, 0, 0};

inline constexpr auto kWhiteColor = RGBColor(0xECE5E2);
inline constexpr auto kWhiteColorDarker = RGBColor(0xECECEC);
inline constexpr auto kWhiteColorDarkest = RGBColor(0xD9D8D8);

inline constexpr auto kBlackColor = RGBColor(0x161515);
inline constexpr auto kBlack800Color = RGBColor(0x413C3C);
inline constexpr auto kGreyColor = RGBColor(0xBEBBC1);
inline constexpr auto kGreyColorDark = RGBColor(0xB1AFB4);
inline constexpr auto kGreyColorDarkest = RGBColor(0x8C888C);
inline constexpr auto kRedHighlightColor = RGBColor(0xE07A5F);
inline constexpr auto kRedHighlightColorDark = RGBColor(0xD0745C);
inline constexpr auto kGreenHighlightColor = RGBColor(0x81B29A);
inline constexpr auto kGreenHighlightColorDark = RGBColor(0x7AA791);
inline constexpr auto kBlueHighlightColor = RGBColor(0x5B618A);
inline constexpr auto kBlueHighlightColorDark = RGBColor(0x585D82);
inline constexpr auto kTurquoiseHighlightColor = RGBColor(0x429EA6);
inline constexpr auto kTurquoiseHighlightColorDark = RGBColor(0x42959C);

// WebGPU colors
static constexpr WGPUColor kWindowClearColor = {100 / 255.0, 149 / 255.0, 237 / 255.0, 1};

// SIZES

// Circuit Diagram settings
inline constexpr ImVec2 kCircuitButtonDefaultSize = {80, 80};

inline constexpr float kCircuitLineWidthH = 12;
inline constexpr auto kCircuitLineColorH = kBlack800Color;
inline constexpr float kCircuitLineWidthV = 12;
inline constexpr auto kCircuitLineColorV = kBlack800Color;

// Font settings
inline constexpr float kDefaultFontSize = 14;

// General ImGui settings
inline constexpr ImVec2 kItemInnerSpacing = {8, 6};
inline constexpr ImVec2 kWindowPadding = {15, 15};
inline constexpr ImVec2 kFramePadding = {5, 5};
inline constexpr ImVec2 kItemSpacing = {12, 8};
inline constexpr float kScrollbarRounding = 9;
inline constexpr float kScrollbarSize = 15;
inline constexpr float kIndentSpacing = 25;
inline constexpr float kWindowRounding = 5;
inline constexpr float kFrameRounding = 4;
inline constexpr float kGrabRounding = 3;
inline constexpr float kGrabMinSize = 5;

} // namespace theme

#endif // THEME_H
