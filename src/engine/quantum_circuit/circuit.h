//
// Created by Eli Michaud on 6/12/2026.
//

#ifndef CIRCUIT_H
#define CIRCUIT_H
#include <cassert>
#include <cstdint>
#include <vector>

#include "gates.h"
#include "math/complex_matrix.h"

// class Circuit {
// public:
//   struct Gate {
//     std::string name;
//     const IComplexMatrix *matrix;
//
//     explicit Gate(std::string name = "Identity", const IComplexMatrix *matrix = &gates::kIdentity);
//
//     [[nodiscard]] int NumQubits() const;
//   };
//
//   using CircuitLayer = std::vector<Gate>;
//
// private:
//   std::uint8_t num_qubits_;
//   std::uint8_t num_layers_;
//
//   std::vector<CircuitLayer> layers_;
//
// public:
//   Circuit(const std::uint8_t num_qubits, const std::uint8_t num_layers) :
//       num_qubits_{num_qubits},
//       num_layers_{num_layers},
//       layers_{std::vector(num_layers, CircuitLayer(num_qubits, Gate{}))} {
//     assert(num_qubits > 0 && "Must have at least 1 qubit");
//     assert(num_layers > 0 && "Must have at least 1 layer");
//   }
//
//   [[nodiscard]] std::uint8_t GetNumQubits() const noexcept { return num_qubits_; }
//   [[nodiscard]] std::uint8_t GetCircuitDepth() const noexcept { return num_layers_; }
//
//   void SetNumQubits(std::uint8_t new_num_qubits);
//   void SetNumLayers(std::uint8_t new_num_layers);
//   void SetNewSize(std::uint8_t new_num_qubits, std::uint8_t new_num_layers);
//
//   void SetGate(std::uint8_t qubit, std::uint8_t layer, const Gate &gate);
// };
//
// static Circuit BuildExampleCircuit();

#endif // CIRCUIT_H
