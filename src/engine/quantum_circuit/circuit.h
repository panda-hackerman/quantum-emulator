//
// Created by Eli Michaud on 6/12/2026.
//

#ifndef CIRCUIT_H
#define CIRCUIT_H
#include <cstdint>
#include <optional>

#include "collections/sparse_set.h"
#include "gates.h"

namespace gates {

// TODO: Test performance for a reasonable maximums for qubits & depth

constexpr inline std::size_t kMinCircuitQubits = 1;
constexpr inline std::size_t kMaxCircuitQubits = 16;
constexpr inline std::size_t kMinCircuitDepth = 1;
constexpr inline std::size_t kMaxCircuitDepth = 16;

} // namespace gates

struct CircuitPart {
  std::string name;
  std::optional<gates::GateMatrix<1>> matrix; ///< A 2x2 matrix
};

class Circuit {
public:
  using PartID = std::uint16_t;
  using GridSize_T = std::uint8_t;

  // Ensure enough ids if every gate is unique (2^n >= qubits * layers) incl. reserved for empty
  static_assert(1 << (sizeof(PartID) * 8) > gates::kMaxCircuitQubits * gates::kMaxCircuitDepth);
  static_assert(std::numeric_limits<GridSize_T>::min() <= gates::kMinCircuitQubits);
  static_assert(std::numeric_limits<GridSize_T>::max() >= gates::kMaxCircuitQubits);
  static_assert(std::numeric_limits<GridSize_T>::min() <= gates::kMinCircuitDepth);
  static_assert(std::numeric_limits<GridSize_T>::max() >= gates::kMaxCircuitDepth);

  static constexpr PartID kEmptyId = 0;

private:
  GridSize_T num_qubits_;
  GridSize_T num_layers_;

  SparseSet<PartID, CircuitPart> gates_;
  Matrix2D<PartID> part_id_grid_;

public:
  Circuit(const GridSize_T num_qubits, const GridSize_T num_layers) :
      num_qubits_{num_qubits}, num_layers_{num_layers}, part_id_grid_{num_qubits, num_layers} {}

  [[nodiscard]] GridSize_T NumQubits() const noexcept { return num_qubits_; }
  [[nodiscard]] GridSize_T CircuitDepth() const noexcept { return num_layers_; }

  void SetNumQubits(GridSize_T new_num_qubits);
  void SetNumLayers(GridSize_T new_num_layers);
  void SetNewSize(GridSize_T new_num_qubits, std::uint8_t new_num_layers);

  void AddNewGate(PartID id, CircuitPart part);
  void SetCircuitPart(GridSize_T qubit, GridSize_T layer, PartID id);
};

// static Circuit BuildExampleCircuit();

#endif // CIRCUIT_H
