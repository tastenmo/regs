/**
 * @file testBytes.cpp
 * @author Martin Heubuch (martin.heubuch@escad.de)
 * @brief 
 * @version 0.1
 * @date 2022-12-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <Bytes.h>

#include <catch2/catch_all.hpp>

using namespace regs;

TEST_CASE("ShiftWithCarry", "[std::byte]") {
    auto shifted_right_carry = shift_right(std::byte{0x81});

    REQUIRE(shifted_right_carry.first == std::byte{0x40});
    REQUIRE(shifted_right_carry.second);

    auto shifted_right_no_carry = shift_right(shifted_right_carry.first);

    REQUIRE(shifted_right_no_carry.first == std::byte{0x20});
    REQUIRE_FALSE(shifted_right_no_carry.second);

    auto shifted_left_carry = shift_left(std::byte{0x81});

    REQUIRE(shifted_left_carry.first == std::byte{0x02});
    REQUIRE(shifted_left_carry.second);

    auto shifted_left_no_carry = shift_left(shifted_left_carry.first);

    REQUIRE(shifted_left_no_carry.first == std::byte{0x04});
    REQUIRE_FALSE(shifted_left_no_carry.second);
}

TEST_CASE("Construction", "[byte_array]") {
    //std::byte result;

    byte_array<0> empty;
    REQUIRE(empty.empty());
    REQUIRE(empty.size() == 0);
    // REQUIRE_THROWS(std::byte{0x00} == empty[0]);

    byte_array one = {std::byte(0x81)};
    REQUIRE_FALSE(one.empty());
    REQUIRE(one.size() == 1);
}

TEST_CASE("ByteShift", "[byte_array]") {
    byte_array toBeShifted = {std::byte(0x81), std::byte(0x82), std::byte(0x83),
                              std::byte(0x84)};

    auto shifted_right_by_one = array_shift_right(toBeShifted, 1);

    REQUIRE(shifted_right_by_one[0] == std::byte{0x40});
    REQUIRE(shifted_right_by_one[1] == std::byte{0xC1});
    REQUIRE(shifted_right_by_one[2] == std::byte{0x41});
    REQUIRE(shifted_right_by_one[3] == std::byte{0x42});

    auto shifted_right_by_three = array_shift_right(toBeShifted, 3);

    REQUIRE(shifted_right_by_three[0] == std::byte{0x50});
    REQUIRE(shifted_right_by_three[1] == std::byte{0x70});
    REQUIRE(shifted_right_by_three[2] == std::byte{0x90});
    REQUIRE(shifted_right_by_three[3] == std::byte{0x10});
}
