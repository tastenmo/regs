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

#include <catch2/catch_all.hpp>
#include <SingleRegister.h>

using namespace regs;

struct Cpu_state;

/**
 * Exemplary highly structured type for accessing 'cpu_state'
 */
struct Cpu_state : SingleRegister<uint16_t>
{

  using reg_type = SingleRegister<uint16_t>::reg_type;

  struct Mode : Field<reg_type, 0, 4, read_write>
  {
    enum
    {
      KERNEL = 0b1000,
      USER = 0b1001,
      MONITOR = 0b1010,
    };
  };

  struct A : Field<reg_type, 6, 1, read_write>
  {
  };

  struct B : Field<reg_type, 8, 1, read_write>
  {
  };

  struct C : Field<reg_type, 10, 1, read_write>
  {
  };

  struct Irq : Field<reg_type, 12, 3, read_write>
  {
  };

  // struct Invalid_bit  : Bitfield<18,1> { };
  // struct Invalid_area : Bitfield<15,4> { };
};

struct AccessStruct : SingleRegister<uint32_t>
{

  using reg_type = SingleRegister::reg_type;

  struct Nibble0 : Field<reg_type, 0, 4, read_only>{};
  struct Bit5 : Field<reg_type, 5, 1, read_only>{};
  struct Bit6 : Field<reg_type, 6, 1, read_write>{};

  struct Nibble1 : Field<reg_type, 8, 4, write_only>{};

};

TEST_CASE("regs_read_write_is")
{

  Cpu_state cpu;
  SECTION("set")
  {
    // Cpu_state::reg_type state = cpu.read();
    cpu.write<Cpu_state::Mode, Cpu_state::Mode::MONITOR>();

    cpu.write<Cpu_state::A, 1>();
    cpu.set<Cpu_state::B>();
    cpu.write<Cpu_state::C>(1);

    cpu.write<Cpu_state::Irq>(0xdddd);

    // REQUIRE(cpu_state == 0b0101010101001010);
    REQUIRE(cpu.read<Cpu_state::Mode>() == Cpu_state::Mode::MONITOR);
    REQUIRE(cpu.is<Cpu_state::A, 1>());
    REQUIRE(cpu.is<Cpu_state::B, 1>());
    REQUIRE(cpu.read<Cpu_state::C>() == 1);
    REQUIRE(cpu.is<Cpu_state::Irq, 0b101>());
  }

  SECTION("clear")
  {
    cpu.write<Cpu_state::Mode, Cpu_state::Mode::MONITOR>();

    cpu.write<Cpu_state::A, 1>();
    cpu.set<Cpu_state::B>();
    cpu.write<Cpu_state::C>(1);

    cpu.write<Cpu_state::Irq>(0xdddd);

    cpu.clear<Cpu_state::B>();
    cpu.clear<Cpu_state::Irq>();

    // REQUIRE(cpu_state == 0b0000010001001010);
    REQUIRE(cpu.read<Cpu_state::Mode>() == Cpu_state::Mode::MONITOR);
    REQUIRE(cpu.is<Cpu_state::A, 1>());
    REQUIRE(cpu.read<Cpu_state::B>() == 0);
    REQUIRE(cpu.read<Cpu_state::C>() == 1);
    REQUIRE(cpu.read<Cpu_state::Irq>() == 0);
  }
}

TEST_CASE("placement new")
{

  uint16_t bare_reg;

  Cpu_state *cpu = new (&bare_reg) Cpu_state();

  SECTION("set")
  {

    cpu->write<Cpu_state::Mode>(Cpu_state::Mode::MONITOR);
    cpu->set<Cpu_state::A>();
    cpu->set<Cpu_state::B>();
    cpu->write<Cpu_state::C>(0xdddd);
    cpu->write<Cpu_state::Irq, 0xdddd>();
    // Cpu_state::Invalid_bit::set(state, 0xdddd);
    // Cpu_state::Invalid_area::set(state, 0xdddd);
    // cpu->write(state);
    // state = cpu->read();

    REQUIRE(bare_reg == 0b0101010101001010);
    REQUIRE(cpu->is<Cpu_state::Mode, Cpu_state::Mode::MONITOR>());
    REQUIRE(cpu->is<Cpu_state::A, 1>());
    REQUIRE(cpu->is<Cpu_state::B, 1>());
    REQUIRE(cpu->is<Cpu_state::C, 1>());
    REQUIRE(cpu->is<Cpu_state::Irq, 0b101>());
  }

  SECTION("clear")
  {
    bare_reg = 0b0101010101001010;

    cpu->clear<Cpu_state::B>();
    cpu->clear<Cpu_state::Irq>();

    REQUIRE(bare_reg == 0b0000010001001010);
    REQUIRE(cpu->is<Cpu_state::B, 0>());
    REQUIRE(cpu->is<Cpu_state::Irq, 0>());
  }
}

TEST_CASE("ro rw wo")
{

  uint32_t bare = 0;

  AccessStruct *acc = new (&bare) AccessStruct();

  REQUIRE(acc->is<AccessStruct::Bit5, 0>());

  //REQUIRE(acc->set<AccessStruct::Bit5, 0>());

  

  
}
