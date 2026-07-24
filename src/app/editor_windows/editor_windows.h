//
// Created by Eli Michaud on 6/25/2026.
//

#ifndef EDITOR_WINDOWS_H
#define EDITOR_WINDOWS_H

#include "circuit_editor_button.h"
#include "quantum_circuit/circuit.h"

struct EditorWindow; // Forward declare

namespace editor::circuit {

using Part = Circuit::Part;
using GridSize = Circuit::GridSize_T;

/// Data for the circuit editor.
inline constinit struct EditorData {
  const GateButton *buttons_arr[Circuit::kMaxQubits][Circuit::kMaxDepth]{};
  bool circuit_dirty = true;

  constexpr EditorData() noexcept {
    for (GridSize qubit = 0; qubit < Circuit::kMaxQubits; ++qubit) {
      for (GridSize layer = 0; layer < Circuit::kMaxQubits; ++layer) {
        buttons_arr[qubit][layer] = &gate::kEmpty; // Initialize all to empty
      }
    }
  }
} editor_data{};

/// Data that can be modified by ImGui (e.g. by an input)
inline constinit struct GuiData {
  // For Circuit Editor Window
  int num_qubits = -1;
  int num_layers = -1;
  bool show_qubit_info = true;

  // For Circuit Info Window
  bool skip_zero_probabilities = true;
} gui_data{};

/* -- Drag and drop info -- */
inline constexpr auto kPayloadTypeSwap = "BUTTON_SWAP";
inline constexpr auto kPayloadTypeSet = "BUTTON_SET";

struct GateSwapPayload {
  GridSize from_qubit;
  GridSize from_layer;
};

struct GateSetPayload {
  const GateButton *button;
};

/* -- Other Constants */
inline constexpr double kInfoGraphBarSize = 0.8;

/* -- Editor Windows -- */
extern EditorWindow main_window;
extern EditorWindow info_window;
extern EditorWindow palette_window;

/* -- Window Functions -- */
void DrawEditor();
void ClearEditor();
void UpdateCircuitSize();

void DrawInfo();

void DrawPalette();

void SetButton(GridSize qubit, GridSize layer, const GateButton *button);

/**
 * Sets the current circuit to an example circuit, which looks like:
 * @code
 * q0 |0> ---X-Z---
 * q1 |0> -H-*---*-
 * q2 |0> -X-----X-
 * @endcode
 */
void SetExampleCircuit();

[[nodiscard]] bool ShouldConnectGates(Part a, Part b, GridSize layer);
[[nodiscard]] bool IsValidSet(GridSize qubit, GridSize layer, Part part);
[[nodiscard]] bool IsValidSwap(GridSize qubit_a, GridSize layer_a, GridSize qubit_b,
                               GridSize layer_b);

} // namespace editor::circuit

#endif // EDITOR_WINDOWS_H
