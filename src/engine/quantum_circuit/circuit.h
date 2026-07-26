//
// Created by Eli Michaud on 6/12/2026.
//

#ifndef CIRCUIT_H
#define CIRCUIT_H
#include <cstdint>
#include <format>
#include <utility>

#include "collections/sparse_set.h"
#include "math/constants.h"

class Circuit {
public:
  /// Unsigned integer type guaranteed to store up to at least the max qubit and layer size.
  using GridSize_T = std::uint_least8_t;
  /// Integer type guaranteed to have at least 1 unique value for each possible qubit & layer index.
  using GridIndex_T = std::uint_least8_t;

  /// Pointer to generic complex matrix
  using Matrix_T = Matrix2D<Complex, 2, 2>;

  /// A single circuit part.
  enum class Part : std::uint8_t {
    kEmpty = 0,
    kMatrix2x2,
    kControlBit,
    kAntiControlBit,
    kMeasure,
    kSwap,
  };

  [[nodiscard]] static constexpr bool IsControlBit(const Part part) {
    return part == Part::kControlBit || part == Part::kAntiControlBit;
  }

  static constexpr GridSize_T kMinQubits = 1; ///< Minimum allowed qubits in a circuit
  static constexpr GridSize_T kMaxQubits = 8; ///< Maximum allowed qubits in a circuit
  static constexpr GridSize_T kMinDepth = 1; ///< Minimum allowed layers (aka circuit depth)
  static constexpr GridSize_T kMaxDepth = 31; ///< Maximum allowed layers (aka circuit depth)

  static_assert(std::cmp_less_equal(std::numeric_limits<GridSize_T>::lowest(), kMinQubits));
  static_assert(std::cmp_less_equal(std::numeric_limits<GridSize_T>::lowest(), kMinDepth));
  static_assert(std::cmp_greater_equal(std::numeric_limits<GridSize_T>::max(), kMaxQubits));
  static_assert(std::cmp_greater_equal(std::numeric_limits<GridSize_T>::max(), kMaxDepth));

  static_assert(std::cmp_greater(tmp::max_unique_v<GridIndex_T>, (kMaxQubits * kMaxDepth)));

protected:
  GridSize_T num_qubits_ = kMinQubits;
  GridSize_T num_layers_ = kMinDepth;

  std::array<Part, kMaxDepth * kMaxQubits> parts_array_;
  SparseSet<GridIndex_T, const Matrix_T *> id_to_matrix_;

  /// Convert coordinates to flat index (without bounds checking)
  [[nodiscard]]
  static constexpr GridIndex_T IndexOf(const GridSize_T qubit, const GridSize_T layer) noexcept {
    /* This is flipped from the typical circuit diagram representation. Here, we use the qubits as
     * the columns, so that the layers are contiguous. */
    return qubit + (kMaxQubits * layer);
  }

  /**
   * Check if the qubit and layer are within the minimum and maximum allowed values. Otherwise,
   * throws an exception.
   * @throw std::out_of_range If either parameter is not within the allowed range.
   * @see Circuit::kMinQubits, Circuit::kMaxQubits, Circuit::kMinDepth, Circuit::kMaxQubits
   */
  static constexpr void AssertInLimit(const GridSize_T qubit, const GridSize_T layer) {
    if (qubit < kMinQubits || qubit > kMaxQubits || layer < kMinDepth || layer > kMaxDepth) {
      throw std::out_of_range(std::format(
          "Qubit {} and/or Layer {} must be within the ranges ({} to {}) and ({} to {}), respectively",
          qubit, layer, kMinQubits, kMaxQubits, kMinDepth, kMaxDepth));
    }
  }

