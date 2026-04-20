# regs
C++ Register access

## Usage Examples

### 1) Array of fields inside one register

This models four packed bytes in one 32-bit register. You can access with both
compile-time and runtime indexes.

```cpp
#include <Register.h>
#include <RegisterArray.h>

using namespace regs;

struct ByteSlots;
struct ByteSlots : Register<ByteSlots, uint32_t> {
	using Register::Register;
	using Slot = FieldArray<ByteSlots, 0, 8, 4, 8, 0, read_write, uint8_t>;
};

ByteSlots slots;

slots.write<ByteSlots::Slot, 0>(0x11);  // static index
slots.write<ByteSlots::Slot>(3, 0x44);  // runtime index

auto s0 = slots.read<ByteSlots::Slot, 0>();
auto s3 = slots.read<ByteSlots::Slot>(3);
```

### 2) Array of registers (IOCR0/4/8/12 style)

This models a PORT-like block where four IOCR registers are laid out with a
fixed stride of 4 bytes. Each IOCR register contains four pin-control fields.

```cpp
#include <RegisterPack.h>
#include <RegisterArray.h>

using namespace regs;

struct PortPack : RegisterPack<16> {
	using RegisterPack::RegisterPack;
};

template <unsigned Offset>
struct IOCR : PackedRegister<PortPack, IOCR<Offset>, Offset, uint32_t> {
	using PackedRegister<PortPack, IOCR<Offset>, Offset, uint32_t>::PackedRegister;
	using PC = FieldArray<IOCR<Offset>, 0, 5, 4, 8, 0, read_write, uint32_t>;
};

using IOCRArray = RegisterArray<IOCR, 0, 4, 4>;

PortPack port;

// IOCR0, pin group entry 2
auto iocr0 = IOCRArray::at<0>(port);
iocr0.write<IOCR<0>::PC, 2>(0x10);

// Runtime access: IOCR index and pin entry index
auto iocr2 = IOCRArray::at<2>(port);
iocr2.write<IOCR<8>::PC>(3, 0x12);
auto raw = IOCRArray::read(port, 2);
```

### 3) Runtime pin -> IOCR mapping

For a port pin number in range 0..15:

- IOCR register index = pin / 4
- Pin position inside that IOCR register = pin % 4

```cpp
std::size_t pin = 11;
std::size_t iocr_index = pin / 4;       // 2 -> IOCR8
std::size_t pin_slot = pin % 4;         // 3 -> PC field at bits 24..28

auto iocr = IOCRArray::at<2>(port);     // compile-time register lookup
iocr.write<IOCR<8>::PC>(pin_slot, 0x12);
```
