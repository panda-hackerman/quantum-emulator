//
// Created by Eli Michaud on 7/13/2026.
//

#include "circuit_info_processor.h"

void ComputedCircuitInfo::RecalculateData(const StateVector &state_vector) {
  const int num_qubits = state_vector.NumQubits();
  const int state_vector_size = static_cast<int>(state_vector.Size()); // = 1 << num_qubits

  // Clear and reserve data
  labels.clear();
  labels_c.clear();
  labels_c_nonzero.clear();

  densities.clear();
  amplitudes.clear();

  densities_nonzero.clear();
  amplitudes_nonzero.clear();

  labels.reserve(state_vector_size);
  densities.reserve(state_vector_size);
  amplitudes.reserve(state_vector_size);

  // Calculate
  for (int i = 0; i < state_vector_size; ++i) {
    const double amplitude = std::abs(state_vector[i]);
    const double density = std::pow(amplitude, 2);
    const bool nonzero = amplitude != 0;

    amplitudes.push_back(amplitude);
    densities.push_back(density);
    if (nonzero) amplitudes_nonzero.push_back(amplitude);
    if (nonzero) densities_nonzero.push_back(density);

    std::string bit_string = Bitset(i).to_string(); // Convert decimal -> binary
    labels.emplace_back(bit_string.substr(bit_string.size() - num_qubits));
    labels_c.push_back(labels.back().data());

    if (nonzero) labels_c_nonzero.push_back(labels.back().data());
  }
}