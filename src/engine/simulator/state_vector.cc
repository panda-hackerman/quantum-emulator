//
// Created by Eli Michaud on 7/11/2026.
//

#include "state_vector.h"

StateVector::StateVector(const Circuit::GridSize_T num_qubits) :
    num_qubits_{num_qubits}, u_ptr_{std::make_unique<Complex[]>(1 << num_qubits)} {
  SetZero(); // |0...0>, by default
}

constexpr void StateVector::SetZero() const noexcept {
  for (std::size_t i = 0; i < Size(); ++i) {
    (*this)[i] = i == 0 ? 1 : 0;
  }
}

constexpr void StateVector::SetOne() const noexcept {
  const auto last_idx = Size() - 1;
  for (std::size_t i = 0; i < Size(); ++i) {
    (*this)[i] = i == last_idx ? 1 : 0;
  }
}

StateVector::StateVector(const StateVector &other) :
    num_qubits_{other.num_qubits_}, u_ptr_{std::make_unique<Complex[]>(1 << num_qubits_)} {
  for (std::size_t i = 0; i < Size(); ++i) {
    (*this)[i] = other[i];
  }
}

StateVector::StateVector(StateVector &&other) noexcept :
    num_qubits_{other.num_qubits_}, u_ptr_(std::move(other.u_ptr_)) {
}

StateVector &StateVector::operator=(const StateVector &other) {
  if (this == &other) return *this;
  num_qubits_ = other.num_qubits_;
  u_ptr_ = std::make_unique<Complex[]>(Size());

  for (std::size_t i = 0; i < Size(); ++i) {
    u_ptr_.get()[i] = other[i];
  }

  return *this;
}

StateVector &StateVector::operator=(StateVector &&other) noexcept {
  if (this == &other) return *this;
  num_qubits_ = other.num_qubits_;
  u_ptr_ = std::move(other.u_ptr_);
  return *this;
}