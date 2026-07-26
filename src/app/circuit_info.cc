//
// Created by Eli Michaud on 7/13/2026.
//

#include "circuit_info.h"

void ComputedCircuitInfo::RecalculateData(const StateVector &state_vector) {
  const Circuit::GridSize_T num_qubits = state_vector.NumQubits();
  const std::size_t state_vector_size = state_vector.Size();

  // Clear and reserve data
  labels.clear();
  labels_c.clear();
  labels_c_nonzero.clear();

  densities.clear();
  amplitudes.clear();

  densities_nonzero.clear();
  amplitudes_nonzero.clear();

  chance_of_one.clear();

  labels.reserve(state_vector_size);
  densities.reserve(state_vector_size);
  amplitudes.reserve(state_vector_size);

  chance_of_one.resize(num_qubits);

  for (std::size_t i = 0; i < state_vector_size; ++i) {
    // Calculate Probability Density / Amplitudes
    const double amplitude = std::abs(state_vector[i]);
    const double density = std::pow(amplitude, 2);
    const bool nonzero = amplitude != 0;

    amplitudes.push_back(amplitude);
    densities.push_back(density);
    if (nonzero) amplitudes_nonzero.push_back(amplitude);
    if (nonzero) densities_nonzero.push_back(density);

    Bitset bitset = i;
    std::string bit_string = bitset.to_string();

    labels.emplace_back(bit_string.substr(bit_string.size() - num_qubits));
    labels_c.push_back(labels.back().data()); // Ignore error, compiles fine

    if (nonzero) labels_c_nonzero.push_back(labels.back().data());

    // Add chance of one
    for (Circuit::GridSize_T qubit = 0; qubit < num_qubits; ++qubit) {
      if (bitset[qubit]) chance_of_one.at(qubit) += density;
    }
  }
}