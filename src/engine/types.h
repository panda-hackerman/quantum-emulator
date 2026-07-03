/**
 * @file types.h
 *
 * @brief Generic type definitions
 *
 * @author Eli Michaud
 * @since 7/3/2026
 */

#ifndef QUANTUMEMULATOR_TYPES_H
#define QUANTUMEMULATOR_TYPES_H

#include <complex>

#include "math/matrix.h"

/* You can change from float to double to increase precision */

/// The underlying type for the complex number type
using Complex_Base = float;

/// A complex number
using Complex = std::complex<Complex_Base>;

#endif // QUANTUMEMULATOR_TYPES_H
