//
// Created by Eli Michaud on 6/25/2026.
//

#ifndef CUSTOM_WINDOWS_H
#define CUSTOM_WINDOWS_H

#include <bitset>

#include "../resources/texture.h"
#include "imgui.h"
#include "math/constants.h"
#include "quantum_circuit/circuit.h"

struct GateButton {
  const char *name = nullptr;
  Circuit::Part part = Circuit::Part::kEmpty;
  const SpriteID sprite_id = SpriteID::kUndefined;
  const Circuit::Matrix_T *matrix = nullptr;

  constexpr bool MatrixMatches(const Circuit::Matrix_T *other) const {
    if (this->matrix == other) return true; // Pointers are equal

    if (matrix != nullptr && other != nullptr) {
      if (*matrix == *other) return true;   // Values are equal
    }

    return false;
  }
};

/// Preset gate types
namespace gate { /// Default gates

inline constexpr GateButton kEmpty = {"", Circuit::Part::kEmpty};
inline constexpr GateButton kControlBit = {"Control", Circuit::Part::kControlBit,
                                           SpriteID::kCircuitControl};
inline constexpr GateButton kAntiControlBit = {"Anti-Control", Circuit::Part::kAntiControlBit,
                                               SpriteID::kCircuitAntiControl};

inline constexpr GateButton kMeasurementGate = {"Measurement", Circuit::Part::kMeasure,
                                                SpriteID::kCircuitMeasure};
inline constexpr GateButton kSwapGate = {"Swap", Circuit::Part::kSwap, SpriteID::kCircuitSwap};
inline constexpr GateButton kIdentity = {"Identity", Circuit::Part::kMatrix2x2,
                                         SpriteID::kCircuitIdentity, &matrix::kIdentity};
inline constexpr GateButton kPauliX = {"Pauli X", Circuit::Part::kMatrix2x2,
                                       SpriteID::kCircuitPauliX, &matrix::kPauliX};
inline constexpr GateButton kPauliY = {"Pauli Y", Circuit::Part::kMatrix2x2,
                                       SpriteID::kCircuitPauliY, &matrix::kPauliY};
inline constexpr GateButton kPauliZ = {"Pauli Z", Circuit::Part::kMatrix2x2,
                                       SpriteID::kCircuitPauliZ, &matrix::kPauliZ};
inline constexpr GateButton kHadamard = {"Hadamard", Circuit::Part::kMatrix2x2,
                                         SpriteID::kCircuitHadamard, &matrix::kHadamard};
inline constexpr GateButton kTGate = {"T Gate", Circuit::Part::kMatrix2x2, SpriteID::kCircuitTGate,
                                      &matrix::kPi8ths};
inline constexpr GateButton kPhaseGate = {"Phase Gate", Circuit::Part::kMatrix2x2,
                                          SpriteID::kCircuitPhase, &matrix::kPhase};

inline constexpr auto kKnownGates = std::array{
    &kEmpty, &kIdentity,  &kHadamard,   &kPauliX,         &kPauliY,   &kPauliZ,
    &kTGate, &kPhaseGate, &kControlBit, &kAntiControlBit, &kSwapGate, &kMeasurementGate,
};
} // namespace gate

class CircuitEditor {
private:
  Circuit *circuit_;
  bool *circuit_dirty_;
  const GateButton *buttons_arr_[Circuit::kMaxQubits][Circuit::kMaxDepth]; // TODO: Dynamic?

  /* Drag and drop payload types */
  static constexpr auto kPayloadTypeSwap = "BUTTON_SWAP";
  static constexpr auto kPayloadTypeSet = "BUTTON_SET";

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
  void ClearCircuit();
  void Set(Circuit::GridSize_T qubit, Circuit::GridSize_T layer, const GateButton *button);

  void ReadFromCircuit();

  /// True if we should draw a vertical line between these two gates
  [[nodiscard]] bool ShouldConnectGates(Circuit::Part a, Circuit::Part b,
                                        Circuit::GridSize_T layer) const;

  [[nodiscard]] bool IsValidSwap(Circuit::GridSize_T qubit_a, Circuit::GridSize_T layer_a,
                                 Circuit::GridSize_T qubit_b, Circuit::GridSize_T layer_b) const;

  [[nodiscard]] bool IsValidSet(Circuit::GridSize_T qubit, Circuit::GridSize_T layer,
                                Circuit::Part part) const;
};

class CircuitPalette {
public:
  void Draw();
};

class CircuitInfoPanel {
private:
  using Bitset = std::bitset<Circuit::kMaxQubits>;

  Circuit *circuit_;
  bool *circuit_dirty_;
  std::string info_str_;

  std::vector<Complex> state_vector_;

  std::vector<std::string> plot_labels_;
  std::vector<double> plot_probs_;

public:
  struct {
    bool skip_empty_probs = false; // Disabled by default
  } data;

  explicit CircuitInfoPanel(Circuit *circuit, bool *circuit_dirty) :
      circuit_{circuit}, circuit_dirty_{circuit_dirty} {}

  void Draw();
  void RecomputeInfo();
};

#endif // CUSTOM_WINDOWS_H
