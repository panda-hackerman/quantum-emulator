//
// Created by Eli Michaud on 7/21/2026.
//

#ifndef EDITORCONFIG_CIRCUIT_H
#define EDITORCONFIG_CIRCUIT_H

#include <algorithm>
#include <utility>

#include "../application/application.h"
#include "imgui_internal.h"

namespace editorconfig::circuit {

namespace internal {
#ifndef _MSC_VER
#define SCAN_FUNC sscanf
// inline auto scan_func = sscanf;
#else
#define SCAN_FUNC sscanf_s
// inline auto scan_func = sscanf_s;
#endif
} // namespace internal

inline constexpr const char *kTypeName = "CircuitEditor";

static void *ReadOpen(ImGuiContext *, ImGuiSettingsHandler *, const char *) {
  // Assuming there's only ever one circuit
  return &Application::Instance().GetWindowManager().GetCircuitWindow();
}

static void ReadLine(ImGuiContext *, ImGuiSettingsHandler *, void *entry, const char *line) {
  if (line[0] == '\0') return;

  CircuitEditor &editor = *reinterpret_cast<CircuitEditor *>(entry);

  int num_qubits;
  int num_layers;

  int qubit;
  int layer;
  char button_name[50] = {};

  // "Evil" use of sscanf but input streams make this 1 million times more annoying.
  if (sscanf_s(line, "NumQubits=%d", &num_qubits)) {
    editor.data.num_qubits = num_qubits;
    editor.UpdateCircuitSize();
  } else if (sscanf_s(line, "NumLayers=%d", &num_layers)) {
    editor.data.num_layers = num_layers;
    editor.UpdateCircuitSize();
  } else if (sscanf_s(line, "(q%d,t%d)=%50c", &qubit, &layer, button_name, 50)) {
    std::string str = button_name;

    const auto button_itr = std::ranges::find_if(
        gate::kKnownGates, [&str](const GateButton *other) { return str == other->name; });

    if (button_itr != gate::kKnownGates.end()) {
      editor.Set(static_cast<Circuit::GridSize_T>(qubit), static_cast<Circuit::GridSize_T>(layer),
                 *button_itr);
    }
  }
}

static void WriteAll(ImGuiContext *, ImGuiSettingsHandler *handler, ImGuiTextBuffer *buf) {
  const CircuitEditor &editor = Application::Instance().GetWindowManager().GetCircuitWindow();

  buf->appendf("[%s][%s]\n", handler->TypeName, kTypeName);

  if (std::cmp_greater_equal(editor.data.num_qubits, Circuit::kMinQubits)) {
    buf->appendf("NumQubits=%d\n", editor.data.num_qubits);
  }

  if (std::cmp_greater_equal(editor.data.num_layers, Circuit::kMinDepth)) {
    buf->appendf("NumLayers=%d\n", editor.data.num_layers);
  }

  for (Circuit::GridSize_T qubit = 0; qubit < Circuit::kMaxQubits; ++qubit) {
    for (Circuit::GridSize_T layer = 0; layer < Circuit::kMaxDepth; ++layer) {
      const GateButton *button = editor.Get(qubit, layer);
      if (button->part == Circuit::Part::kEmpty || button->name[0] == '\0') continue;

      buf->appendf("(q%d,t%d)=%s\n", qubit, layer, button->name);
    }
  }
}

inline ImGuiSettingsHandler BuildHandler() {

  ImGuiSettingsHandler handler;

  handler.TypeName = kTypeName;
  handler.TypeHash = ImHashStr(kTypeName);
  handler.ReadOpenFn = ReadOpen;
  handler.ReadLineFn = ReadLine;
  handler.WriteAllFn = WriteAll;

  return handler;
}
} // namespace editorconfig::circuit

#endif // EDITORCONFIG_CIRCUIT_H
