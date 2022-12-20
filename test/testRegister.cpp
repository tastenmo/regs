/**
 * @file test.cpp
 * @author Martin Heubuch (martin.heubuch@escad.de)
 * @brief
 * @version 0.1
 * @date 2022-08-03
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <RegisterBase.h>

#include <catch2/catch_all.hpp>

using namespace regs;

enum class Color : int { RED = -10, BLUE = 0, GREEN = 10 };

struct State;

struct State : RegisterBase<State, uint32_t> {
    using RegisterBase::RegisterBase;

    using Bool1 = Field<State, 0, 1>;
    using Bool2 = Field<State, 1, 1>;
    using Bool3 = Field<State, 2, 1>;

    using Bits1 = Field<State, 4, 3>;

    using Byte2 = Field<State, 8, 8>;

    using Bits2 = Field<State, 16, 4>;

    using Nibble1 = Field<State, 20, 6>;
};

uint32_t raw_state;

TEST_CASE("Bools", "[regs]") {
    State state;

    REQUIRE(state.is<State::Bool1, false>());
    REQUIRE(state.read<State::Bool2>() == 0);
    REQUIRE(state.is<State::Bool3, false>());

    state.write<State::Bool1>(1);

    REQUIRE(state.read<State::Bool1>() == 1);
    REQUIRE(state.is<State::Bool2, false>());
    REQUIRE(state.read<State::Bool3>() == 0);

    state.write<State::Bool3, 1>();

    REQUIRE(state.is<State::Bool1, true>());
    REQUIRE(state.is<State::Bool2, false>());
    REQUIRE(state.is<State::Bool3, true>());

    raw_state = 0x7303;

    State* new_state = new (&raw_state) State(noInit{});

    REQUIRE(new_state->is<State::Bool1, true>());
    REQUIRE(new_state->read<State::Bool2>() == true);
    REQUIRE(new_state->is<State::Bool3, false>());

    REQUIRE(raw_state == 0x7303);

    new_state->write<State::Bool1>(0);

    REQUIRE(new_state->is<State::Bool1, false>());
    REQUIRE(new_state->is<State::Bool2, true>());
    REQUIRE(new_state->is<State::Bool3, false>());

    REQUIRE(raw_state == 0x7302);

    new_state->write<State::Bool2, 0>();

    REQUIRE(new_state->is<State::Bool1, false>());
    REQUIRE(new_state->is<State::Bool2, false>());
    REQUIRE(new_state->is<State::Bool3, false>());

    REQUIRE(raw_state == 0x7300);
}

TEST_CASE("BitsAndBytes", "[regs]") {
    State state;

    REQUIRE(state.is<State::Bits1, 0>());
    REQUIRE(state.read<State::Bits2>() == 0);
    REQUIRE(state.is<State::Byte2, 0>());
    REQUIRE(state.is<State::Nibble1, 0>());

    state.write<State::Bits1>(2);

    REQUIRE(state.read<State::Bits1>() == 2);
    REQUIRE(state.is<State::Bits1, 2>());
    REQUIRE(state.read<State::Bits2>() == 0);
    REQUIRE(state.read<State::Nibble1>() == 0);

    state.write<State::Bits2, 0x0f>();

    REQUIRE(state.read<State::Bits2>() == 15);
    REQUIRE(state.is<State::Bits2, 0x0f>());
    REQUIRE(state.is<State::Bits1, 2>());
    REQUIRE(state.read<State::Nibble1>() == 0);

    state.write<State::Nibble1>(0x2A);

    REQUIRE(state.read<State::Nibble1>() == 0b101010);

    REQUIRE(state.is<State::Bits1, 2>());
    REQUIRE(state.is<State::Bits2, 0x0f>());

    raw_state = 0x2AFFF23;

    State* new_state = new (&raw_state) State(noInit{});

    REQUIRE(new_state->is<State::Bits1, 2>());
    REQUIRE(new_state->is<State::Bits2, 0x0f>());
    REQUIRE(new_state->read<State::Nibble1>() == 0b101010);
    
}
