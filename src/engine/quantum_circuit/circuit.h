//
// Created by Eli Michaud on 6/12/2026.
//

#ifndef CIRCUIT_H
#define CIRCUIT_H
#include <cstdint>

#include "collections/compile_time_map.h"
#include "gate_matrices.h"

struct CircuitPart {
  const char *name;
  const matrix::GateMatrix<1> *gate;
};

namespace gates {

// TODO: Test performance for a reasonable maximums for qubits & depth

inline constexpr std::size_t kMinCircuitQubits = 1;
inline constexpr std::size_t kMaxCircuitQubits = 16;
inline constexpr std::size_t kMinCircuitDepth = 1;
inline constexpr std::size_t kMaxCircuitDepth = 16;

enum GateID : std::uint16_t {
  kNullGate,
  kControlBit,
  kIdentityGate,
  kPauliXGate,
  kPauliYGate,
  kPauliZGate,
  kHadamardGate,
  kTGate,
};

// Ensure enough ids if every gate is unique (2^n >= qubits * layers) incl. reserved for empty
static_assert(1 << (sizeof(GateID) * 8) > kMaxCircuitQubits * kMaxCircuitDepth);

inline constexpr auto kIdToGateMap = CTMapBuilder<GateID, CircuitPart>({
    {kNullGate, {nullptr, nullptr}},
    {kControlBit, {"*", nullptr}},
    {kIdentityGate, {"Identity", &matrix::kIdentity}},
    {kPauliXGate, {"PauliX", &matrix::kPauliX}},
    {kPauliYGate, {"kPauliY", &matrix::kPauliY}},
    {kPauliZGate, {"PauliZ", &matrix::kPauliZ}},
    {kHadamardGate, {"Hadamard", &matrix::kHadamard}},
    {kTGate, {"T", &matrix::kPi8ths}},
});

} // namespace gates

class Circuit {
public:
  using GateID = gates::GateID;
  using GridSize_T = std::uint8_t;

  static_assert(std::numeric_limits<GridSize_T>::min() <= gates::kMinCircuitQubits);
  static_assert(std::numeric_limits<GridSize_T>::max() >= gates::kMaxCircuitQubits);
  static_assert(std::numeric_limits<GridSize_T>::min() <= gates::kMinCircuitDepth);
  static_assert(std::numeric_limits<GridSize_T>::max() >= gates::kMaxCircuitDepth);

  // static constexpr PartID kEmptyId = 0; ///< Represents "nothing" in a circuit

private:
  GridSize_T num_qubits_;
  GridSize_T num_layers_;

  // SparseSet<PartID, CircuitPart> gates_;
  Matrix2D<GateID> part_id_grid_;

public:
  Circuit(const GridSize_T num_qubits, const GridSize_T num_layers) :
      num_qubits_{num_qubits}, num_layers_{num_layers}, part_id_grid_{num_qubits, num_layers} {}

  [[nodiscard]] constexpr GridSize_T NumQubits() const noexcept { return num_qubits_; }
  [[nodiscard]] constexpr GridSize_T CircuitDepth() const noexcept { return num_layers_; }

  void SetNumQubits(GridSize_T new_num_qubits);
  void SetNumLayers(GridSize_T new_num_layers);
  void SetNewSize(GridSize_T new_num_qubits, std::uint8_t new_num_layers);

  [[nodiscard]] bool IsInBounds(GridSize_T qubit, GridSize_T layer) const;
  [[nodiscard]] GateID GetIdAt(GridSize_T qubit, GridSize_T layer);
  [[nodiscard]] const CircuitPart &GetCircuitPart(GridSize_T qubit, GridSize_T layer);

  void SetCircuitPart(GridSize_T qubit, GridSize_T layer, GateID id);

  /**
   * @return An example circuit.
   *
   * Builds a simple example circuit.
   * @code
   * q0 |0> ---X-Z---
   * q1 |0> -H-*---*-
   * q2 |0> -X-----X-
   */
  static Circuit BuildExampleCircuit() {
    Circuit circuit{3, 4};

    circuit.SetCircuitPart(1, 0, gates::kHadamardGate);
    circuit.SetCircuitPart(2, 0, gates::kPauliXGate);

    circuit.SetCircuitPart(0, 1, gates::kPauliXGate);
    circuit.SetCircuitPart(1, 1, gates::kControlBit);

    circuit.SetCircuitPart(0, 2, gates::kPauliZGate);

    circuit.SetCircuitPart(1, 3, gates::kControlBit);
    circuit.SetCircuitPart(2, 3, gates::kPauliXGate);

    return circuit;
  }
};

#endif // CIRCUIT_H
