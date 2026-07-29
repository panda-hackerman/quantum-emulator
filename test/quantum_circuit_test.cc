//
// Created by Eli Michaud on 7/29/2026.
//

#include "gtest/gtest.h"
#include "quantum_circuit/circuit.h"

class BasicCircuitTests
    : public ::testing::TestWithParam<std::tuple<Circuit::GridSize_T, Circuit::GridSize_T>> {
protected:
  Circuit circuit_;

  static constexpr std::array kValidParts{
      Circuit::Part::kMatrix2x2, Circuit::Part::kControlBit, Circuit::Part::kAntiControlBit,
      Circuit::Part::kMeasure,   Circuit::Part::kSwap,       Circuit::Part::kEmpty,
  };

  static constexpr std::array kValidMatrices{
      &matrix::kPauliX, &matrix::kPauliY, &matrix::kPauliZ,   &matrix::kHadamard,
      &matrix::kPi8ths, &matrix::kPhase,  &matrix::kIdentity,
  };

  BasicCircuitTests() : circuit_{std::get<0>(GetParam()), std::get<1>(GetParam())} {}
};

inline std::string CircuitTestNameSuffixGenerator(
    const ::testing::TestParamInfo<BasicCircuitTests::ParamType> &info) {
  const auto num_qubits = std::get<0>(info.param);
  const auto num_layers = std::get<1>(info.param);

  return "Qubits" + std::to_string(num_qubits) + "_Depth" + std::to_string(num_layers);
}

constexpr Circuit::GridSize_T kMaxQubitsPlusOne = Circuit::kMaxQubits + 1;
constexpr Circuit::GridSize_T kMaxDepthPlusOne = Circuit::kMaxDepth + 1;

INSTANTIATE_TEST_SUITE_P(CircuitTests, BasicCircuitTests,
                         ::testing::Combine(::testing::Range(Circuit::kMinQubits,
                                                             kMaxQubitsPlusOne),
                                            ::testing::Range(Circuit::kMinDepth, kMaxDepthPlusOne)),
                         CircuitTestNameSuffixGenerator);

TEST_P(BasicCircuitTests, IsInitAtCorrectSize) {
  const Circuit::GridSize_T qubits = std::get<0>(GetParam());
  const Circuit::GridSize_T layers = std::get<1>(GetParam());

  EXPECT_EQ(circuit_.GetNumQubits(), qubits);
  EXPECT_EQ(circuit_.GetNumLayers(), layers);
}

TEST_P(BasicCircuitTests, Resize_Works) {
  const Circuit::GridSize_T qubits = std::get<0>(GetParam());
  const Circuit::GridSize_T layers = std::get<1>(GetParam());

  // Resize qubits
  ASSERT_NO_THROW(circuit_.SetNumQubits(Circuit::kMaxQubits));
  EXPECT_EQ(circuit_.GetNumQubits(), Circuit::kMaxQubits);

  // Resize layers
  ASSERT_NO_THROW(circuit_.SetNumLayers(Circuit::kMaxDepth));
  EXPECT_EQ(circuit_.GetNumLayers(), Circuit::kMaxDepth);

  // Resize both
  ASSERT_NO_THROW(circuit_.SetSize(qubits, layers));
  EXPECT_EQ(circuit_.GetNumQubits(), qubits);
  EXPECT_EQ(circuit_.GetNumLayers(), layers);
}

TEST_P(BasicCircuitTests, ClearCircuit_Works) {

  const Circuit::GridSize_T num_qubits = std::get<0>(GetParam());
  const Circuit::GridSize_T num_layers = std::get<1>(GetParam());

  for (Circuit::GridSize_T qubit = 0; qubit < num_qubits; ++qubit) {
    for (Circuit::GridSize_T layer = 0; layer < num_layers; ++layer) {
      ASSERT_NO_THROW(circuit_.AddGate(qubit, layer, &matrix::kHadamard));
    }
  }

  circuit_.Clear();

  for (Circuit::GridSize_T qubit = 0; qubit < num_qubits; ++qubit) {
    for (Circuit::GridSize_T layer = 0; layer < num_layers; ++layer) {
      EXPECT_EQ(circuit_.GetPartTypeAt(qubit, layer), Circuit::Part::kEmpty);
      EXPECT_EQ(circuit_.GetMatrixAt(qubit, layer), nullptr);
    }
  }
}

