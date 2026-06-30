//
// Created by Eli Michaud on 6/25/2026.
//

#ifndef CUSTOM_WINDOWS_H
#define CUSTOM_WINDOWS_H
#include <iostream>

#include "imgui.h"
#include "quantum_circuit/circuit.h"

struct GateButton {
  const char *name = nullptr;
  Circuit::Part part_type = Circuit::Part::kEmpty;
  const Circuit::Matrix_T *matrix = nullptr;

  constexpr bool MatrixMatches(const Circuit::Matrix_T *other) const {
    if (this->matrix == other) return true; // Pointers are equal

    if (matrix != nullptr && other != nullptr) {
      if (*matrix == *other) return true;   // Values are equal
    }

    return false;
  }
};

namespace gate { /// Default gates
inline constexpr GateButton kEmpty = {"", Circuit::Part::kEmpty};

inline constexpr GateButton kControlBit = {"*", Circuit::Part::kControlBit};
inline constexpr GateButton kAntiControlBit = {"!*", Circuit::Part::kAntiControlBit};
inline constexpr GateButton kMeasurementGate = {"%", Circuit::Part::kMeasure};
inline constexpr GateButton kSwapGate = {"-x-", Circuit::Part::kSwap};

inline constexpr GateButton kIdentity = {"I", Circuit::Part::kMatrix2x2, &matrix::kIdentity};
inline constexpr GateButton kPauliX = {"X", Circuit::Part::kMatrix2x2, &matrix::kPauliX};
inline constexpr GateButton kPauliY = {"Y", Circuit::Part::kMatrix2x2, &matrix::kPauliY};
inline constexpr GateButton kPauliZ = {"Z", Circuit::Part::kMatrix2x2, &matrix::kPauliZ};
inline constexpr GateButton kHadamard = {"H", Circuit::Part::kMatrix2x2, &matrix::kHadamard};
inline constexpr GateButton kTGate = {"T", Circuit::Part::kMatrix2x2, &matrix::kPi8ths};

inline constexpr auto kKnownGates = std::array{
    &kEmpty,  &kControlBit, &kAntiControlBit, &kMeasurementGate, &kSwapGate, &kIdentity,
    &kPauliX, &kPauliY,     &kPauliZ,         &kHadamard,        &kTGate,
};
} // namespace gate

class CircuitEditor {
private:
  Circuit *circuit_;
  bool *circuit_dirty_;
  const GateButton *buttons_arr_[Circuit::kMaxQubits][Circuit::kMaxDepth]; // TODO: Dynamic?

public:
  struct { // Data that will be touched directly by ImGui (e.g. by an input)
    int num_qubits = -1;
    int num_layers = -1;
  } data;

  struct GateSwapPayload {
    Circuit::GridSize_T from_qubit;
    Circuit::GridSize_T from_layer;
  };

  explicit CircuitEditor(Circuit *circuit, bool *circuit_dirty) :
      circuit_{circuit}, circuit_dirty_{circuit_dirty}, buttons_arr_{} {
    data.num_qubits = circuit->GetNumQubits();
    data.num_layers = circuit->GetNumLayers();

    ReadFromCircuit();
  }

  void Draw();
  void UpdateCircuitSize();
  void Set(Circuit::GridSize_T qubit, Circuit::GridSize_T layer, const GateButton *button);

  void ReadFromCircuit();
};

class CircuitPalette {
public:
  static constexpr ImVec2 button_size_{60, 60};

  void Draw();
};

class CircuitInfoPanel {
private:
  Circuit *circuit_;
  bool *circuit_dirty_;
  std::string info_str_;

public:
  explicit CircuitInfoPanel(Circuit *circuit, bool *circuit_dirty) :
      circuit_{circuit}, circuit_dirty_{circuit_dirty} {}

  void Draw();
  void RecomputeInfo();
};

#endif // CUSTOM_WINDOWS_H
