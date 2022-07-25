// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2022 Daniil Goncharov <neargye@gmail.com>.
//
// Permission is hereby  granted, free of charge, to any  person obtaining a copy
// of this software and associated  documentation files (the "Software"), to deal
// in the Software  without restriction, including without  limitation the rights
// to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
// copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
// IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
// FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
// AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
// LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//#include <catch2/catch.hpp>
#include <catch2/catch_test_macros.hpp>
#include <SingleRegister.h>

using namespace regs;

static uint16_t cpu_state;

/**
 * Exemplary highly structured type for accessing 'cpu_state'
 */
struct Cpu_state : SingleRegister<uint16_t>
{
  struct Mode : Bitfield<0, 4>
  {
    enum
    {
      KERNEL = 0b1000,
      USER = 0b1001,
      MONITOR = 0b1010,
    };
  };
  struct A : Bitfield<6, 1>
  {
  };
  struct B : Bitfield<8, 1>
  {
  };
  struct C : Bitfield<10, 1>
  {
  };
  struct Irq : Bitfield<12, 3>
  {
  };

  // struct Invalid_bit  : Bitfield<18,1> { };
  // struct Invalid_area : Bitfield<15,4> { };

  inline static access_t read() { return cpu_state; }

  inline static void write(access_t &v) { cpu_state = v; }
};

TEST_CASE("regs_set_clear")
{
  SECTION("set")
  {
    Cpu_state::access_t state = Cpu_state::read();
    Cpu_state::Mode::set(state, Cpu_state::Mode::MONITOR);
    Cpu_state::A::set(state, 1);
    Cpu_state::B::set(state);
    Cpu_state::C::set(state, 0xdddd);
    Cpu_state::Irq::set(state, 0xdddd);
    // Cpu_state::Invalid_bit::set(state, 0xdddd);
    // Cpu_state::Invalid_area::set(state, 0xdddd);
    Cpu_state::write(state);
    state = Cpu_state::read();

    REQUIRE(cpu_state == 0b0101010101001010);
    REQUIRE(Cpu_state::Mode::get(state) == Cpu_state::Mode::MONITOR);
    REQUIRE(Cpu_state::A::get(state) == 1);
    REQUIRE(Cpu_state::B::get(state) == 1);
    REQUIRE(Cpu_state::C::get(state) == 1);
    REQUIRE(Cpu_state::Irq::get(state) == 0b101);
  }

  SECTION("clear")
  {

    Cpu_state::access_t state = Cpu_state::read();

    Cpu_state::B::clear(state);
    Cpu_state::Irq::clear(state);
    Cpu_state::write(state);
    state = Cpu_state::read();

    REQUIRE(cpu_state == 0b0000010001001010);
    REQUIRE(Cpu_state::B::get(state) == 0);
    REQUIRE(Cpu_state::Irq::get(state) == 0);
  }
}