TEST_P(BasicCircuitTests, SetCircuitParts_NoThrow_CorrectValues) {
  const Circuit::GridSize_T num_qubits = std::get<0>(GetParam());
  const Circuit::GridSize_T num_layers = std::get<1>(GetParam());

  for (Circuit::GridSize_T qubit = 0; qubit < num_qubits; ++qubit) {
    for (Circuit::GridSize_T layer = 0; layer < num_layers; ++layer) {

      // Matrix Gate
      ASSERT_NO_THROW(circuit_.AddGate(qubit, layer, &matrix::kHadamard));
      EXPECT_EQ(circuit_.GetPartTypeAt(qubit, layer), Circuit::Part::kMatrix2x2);
      EXPECT_EQ(circuit_.GetMatrixAt(qubit, layer), &matrix::kHadamard);

      // Control bit
      ASSERT_NO_THROW(circuit_.AddControlBit(qubit, layer));
      EXPECT_EQ(circuit_.GetPartTypeAt(qubit, layer), Circuit::Part::kControlBit);
      EXPECT_EQ(circuit_.GetMatrixAt(qubit, layer), nullptr);

      // Empty
      ASSERT_NO_THROW(circuit_.AddEmpty(qubit, layer));
      EXPECT_EQ(circuit_.GetPartTypeAt(qubit, layer), Circuit::Part::kEmpty);
      EXPECT_EQ(circuit_.GetMatrixAt(qubit, layer), nullptr);

      // Anti-control
      ASSERT_NO_THROW(circuit_.AddAntiControlBit(qubit, layer));
      EXPECT_EQ(circuit_.GetPartTypeAt(qubit, layer), Circuit::Part::kAntiControlBit);
      EXPECT_EQ(circuit_.GetMatrixAt(qubit, layer), nullptr);

      // Swap
      ASSERT_NO_THROW(circuit_.AddSwap(qubit, layer));
      EXPECT_EQ(circuit_.GetPartTypeAt(qubit, layer), Circuit::Part::kSwap);
      EXPECT_EQ(circuit_.GetMatrixAt(qubit, layer), nullptr);

      // Measurement
      ASSERT_NO_THROW(circuit_.AddMeasurement(qubit, layer));
      EXPECT_EQ(circuit_.GetPartTypeAt(qubit, layer), Circuit::Part::kMeasure);
      EXPECT_EQ(circuit_.GetMatrixAt(qubit, layer), nullptr);
    }
  }
}

TEST_P(BasicCircuitTests, SetCircuitPartsOutOfBounds_Throws) {

  const Circuit::GridSize_T num_qubits = std::get<0>(GetParam());
  const Circuit::GridSize_T num_layers = std::get<1>(GetParam());

  for (Circuit::GridSize_T qubit = num_qubits; qubit < kMaxQubitsPlusOne; ++qubit) {
    for (Circuit::GridSize_T layer = num_layers; layer < kMaxDepthPlusOne; ++layer) {

      EXPECT_ANY_THROW(circuit_.AddGate(qubit, layer, &matrix::kHadamard));
      EXPECT_ANY_THROW((void)circuit_.GetPartTypeAt(qubit, layer));
      EXPECT_ANY_THROW((void)circuit_.GetMatrixAt(qubit, layer));

      EXPECT_ANY_THROW(circuit_.AddEmpty(qubit, layer));
      EXPECT_ANY_THROW((void)circuit_.GetMatrixAt(qubit, layer));
      EXPECT_ANY_THROW((void)circuit_.GetPartTypeAt(qubit, layer));

      EXPECT_ANY_THROW(circuit_.AddControlBit(qubit, layer));
      EXPECT_ANY_THROW((void)circuit_.GetMatrixAt(qubit, layer));
      EXPECT_ANY_THROW((void)circuit_.GetPartTypeAt(qubit, layer));

      EXPECT_ANY_THROW(circuit_.AddAntiControlBit(qubit, layer));
      EXPECT_ANY_THROW((void)circuit_.GetMatrixAt(qubit, layer));
      EXPECT_ANY_THROW((void)circuit_.GetPartTypeAt(qubit, layer));

      EXPECT_ANY_THROW(circuit_.AddMeasurement(qubit, layer));
      EXPECT_ANY_THROW((void)circuit_.GetMatrixAt(qubit, layer));
      EXPECT_ANY_THROW((void)circuit_.GetPartTypeAt(qubit, layer));

      EXPECT_ANY_THROW(circuit_.AddSwap(qubit, layer));
      EXPECT_ANY_THROW((void)circuit_.GetMatrixAt(qubit, layer));
      EXPECT_ANY_THROW((void)circuit_.GetPartTypeAt(qubit, layer));
    }
  }
}

