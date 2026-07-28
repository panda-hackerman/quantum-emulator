//
// Created by Eli Michaud on 6/27/2026.
//

#include <gtest/gtest.h>

#include "math/bitwise_math.h"

template <typename T>
class TwoPowNTest : public ::testing::Test {
public:
  T value = static_cast<T>(5);
  T expected_result = static_cast<T>(32);
};

class TwoPowNNameGenerator {
public:
  template <typename T>
  // ReSharper disable once CppNotAllPathsReturnValue
  static std::string GetName(int) {
    if constexpr (std::is_same_v<T, int8_t>) return "int8";
    if constexpr (std::is_same_v<T, int16_t>) return "int16";
    if constexpr (std::is_same_v<T, int32_t>) return "int32";
    if constexpr (std::is_same_v<T, int64_t>) return "int64";
    if constexpr (std::is_same_v<T, uint8_t>) return "uint8";
    if constexpr (std::is_same_v<T, uint16_t>) return "uint16";
    if constexpr (std::is_same_v<T, uint32_t>) return "uint32";
    if constexpr (std::is_same_v<T, uint64_t>) return "uint64";
  }
};

using TwoPowNTestTypes = ::testing::Types<int8_t, int16_t, int32_t, int64_t, uint8_t,
                                          uint16_t, uint32_t, uint64_t>;

TYPED_TEST_SUITE(TwoPowNTest, TwoPowNTestTypes, TwoPowNNameGenerator);

TYPED_TEST(TwoPowNTest, TwoPowNBitshift) {
  auto result = bit::TwoPowN(this->value);

  EXPECT_EQ(result, this->expected_result);
}