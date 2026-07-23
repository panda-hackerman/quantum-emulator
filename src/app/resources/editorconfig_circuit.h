//
// Created by Eli Michaud on 7/21/2026.
//

#ifndef EDITORCONFIG_CIRCUIT_H
#define EDITORCONFIG_CIRCUIT_H

#include <algorithm>

#include "imgui_internal.h"

namespace editorconfig::circuit {

namespace internal {
#ifndef _MSC_VER
inline auto scan_func = sscanf;
#else
inline auto scan_func = sscanf_s;
#endif
} // namespace internal

inline constexpr const char *kTypeName = "CircuitEditor";

static void *ReadOpen(ImGuiContext *, ImGuiSettingsHandler *, const char *) {
  editor::circuit::ClearEditor();
  return nullptr;
}

static void ReadLine(ImGuiContext *, ImGuiSettingsHandler *, void *, const char *line) {
  if (line == nullptr || line[0] == '\0') return;

  int num_qubits;
  int num_layers;

  int qubit;
  int layer;
  char button_name[50] = {};

  // "Evil" use of sscanf but input streams make this 1 million times more annoying.
  if (internal::scan_func(line, "Size=%d,%d", &num_qubits, &num_layers)) {
    editor::circuit::gui_data.num_qubits = num_qubits;
    editor::circuit::gui_data.num_layers = num_layers;
    editor::circuit::UpdateCircuitSize();
  } else if (internal::scan_func(line, "(q%d,t%d)=%50c", &qubit, &layer, button_name, 50)) {
    const Circuit::GridSize_T qubit_idx = static_cast<Circuit::GridSize_T>(qubit);
    const Circuit::GridSize_T layer_idx = static_cast<Circuit::GridSize_T>(layer);

    // Find first gate that matches string
    const auto button_itr =
        std::ranges::find_if(gate::kKnownGates, [&button_name](const GateButton *other) {
          return std::strcmp(button_name, other->name) == 0;
        });

    if (button_itr != gate::kKnownGates.end()) {
      editor::circuit::SetButton(qubit_idx, layer_idx, *button_itr);
    } else {
      std::cerr << "While parsing: Unknown gate: \"" << button_name << "\"!";
    }
  }
}

static void WriteAll(ImGuiContext *, ImGuiSettingsHandler *handler, ImGuiTextBuffer *buf) {
  const auto num_qubits = static_cast<Circuit::GridSize_T>(editor::circuit::gui_data.num_qubits);
  const auto num_layers = static_cast<Circuit::GridSize_T>(editor::circuit::gui_data.num_layers);

  buf->appendf("[%s][%s]\n", handler->TypeName, kTypeName);

  if (num_qubits >= Circuit::kMinQubits && num_layers >= Circuit::kMinDepth) {
    buf->appendf("Size=%d,%d\n", num_qubits, num_layers);
  }

  for (Circuit::GridSize_T qubit = 0; qubit < Circuit::kMaxQubits; ++qubit) {
    for (Circuit::GridSize_T layer = 0; layer < Circuit::kMaxDepth; ++layer) {
      const GateButton *button = editor::circuit::editor_data.buttons_arr[qubit][layer];

      if (button == nullptr || button->part == Circuit::Part::kEmpty || button->name == nullptr ||
          button->name[0] == '\0')
        continue;

      buf->appendf("(q%d,t%d)=%s\n", qubit, layer, button->name);
    }
  }
}

static void ClearAll(ImGuiContext *, ImGuiSettingsHandler *) {
  editor::circuit::ClearEditor();
}

inline ImGuiSettingsHandler BuildHandler() {

  ImGuiSettingsHandler handler;

  handler.TypeName = kTypeName;
  handler.TypeHash = ImHashStr(kTypeName);
  handler.ClearAllFn = ClearAll;
  handler.ReadOpenFn = ReadOpen;
  handler.ReadLineFn = ReadLine;
  handler.WriteAllFn = WriteAll;

  return handler;
}
} // namespace editorconfig::circuit

#endif // EDITORCONFIG_CIRCUIT_H
