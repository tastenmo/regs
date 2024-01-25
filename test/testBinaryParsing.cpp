
#include "catch2/matchers/catch_matchers_floating_point.hpp"
#include <RegisterPack.h>

#include <catch2/catch_all.hpp>
#include <sys/types.h>

using namespace regs;
using namespace Catch::Matchers;

/**
 * @brief FrameHeader is a RegisterPack with 40 bytes
 *
 */
struct FrameHeader : public RegisterPack<40> {
  using RegisterPack::RegisterPack;

  struct Magic0 : public PackedRegister<FrameHeader, Magic0, 0, uint32_t> {};
  struct Magic1 : public PackedRegister<FrameHeader, Magic1, 4, uint32_t> {};
  struct Version : public PackedRegister<FrameHeader, Version, 8, uint32_t> {};
  struct total_length
    : public PackedRegister<FrameHeader, total_length, 12, uint32_t> {};
  struct platform : public PackedRegister<FrameHeader, platform, 16, uint32_t> {};
  struct time_cpu_cycle
    : public PackedRegister<FrameHeader, time_cpu_cycle, 24, uint32_t> {};
  struct detect_obj_no
    : public PackedRegister<FrameHeader, detect_obj_no, 28, uint32_t> {};
  struct tlvs_no : public PackedRegister<FrameHeader, tlvs_no, 32, uint32_t> {};
  struct sub_frame_no
    : public PackedRegister<FrameHeader, sub_frame_no, 36, uint32_t> {};

};

struct TlvHeader :public RegisterPack<8> {
  using RegisterPack::RegisterPack;

  struct TlvType : public PackedRegister<TlvHeader, TlvType, 0, uint32_t> {};
  struct Length : public PackedRegister<TlvHeader, Length, 4, uint32_t> {};
};

struct DetectedPoints : public RegisterPack<16> {
  using RegisterPack::RegisterPack;

  struct x : public PackedRegister<DetectedPoints, x, 0, float> {};
  struct y : public PackedRegister<DetectedPoints, y, 4, float> {};
  struct z : public PackedRegister<DetectedPoints, z, 8, float> {};
  struct v : public PackedRegister<DetectedPoints, v, 12, float> {};
};

struct SideInfo : public RegisterPack<4> {
  using RegisterPack::RegisterPack;

  struct Point : public PackedRegister<SideInfo, Point, 0, uint32_t> {
    using snr = Field<Point, 0, 16, 0, read_only, uint16_t>;
    using noise = Field<Point, 16, 16, 0, read_only, uint16_t>;


  };
};

template<typename T> struct Point3D{

  Point3D(T x, T y, T z) : x(x), y(y), z(z) {}

  T x;
  T y;
  T z;
};

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

  FrameHeader *header_pack = new (&raw_data) FrameHeader(noInit{});

  //HeaderPack::Magic(*header_pack).read() == 0x02030405;

  REQUIRE(FrameHeader::Magic0(*header_pack).read() == 0x03040102);
  REQUIRE(FrameHeader::Magic1(*header_pack).read() == 0x07080506);

  REQUIRE(FrameHeader::total_length(*header_pack).read() == 96);

  uint32_t detect_obj_no = FrameHeader::detect_obj_no(*header_pack).read();

  REQUIRE(detect_obj_no == 3);

  TlvHeader *tlv_header = new (&raw_data[40]) TlvHeader(noInit{});

  REQUIRE(TlvHeader::TlvType(*tlv_header).read() == 1);
  REQUIRE(TlvHeader::Length(*tlv_header).read() == 48);

  std::vector<Point3D<float>> points;

  std::vector<float> velocities;

  for(uint32_t index = 48; index < 48 + (detect_obj_no * 16) ; index += 16) {
    DetectedPoints *detected_points = new (&raw_data[index]) DetectedPoints(noInit{});

    points.emplace_back(
        DetectedPoints::x(*detected_points).read(),
        DetectedPoints::y(*detected_points).read(),
        DetectedPoints::z(*detected_points).read()
        );
    
    velocities.emplace_back(DetectedPoints::v(*detected_points).read());

  }

  REQUIRE_THAT(points[0].x, WithinAbs(0.07, 0.08));
  REQUIRE_THAT(points[0].y, WithinAbs(0.6, 0.7));
  REQUIRE_THAT(points[0].z, WithinAbs(0.5, 0.6));





}
