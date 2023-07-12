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

#include <Register.h>

#include <catch2/catch_all.hpp>

using namespace regs;

enum class Color : int { RED = -10, BLUE = 0, GREEN = 10 };

struct State;

struct State : Register<State, uint32_t> {
  using Register::Register;

  using Bool1 = Field<State, 0, 1>;
  using Bool2 = Field<State, 1, 1>;
  using Bool3 = Field<State, 2, 1>;

  using Bits1 = Field<State, 4, 3>;

  using Byte2 = Field<State, 8, 8, 0, read_write, uint8_t>;

  using Bits2 = Field<State, 16, 4>;

  using Nibble1 = Field<State, 20, 6>;
};

struct GPIO_Ctrl;

struct GPIO_Ctrl : Register<GPIO_Ctrl, uint32_t> {
  using Register::Register;

  using FuncSel = Field<GPIO_Ctrl, 0, 5>;
  struct OutOver : Field<GPIO_Ctrl, 8, 2> {
    enum {
      OutFromPeri = 0b00,
      InvertedOutFromPeri = 0b01,
      Low = 0b10,
      High = 0b11
    };
  };
  // using OutOver = Field<GPIO_Ctrl, 8, 2>;

  enum class OE_OverValue {
    OE_FromPeri = 0b00,
    InvertedOE_FromPeri = 0b01,
    Disable = 0b10,
    Enable = 0b11
  };

  using OE_Over = Field<GPIO_Ctrl, 12, 2, 0, read_write, OE_OverValue>;

  enum class InOverValue {
    PeriInput = 0b00,
    InvertedPeriInput = 0b01,
    Low = 0b10,
    High = 0b11
  };

  struct InOver : Field<GPIO_Ctrl, 16, 2, 0, read_write, InOverValue>{

    using enum InOverValue;

  };

  using IrqOver = Field<GPIO_Ctrl, 28, 2>;
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

  State *new_state = new (&raw_state) State(noInit{});

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

  auto result = state.read<State::Byte2>();

  STATIC_REQUIRE(std::is_same_v<decltype(result), uint8_t>);

  state.write<State::Byte2>(0x23);

  REQUIRE(state.is<State::Byte2, 0x23>());

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

  State *new_state = new (&raw_state) State(noInit{});

  REQUIRE(new_state->is<State::Bits1, 2>());
  REQUIRE(new_state->is<State::Bits2, 0x0f>());
  REQUIRE(new_state->read<State::Nibble1>() == 0b101010);
}

TEST_CASE("Enums", "[regs]") {
  GPIO_Ctrl ctrl;

  REQUIRE(ctrl.is<GPIO_Ctrl::OutOver, GPIO_Ctrl::OutOver::OutFromPeri>());

  ctrl.write<GPIO_Ctrl::OutOver, GPIO_Ctrl::OutOver::High>();

  REQUIRE(ctrl.is<GPIO_Ctrl::OutOver, GPIO_Ctrl::OutOver::High>());
  REQUIRE(ctrl.read<GPIO_Ctrl::OutOver>() == GPIO_Ctrl::OutOver::High);

  ctrl.write<GPIO_Ctrl::OutOver>(GPIO_Ctrl::OutOver::Low);
  REQUIRE(ctrl.is<GPIO_Ctrl::OutOver, GPIO_Ctrl::OutOver::Low>());
  REQUIRE(ctrl.read<GPIO_Ctrl::OutOver>() == GPIO_Ctrl::OutOver::Low);

  // ctrl.read<GPIO_Ctrl::OE_Over>();

  REQUIRE(ctrl.is<GPIO_Ctrl::OE_Over, GPIO_Ctrl::OE_OverValue::OE_FromPeri>());
  REQUIRE(ctrl.read<GPIO_Ctrl::OE_Over>() ==
          GPIO_Ctrl::OE_OverValue::OE_FromPeri);

  ctrl.write<GPIO_Ctrl::OE_Over, GPIO_Ctrl::OE_OverValue::Disable>();

  REQUIRE(ctrl.is<GPIO_Ctrl::OE_Over, GPIO_Ctrl::OE_OverValue::Disable>());
  REQUIRE(ctrl.read<GPIO_Ctrl::OE_Over>() == GPIO_Ctrl::OE_OverValue::Disable);


  
  REQUIRE(ctrl.is<GPIO_Ctrl::InOver, GPIO_Ctrl::InOver::PeriInput>());
  REQUIRE(ctrl.read<GPIO_Ctrl::InOver>() == GPIO_Ctrl::InOver::PeriInput);

  ctrl.write<GPIO_Ctrl::InOver>(GPIO_Ctrl::InOver::High);

  REQUIRE(ctrl.is<GPIO_Ctrl::InOver, GPIO_Ctrl::InOver::High>());
  REQUIRE(ctrl.read<GPIO_Ctrl::InOver>() == GPIO_Ctrl::InOver::High);

  //STATIC_REQUIRE(ctrl.write<GPIO_Ctrl::InOver, true>());


}
