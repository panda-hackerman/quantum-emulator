//
// Created by Eli Michaud on 6/25/2026.
//

#ifndef CUSTOM_WINDOWS_H
#define CUSTOM_WINDOWS_H
#include <format>

#include "quantum_circuit/circuit.h"

struct CircuitEditorWindow {
  Circuit *circuit = nullptr;

  struct {
    int num_qubits;
    int num_layers;
  } data;

  void Draw();
  void UpdateCircuitSize();
};

#endif // CUSTOM_WINDOWS_H
