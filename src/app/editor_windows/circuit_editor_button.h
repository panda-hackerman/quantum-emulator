//
// Created by Eli Michaud on 7/23/2026.
//

#ifndef CIRCUIT_EDITOR_BUTTON_H
#define CIRCUIT_EDITOR_BUTTON_H

#include "../resources/texture.h"
#include "quantum_circuit/circuit.h"

struct GateButton {
  const char *name = nullptr;
  Circuit::Part part = Circuit::Part::kEmpty;
  const SpriteID sprite_id = SpriteID::kUndefined;
  const Circuit::Matrix_T *matrix = nullptr;
};

/// Preset gate types
namespace gate {

/* Default Gates: */
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

#endif // CIRCUIT_EDITOR_BUTTON_H