TEST_P(BasicCircuitTests, GetPartsInLayer_NoThrow_CorrectValues) {

  const Circuit::GridSize_T num_qubits = std::get<0>(GetParam());
  const Circuit::GridSize_T num_layers = std::get<1>(GetParam());

  for (Circuit::GridSize_T layer = 0; layer < num_layers; ++layer) {
    std::vector expected_parts(num_qubits, Circuit::Part::kEmpty);

    // Build random layer
    for (Circuit::GridSize_T qubit = 0; qubit < num_qubits; ++qubit) {

      const Circuit::GridIndex_T flat_idx = qubit + (num_qubits * layer);
      std::size_t index_wrapped = flat_idx % kValidParts.size();
      Circuit::Part part = kValidParts.at(index_wrapped);

      expected_parts.at(qubit) = part;

      // Ensure circuit is valid
      int iterations = 0;
      while (!Circuit::IsValidLayer(expected_parts)) {
        index_wrapped++;
        index_wrapped = index_wrapped % kValidParts.size();
        part = kValidParts.at(index_wrapped);

        expected_parts.at(qubit) = part;

        if (++iterations >= kValidParts.size()) {
          FAIL() << "Couldn't build a valid circuit with any gate, for some reason!";
        }
      }

      // Add part to circuit
      switch (part) {
        case Circuit::Part::kEmpty:
          circuit_.AddEmpty(qubit, layer);
          break;
        case Circuit::Part::kMatrix2x2:
          circuit_.AddGate(qubit, layer, &matrix::kHadamard);
          break;
        case Circuit::Part::kControlBit:
          circuit_.AddControlBit(qubit, layer);
          break;
        case Circuit::Part::kAntiControlBit:
          circuit_.AddAntiControlBit(qubit, layer);
          break;
        case Circuit::Part::kMeasure:
          circuit_.AddMeasurement(qubit, layer);
          break;
        case Circuit::Part::kSwap:
          circuit_.AddSwap(qubit, layer);
          break;
        default:
          FAIL() << "Unexpected gate!";
      }
    }

    // Check layer matches
    // std::span<Circuit::Part> actual_layer;
    ASSERT_NO_THROW((void)circuit_.GetPartsInLayer(layer));
    auto actual_layer = circuit_.GetPartsInLayer(layer);

    for (Circuit::GridSize_T qubit = 0; qubit < num_qubits; ++qubit) {
      EXPECT_EQ(expected_parts.at(qubit), actual_layer[qubit]);
    }
  }
}

TEST_P(BasicCircuitTests, GetMatricesInLayer_NoThrow_CorrectValues) {

  const Circuit::GridSize_T num_qubits = std::get<0>(GetParam());
  const Circuit::GridSize_T num_layers = std::get<1>(GetParam());

  for (Circuit::GridSize_T layer = 0; layer < num_layers; ++layer) {
    std::vector expected_matrices(num_qubits, &matrix::kIdentity);

    // Build random layer
    for (Circuit::GridSize_T qubit = 0; qubit < num_qubits; ++qubit) {
      const std::size_t index_wrapped = qubit % kValidMatrices.size();
      const Circuit::Matrix_T *matrix = kValidMatrices.at(index_wrapped);

      expected_matrices.at(qubit) = matrix;

      ASSERT_NO_THROW(circuit_.AddGate(qubit, layer, matrix));
    }

    // Check layer matches
    ASSERT_NO_THROW((void)circuit_.GetMatricesInLayer(layer));
    auto actual_layer = circuit_.GetMatricesInLayer(layer);

    for (Circuit::GridSize_T qubit = 0; qubit < num_qubits; ++qubit) {
      EXPECT_EQ(*expected_matrices.at(qubit), *actual_layer[qubit]);
    }
  }

}
