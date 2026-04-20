#include <iostream>

#include <Register.h>
#include <RegisterArray.h>
#include <RegisterPack.h>

using namespace regs;

struct ByteSlots;
struct ByteSlots : Register<ByteSlots, uint32_t> {
  using Register::Register;

  // Four 8-bit values packed into one 32-bit register.
  using Slot = FieldArray<ByteSlots, 0, 8, 4, 8, 0, read_write, uint8_t>;
};

struct PortPack : RegisterPack<16> {
  using RegisterPack::RegisterPack;
};

template <unsigned Offset>
struct IOCR : PackedRegister<PortPack, IOCR<Offset>, Offset, uint32_t> {
  using PackedRegister<PortPack, IOCR<Offset>, Offset, uint32_t>::PackedRegister;

  // Each IOCR register configures four pins with 5-bit fields at bit 0/8/16/24.
  using PC = FieldArray<IOCR<Offset>, 0, 5, 4, 8, 0, read_write, uint32_t>;
};

// IOCR0, IOCR4, IOCR8, IOCR12 at byte offsets 0, 4, 8, 12.
using IOCRArray = RegisterArray<IOCR, 0, 4, 4>;

int main() {
  ByteSlots slots;

  slots.write<ByteSlots::Slot, 0>(0x11);
  slots.write<ByteSlots::Slot, 1>(0x22);
  slots.write<ByteSlots::Slot>(2, 0x33);
  slots.write<ByteSlots::Slot>(3, 0x44);

  std::cout << "slot[2] = 0x" << std::hex
            << static_cast<unsigned>(slots.read<ByteSlots::Slot, 2>())
            << std::endl;

  std::size_t runtime_index = 1;
  std::cout << "slot[" << runtime_index << "] = 0x" << std::hex
            << static_cast<unsigned>(slots.read<ByteSlots::Slot>(runtime_index))
            << std::endl;

  PortPack port;

  // Static register index + static pin-in-register index.
  auto iocr0 = IOCRArray::at<0>(port);
  iocr0.write<IOCR<0>::PC, 2>(0x10);

  // Runtime register index + runtime pin-in-register index.
  std::size_t iocr_index = 2;
  auto iocr_word = IOCRArray::read(port, iocr_index);
  std::cout << "IOCR[" << iocr_index << "] before = 0x" << std::hex << iocr_word
            << std::endl;

  auto iocr2 = IOCRArray::at<2>(port);
  iocr2.write<IOCR<8>::PC>(3, 0x12);

  iocr_word = IOCRArray::read<2>(port);
  std::cout << "IOCR[2] after = 0x" << std::hex << iocr_word << std::endl;

  return 0;
}
