
#include <RegisterPack.h>
#include <RegisterArray.h>

#include <catch2/catch_all.hpp>

using namespace regs;

struct TestRegisterPack : public RegisterPack<8> {
  using RegisterPack::RegisterPack;
};

struct Register0;
struct Register0
    : public PackedRegister<TestRegisterPack, Register0, 0, uint32_t> {

  using Bool1 = Field<Register0, 0, 1>;
  using Bool2 = Field<Register0, 1, 1>;
  using Bool3 = Field<Register0, 2, 1>;

  using Bits1 = Field<Register0, 4, 3>;

  using Byte2 = Field<Register0, 8, 8, 0, read_write, uint8_t>;

  using Bits2 = Field<Register0, 16, 4>;

  using Nibble1 = Field<Register0, 20, 6>;
};

struct Register1;
struct Register1
    : public PackedRegister<TestRegisterPack, Register1, 4, uint8_t> {
  using Bool1 = Field<Register1, 0, 1>;
  using Bool2 = Field<Register1, 1, 1>;
  using Bool3 = Field<Register1, 2, 1>;

  using Nibble1 = Field<Register1, 3, 5>;
};

template <unsigned Offset>
struct ArrayRegister
    : public PackedRegister<TestRegisterPack, ArrayRegister<Offset>, Offset,
                            uint8_t> {
  using PackedRegister<TestRegisterPack, ArrayRegister<Offset>, Offset,
                       uint8_t>::PackedRegister;

  using Value = Field<ArrayRegister<Offset>, 0, 8, 0, read_write, uint8_t>;
};

using ByteRegisters = RegisterArray<ArrayRegister, 0, 8, 1>;

std::byte raw_set[8] = {0x03_b, 0x11_b, 0_b, 0_b, 1_b, 0_b, 0_b, 0_b};

TEST_CASE("RegSet", "[regs]") {

  TestRegisterPack *reg_set = new (&raw_set) TestRegisterPack(noInit{});;

  Register0 reg0(*reg_set);

  STATIC_REQUIRE(sizeof(reg_set) == 8);

  REQUIRE(reg0.is<Register0::Bool1, true>());
  REQUIRE(reg0.is<Register0::Bool2, true>());
  REQUIRE(reg0.is<Register0::Bool3, false>());
  REQUIRE(reg0.read<Register0::Bits1>() == 0);
  REQUIRE(reg0.read<Register0::Byte2>() == 0x11);

  auto result = reg0.read<Register0::Byte2>();

  STATIC_REQUIRE(std::is_same_v<decltype(result), uint8_t>);

  Register1 reg1(*reg_set);

  REQUIRE(reg1.is<Register1::Bool1, true>());
  REQUIRE(reg1.is<Register1::Bool2, false>());
  REQUIRE(reg1.is<Register1::Bool3, false>());


}

TEST_CASE("RegisterArray", "[regs]") {
  std::byte raw_array_pack[8] = {0x10_b, 0x20_b, 0x30_b, 0x40_b,
                                 0x50_b, 0x60_b, 0x70_b, 0x80_b};

  TestRegisterPack *reg_set = new (&raw_array_pack) TestRegisterPack(noInit{});

  auto reg_static = ByteRegisters::at<2>(*reg_set);
  REQUIRE(reg_static.read<ArrayRegister<2>::Value>() == 0x30);

  REQUIRE(ByteRegisters::read<3>(*reg_set) == 0x40);
  REQUIRE(ByteRegisters::read(*reg_set, 4) == 0x50);

  ByteRegisters::write<1>(*reg_set, 0x2A);
  REQUIRE(ByteRegisters::read<1>(*reg_set) == 0x2A);

  ByteRegisters::write(*reg_set, 6, 0x6A);
  REQUIRE(ByteRegisters::read(*reg_set, 6) == 0x6A);
}
