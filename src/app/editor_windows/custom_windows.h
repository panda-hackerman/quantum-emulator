//
// Created by Eli Michaud on 6/25/2026.
//

#ifndef CUSTOM_WINDOWS_H
#define CUSTOM_WINDOWS_H
#include "quantum_circuit/circuit.h"

struct CircuitEditorWindow {
  Circuit *circuit = nullptr;

  struct {
    int num_qubits = 3;
    int num_layers = 3;
  } data;

  void Draw() {

  }
};

#endif // CUSTOM_WINDOWS_H
