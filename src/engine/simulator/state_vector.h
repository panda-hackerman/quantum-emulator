//
// Created by Eli Michaud on 7/11/2026.
//

#ifndef STATE_VECTOR_H
#define STATE_VECTOR_H
#include <memory>

#include "math/bitwise_math.h"
#include "quantum_circuit/circuit.h"
#include "types.h"

/**
 * Represents the state vector of a quantum circuit with a certain number of qubits.
 */
class StateVector {
private:
  Circuit::GridSize_T num_qubits_;
  std::unique_ptr<Complex[]> u_ptr_;

public:
  explicit StateVector(Circuit::GridSize_T num_qubits = 0);

  /// Creates a deep copy of the state vector
  StateVector(const StateVector &other);

  /// Move the state vector
  StateVector(StateVector &&other) noexcept;

  StateVector &operator=(const StateVector &other);     ///< Deep copy
  StateVector &operator=(StateVector &&other) noexcept; ///< Move operator

  /// Return the size of the state vector (2^n, for n qubits).
  [[nodiscard]] constexpr std::size_t Size() const noexcept { return bit::TwoPowN(num_qubits_); }
  [[nodiscard]] constexpr Circuit::GridSize_T NumQubits() const noexcept { return num_qubits_; }

  explicit constexpr operator std::span<Complex>() const { return {u_ptr_.get(), Size()}; }
  [[nodiscard]] constexpr std::span<Complex> Get() const { return {u_ptr_.get(), Size()}; }

  constexpr Complex &operator[](const std::size_t idx) const { return u_ptr_.get()[idx]; }

  /// Set the state vector to |0...0>
  constexpr void SetZero() const noexcept;

  /// Set the state vector to |1...1>
  constexpr void SetOne() const noexcept;
};
#endif // STATE_VECTOR_H
