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

#if __cplusplus >= 202002L

template <typename T>
concept has_write = requires(const T& t, typename T::reg_type& target, typename T::reg_type value) {
                      typename T::reg_type;
                      t.write(target, value);
                    };

template <typename T, typename value>
concept has_write_constant = requires(const T& t, typename T::reg_type& target) {
                               typename T::reg_type;
                               t.template write_constant<value>(target);
                             };

template <typename T>
concept has_read = requires(const T& t, const typename T::reg_type& target) {
                     typename T::reg_type;
                     {t.read(target) } -> std::same_as<typename T::reg_type>;
                   };

template <typename T>
concept has_is = requires(const T& t, const typename T::reg_type& target) {
   typename T::reg_type;
   t.template is<typename T::reg_type>(target);
   
    };
#endif

using namespace regs;

struct State;

/**
 * Exemplary highly structured type for accessing 'cpu_state'
 */
struct State : RegisterBase<State, uint32_t> {
  using Bool1 = Field<State, 0, 1>;
  using Bool2 = Field<State, 1, 1>;
  using Bool3 = Field<State, 2, 1>;

  using Bool4 = Field<State, 3, 1, read_only>;
  using Bool5 = Field<State, 4, 1>;

  using Byte2 = Field<State, 8, 8>;
};

struct Control;
struct Control : RegisterBase<Control, uint8_t> {
  using Bool1 = Field<Control, 0, 1>;
  using Bool2 = Field<Control, 1, 1>;
  using Bool3 = Field<Control, 2, 1>;

  using Bool4 = Field<Control, 3, 1>;
  using Bool5 = Field<Control, 4, 1>;
};

using FreeField = Field<uint32_t, 0, 1>;

uint32_t raw_state;

TEST_CASE("regs_read_write_is") {
  State state;

  state.write<State::Bool1>(1);

  state.write<State::Bool2, 0>();

  state.write<State::Byte2>(0xFE);

  REQUIRE(state.is<State::Bool1, true>());
  REQUIRE(state.read<State::Bool1>() == 1);
  REQUIRE_FALSE(state.is<State::Bool2, true>());
  REQUIRE(state.read<State::Bool2>() == 0);
  REQUIRE(state.is<State::Byte2, 0xFE>());
  REQUIRE(state.read<State::Byte2>() == 0xFE);
}

TEST_CASE("placement new") {
  State* new_state = new (&raw_state) State();

  new_state->write<State::Bool1>(1);

  new_state->write<State::Bool2, 0>();

  new_state->write<State::Byte2>(0xFE);

  REQUIRE(new_state->is<State::Bool1, true>());
  REQUIRE(new_state->read<State::Bool1>() == 1);
  REQUIRE_FALSE(new_state->is<State::Bool2, true>());
  REQUIRE(new_state->read<State::Bool2>() == 0);
  REQUIRE(new_state->is<State::Byte2, 0xFE>());
  REQUIRE(new_state->read<State::Byte2>() == 0xFE);

  // This fails
  // new_state->read<Control::Bool2>();
}

#if __cplusplus >= 202002L
TEST_CASE("read only ") {
  // State state;

  STATIC_REQUIRE(has_write<State::Bool5>);
  //STATIC_REQUIRE(has_write_constant<State::Bool5, State::Bool5::reg_type>);
  STATIC_REQUIRE(has_read<State::Bool5>);
  //STATIC_REQUIRE(has_is<State::Bool5>);

  STATIC_REQUIRE_FALSE(has_write<State::Bool4>);
  //STATIC_REQUIRE_FALSE(has_write_constant<State::Bool4>);
  STATIC_REQUIRE(has_read<State::Bool4>);
  //STATIC_REQUIRE(has_is<State::Bool4>);
}
#endif
