
#include <RegisterPack.h>

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

  using Byte2 = Field<Register0, 8, 8, read_write, uint8_t>;

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
