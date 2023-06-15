
#include <RegisterPack.h>

#include <catch2/catch_all.hpp>

using namespace regs;

/**
 * @brief HeaderPack is a RegisterPack with 40 bytes
 *
 */
struct HeaderPack : public RegisterPack<40> {
  using RegisterPack::RegisterPack;
};

struct Magic0;
struct Magic0 : public PackedRegister<HeaderPack, Magic0, 0, uint32_t> {
  using Magic0_1 = Field<Magic0, 0, 8, read_only, uint8_t>;
  using Magic0_2 = Field<Magic0, 8, 8, read_only, uint8_t>;
  using Magic0_3 = Field<Magic0, 16, 8, read_only, uint8_t>;
  using Magic0_4 = Field<Magic0, 24, 8, read_only, uint8_t>;
};

struct Magic1;
struct Magic1 : public PackedRegister<HeaderPack, Magic1, 4, uint32_t> {
  using Magic1_1 = Field<Magic1, 0, 8, read_only, uint8_t>;
  using Magic1_2 = Field<Magic1, 8, 8, read_only, uint8_t>;
  using Magic1_3 = Field<Magic1, 16, 8, read_only, uint8_t>;
  using Magic1_4 = Field<Magic1, 24, 8, read_only, uint8_t>;
};

struct version;
struct version : public PackedRegister<HeaderPack, version, 8, uint32_t> {};

struct total_length;
struct total_length
    : public PackedRegister<HeaderPack, total_length, 12, uint32_t> {};

struct platform;
struct platform : public PackedRegister<HeaderPack, platform, 16, uint32_t> {};

struct frame_no;
struct frame_no : public PackedRegister<HeaderPack, frame_no, 20, uint32_t> {};

struct time_cpu_cycle;
struct time_cpu_cycle
    : public PackedRegister<HeaderPack, time_cpu_cycle, 24, uint32_t> {};

struct detect_obj_no;
struct detect_obj_no
    : public PackedRegister<HeaderPack, detect_obj_no, 28, uint32_t> {};

struct tlvs_no;
struct tlvs_no : public PackedRegister<HeaderPack, tlvs_no, 32, uint32_t> {};

struct sub_frame_no;
struct sub_frame_no
    : public PackedRegister<HeaderPack, sub_frame_no, 36, uint32_t> {};

std::byte raw_data[96] = {
    0x02_b, 0x01_b, 0x04_b, 0x03_b, 0x06_b, 0x05_b, 0x08_b, 0x07_b, // magic
    0x04_b, 0x00_b, 0x05_b, 0x03_b,                                 // version
    0x60_b, 0x00_b, 0x00_b, 0x00_b, // total length 96 bytes
    0xb6_b, 0xf3_b, 0x96_b, 0x3f_b, // platform
    0x1d_b, 0x00_b, 0x00_b, 0x00_b, // frame no 29
    0x7b_b, 0xee_b, 0x4b_b, 0xac_b, // time cpu cycle 2890657403
    0x03_b, 0x00_b, 0x00_b, 0x00_b, // detect obj no 3
    0x01_b, 0x00_b, 0x00_b, 0x00_b, // tlvs no 1
    0x00_b, 0x00_b, 0x00_b, 0x00_b, // sub frame no 0
    0x01_b, 0x00_b, 0x00_b, 0x00_b, // tlvs tag 1
    0x30_b, 0x00_b, 0x00_b, 0x00_b, // tlvs length 48 bytes
    0x3c_b, 0x0a_b, 0xa2_b, 0x3d_b, // x 0.07912108
    0xe3_b, 0x94_b, 0x1b_b, 0x3f_b, // y 0.6077406
    0x62_b, 0x89_b, 0x14_b, 0x3f_b, // z 0.5802213
    0x00_b, 0x00_b, 0x00_b, 0x00_b, // v 0
    0xf1_b, 0x0b_b, 0x3d_b, 0x3e_b, // x 0.184615865
    0x4a_b, 0x44_b, 0x2c_b, 0x3f_b, // y 0.672917
    0x5b_b, 0x0f_b, 0xf3_b, 0xbe_b, // z -0.474726528
    0x00_b, 0x00_b, 0x00_b, 0x00_b, // v 0
    0x3c_b, 0x0a_b, 0x22_b, 0xbf_b, 
    0x1b_b, 0x4e_b, 0x04_b, 0x3f_b,
    0xa6_b, 0x0d_b, 0x58_b, 0xbe_b, 
    0x00_b, 0x00_b, 0x00_b, 0x00_b};

TEST_CASE("RegSet", "[regs]") {

  HeaderPack *header_pack = new (&raw_data) HeaderPack(noInit{});

  Magic0 magic0(*header_pack);

  //STATIC_REQUIRE(sizeof(header_pack) == 40);

  REQUIRE(magic0.read<Magic0::Magic0_1>() == 2);
  REQUIRE(magic0.read<Magic0::Magic0_2>() == 1);
  REQUIRE(magic0.read<Magic0::Magic0_3>() == 4);
  REQUIRE(magic0.read<Magic0::Magic0_4>() == 3);

  total_length length(*header_pack);
  REQUIRE(length.read() == 6);


}