  /**
   * Check that the qubit and layer inside the bounds of the current size of the circuit. Otherwise,
   * throws an exception.
   * @throws std::out_of_range If either parameter is out of bounds.
   */
  constexpr void AssertInRange(const GridSize_T qubit, const GridSize_T layer) const {
    if (qubit >= num_qubits_ || layer >= num_layers_) {
      throw std::out_of_range(std::format("Qubit {}, Layer {}, is out of range for size ({}, {})!",
                                          qubit, layer, num_qubits_, num_layers_));
    }
  }

public:
  Circuit(const GridSize_T qubits, const GridSize_T layers) :
      num_qubits_{qubits}, num_layers_{layers}, parts_array_{} {}

  void AddEmpty(GridSize_T qubit, GridSize_T layer); /// Set to empty
  void AddGate(GridSize_T qubit, GridSize_T layer, const Matrix_T *matrix); /// Add a gate
  void AddControlBit(GridSize_T qubit, GridSize_T layer); /// Add a control bit
  void AddAntiControlBit(GridSize_T qubit, GridSize_T layer); /// Add an anti-control bit
  void AddMeasurement(GridSize_T qubit, GridSize_T layer); /// Add a measurement gate
  void AddSwap(GridSize_T qubit, GridSize_T layer); /// Add a swap gate

  void SetNumQubits(GridSize_T num_qubits);
  void SetNumLayers(GridSize_T num_layers);
  void SetSize(GridSize_T num_qubits, GridSize_T num_layers);

  void Clear();

  [[nodiscard]] GridSize_T GetNumQubits() const noexcept { return num_qubits_; }
  [[nodiscard]] GridSize_T GetNumLayers() const noexcept { return num_layers_; }

  [[nodiscard]] Part GetPartTypeAt(GridSize_T qubit, GridSize_T layer) const;
  [[nodiscard]] const Matrix_T *GetMatrixAt(GridSize_T qubit, GridSize_T layer) const;

  /// Get the part at the given index without bounds checking
  [[nodiscard]] Part GetPartTypeUnsafe(GridSize_T qubit, GridSize_T layer) const;

  /// Get the gate at the given index without bounds checking
  [[nodiscard]] const Matrix_T *GetMatrixUnsafe(GridSize_T qubit, GridSize_T layer) const;

  [[nodiscard]] std::span<Part> GetPartsInLayer(GridSize_T layer);
  [[nodiscard]] std::span<const Part> GetPartsInLayer(GridSize_T layer) const;

  [[nodiscard]] std::vector<const Matrix_T *> GetMatricesInLayer(GridSize_T layer) const;

  /**
   * Check for valid swaps
   * @param layer The layer to check
   * @return True if there are at least 2 swap parts in this layer
   */
  [[nodiscard]] bool ExistsValidSwapInLayer(GridSize_T layer) const;

  /**
   * Check if a part exists in a layer
   * @param part The part to check for
   * @param layer The layer to check
   * @return True if the part exists in the layer
   */
  [[nodiscard]] bool ExistsInLayer(Part part, GridSize_T layer) const;

  /**
   * Check if this layer is valid
   * @param layer The parts in the layer to check
   * @return True if this is a valid arrangement.
   */
  [[nodiscard]] static bool IsValidLayer(std::span<Part> layer);

  /**
   * Builds a simple example circuit, which looks like:
   * @code
   * q0 |0> ---X-Z---
   * q1 |0> -H-*---*-
   * q2 |0> -X-----X-
   * @endcode
   *
   * @return A new circuit
   */
  static Circuit BuildExampleCircuit() {
    Circuit circuit{3, 4};

    circuit.AddGate(1, 0, &matrix::kHadamard);
    circuit.AddGate(2, 0, &matrix::kPauliX);

    circuit.AddGate(0, 1, &matrix::kPauliX);
    circuit.AddControlBit(1, 1);

    circuit.AddGate(0, 2, &matrix::kPauliZ);

    circuit.AddControlBit(1, 3);
    circuit.AddGate(2, 3, &matrix::kPauliX);

    return circuit;
  }
};

#endif // CIRCUIT_H
