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

//#include <catch2/catch.hpp>
#include <catch2/catch_test_macros.hpp>
#include <SingleRegister.h>

using namespace regs;

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

};

struct bit_test : SingleRegister<uint32_t>
{

  using EN = Bit<0>;

  using CTS = Bit<4>;

  struct RTS : Bit<8>{};

  // struct Invalid_bit  : Bitfield<18,1> { };
  // struct Invalid_area : Bitfield<15,4> { };

};

TEST_CASE("regs_set_clear")
{

  Cpu_state cpu;
  SECTION("set")
  {
    Cpu_state::access_t state = cpu.read();
    Cpu_state::Mode::set(state, Cpu_state::Mode::MONITOR);
    Cpu_state::A::set(state, 1);
    Cpu_state::B::set(state);
    Cpu_state::C::set(state, 0xdddd);
    Cpu_state::Irq::set(state, 0xdddd);
    // Cpu_state::Invalid_bit::set(state, 0xdddd);
    // Cpu_state::Invalid_area::set(state, 0xdddd);
    cpu.write(state);
    state = cpu.read();

    //REQUIRE(cpu_state == 0b0101010101001010);
    REQUIRE(Cpu_state::Mode::get(state) == Cpu_state::Mode::MONITOR);
    REQUIRE(Cpu_state::A::get(state) == 1);
    REQUIRE(Cpu_state::B::get(state) == 1);
    REQUIRE(Cpu_state::C::get(state) == 1);
    REQUIRE(Cpu_state::Irq::get(state) == 0b101);
  }

  SECTION("clear")
  {

    Cpu_state::access_t state = cpu.read();

    Cpu_state::B::clear(state);
    Cpu_state::Irq::clear(state);
    cpu.write(state);
    state = cpu.read();

    //REQUIRE(cpu_state == 0b0000010001001010);
    REQUIRE(Cpu_state::B::get(state) == 0);
    REQUIRE(Cpu_state::Irq::get(state) == 0);
  }
}

TEST_CASE("placement new")
{

  uint16_t bare_reg;

  Cpu_state *cpu = new (&bare_reg) Cpu_state();

  SECTION("set")
  {
    Cpu_state::access_t state = cpu->read();
    Cpu_state::Mode::set(state, Cpu_state::Mode::MONITOR);
    Cpu_state::A::set(state, 1);
    Cpu_state::B::set(state);
    Cpu_state::C::set(state, 0xdddd);
    Cpu_state::Irq::set(state, 0xdddd);
    // Cpu_state::Invalid_bit::set(state, 0xdddd);
    // Cpu_state::Invalid_area::set(state, 0xdddd);
    cpu->write(state);
    state = cpu->read();

    REQUIRE(bare_reg == 0b0101010101001010);
    REQUIRE(Cpu_state::Mode::get(state) == Cpu_state::Mode::MONITOR);
    REQUIRE(Cpu_state::A::get(state) == 1);
    REQUIRE(Cpu_state::B::get(state) == 1);
    REQUIRE(Cpu_state::C::get(state) == 1);
    REQUIRE(Cpu_state::Irq::get(state) == 0b101);
  }

  SECTION("clear")
  {

    Cpu_state::access_t state = cpu->read();

    Cpu_state::B::clear(state);
    Cpu_state::Irq::clear(state);
    cpu->write(state);
    state = cpu->read();

    REQUIRE(bare_reg == 0b0000010001001010);
    REQUIRE(Cpu_state::B::get(state) == 0);
    REQUIRE(Cpu_state::Irq::get(state) == 0);
  }
}

TEST_CASE("Bits")
{

  uint32_t bare_reg;

  bit_test *bits = new (&bare_reg) bit_test();

  REQUIRE(bare_reg == 0b0);

  bits->set<bit_test::EN>();

  REQUIRE(bits->get<bit_test::EN>() == 1);

  bits->set<bit_test::RTS>();

  REQUIRE(bits->get<bit_test::RTS>() == 1);


  REQUIRE(bits->read() == 0b100000001);



  REQUIRE(bare_reg == 0b100000001);


}

