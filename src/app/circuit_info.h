//
// Created by Eli Michaud on 7/11/2026.
//

#ifndef CIRCUIT_INFO_H
#define CIRCUIT_INFO_H
#include <bitset>

#include "simulator/state_vector.h"

struct ComputedCircuitInfo {
  using Bitset = std::bitset<Circuit::kMaxQubits>;

  std::vector<std::string> labels;
  std::vector<const char *> labels_c;
  std::vector<const char *> labels_c_nonzero;

  std::vector<double> densities;
  std::vector<double> densities_nonzero;

  std::vector<double> amplitudes;
  std::vector<double> amplitudes_nonzero;

  void RecalculateData(const StateVector &state_vector);
};

#endif // CIRCUIT_INFO_H
