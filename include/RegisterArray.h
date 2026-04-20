#pragma once

#include "Fields.h"
#include <cstddef>

namespace regs {

template <typename Reg, unsigned Offset, unsigned Width, std::size_t Count,
          unsigned Stride = Width, unsigned StartByte = 0,
          typename Access = read_write,
          typename ValueType = typename Reg::reg_type>
struct FieldArray {
  using reg = Reg;
  using value_type = ValueType;

  static constexpr std::size_t count = Count;

  template <std::size_t Index>
  using field = Field<Reg, Offset + (Index * Stride), Width, StartByte, Access,
                      ValueType>;

  template <std::size_t Index>
  static value_type read(std::span<const std::byte> target) {
    static_assert(Index < count, "field index out of bounds");
    return field<Index>::read(target);
  }

  static value_type read(std::span<const std::byte> target,
                         std::size_t index) {
    return read_runtime<0>(target, index);
  }

  template <std::size_t Index>
  static void write(std::span<std::byte> target, value_type value) {
    static_assert(Index < count, "field index out of bounds");
    field<Index>::write(target, value);
  }

  static void write(std::span<std::byte> target, std::size_t index,
                    value_type value) {
    write_runtime<0>(target, index, value);
  }

  template <std::size_t Index, value_type Value>
  static bool is(std::span<const std::byte> target) {
    static_assert(Index < count, "field index out of bounds");
    return field<Index>::template is<Value>(target);
  }

 private:
  template <std::size_t Index>
  static value_type read_runtime(std::span<const std::byte> target,
                                 std::size_t index) {
    if (index == Index) {
      return field<Index>::read(target);
    }

    if constexpr (Index + 1 < count) {
      return read_runtime<Index + 1>(target, index);
    }

    ESCAD_ASSERT(false, "FieldArray runtime index out of range");
    return value_type{};
  }

  template <std::size_t Index>
  static void write_runtime(std::span<std::byte> target, std::size_t index,
                            value_type value) {
    if (index == Index) {
      field<Index>::write(target, value);
      return;
    }

    if constexpr (Index + 1 < count) {
      write_runtime<Index + 1>(target, index, value);
      return;
    }

    ESCAD_ASSERT(false, "FieldArray runtime index out of range");
  }
};

template <template <unsigned> typename Reg, unsigned Offset,
          std::size_t Count, unsigned Stride = sizeof(typename Reg<Offset>::reg_type)>
struct RegisterArray {
  static constexpr std::size_t count = Count;

  using reg_type = typename Reg<Offset>::reg_type;
  using reg_pack = typename Reg<Offset>::reg_pack;

  template <std::size_t Index>
  using reg = Reg<Offset + (Index * Stride)>;

  template <std::size_t Index>
  static reg<Index> at(reg_pack &pack) {
    static_assert(Index < count, "register index out of bounds");
    return reg<Index>{pack};
  }

  template <std::size_t Index>
  static reg_type read(reg_pack &pack) {
    static_assert(Index < count, "register index out of bounds");
    return at<Index>(pack).read();
  }

  static reg_type read(reg_pack &pack, std::size_t index) {
    return read_runtime<0>(pack, index);
  }

  template <std::size_t Index>
  static void write(reg_pack &pack, reg_type value) {
    static_assert(Index < count, "register index out of bounds");
    at<Index>(pack).write(value);
  }

  static void write(reg_pack &pack, std::size_t index, reg_type value) {
    write_runtime<0>(pack, index, value);
  }

 private:
  template <std::size_t Index>
  static reg_type read_runtime(reg_pack &pack, std::size_t index) {
    if (index == Index) {
      return at<Index>(pack).read();
    }

    if constexpr (Index + 1 < count) {
      return read_runtime<Index + 1>(pack, index);
    }

    ESCAD_ASSERT(false, "RegisterArray runtime index out of range");
    return reg_type{};
  }

  template <std::size_t Index>
  static void write_runtime(reg_pack &pack, std::size_t index,
                            reg_type value) {
    if (index == Index) {
      at<Index>(pack).write(value);
      return;
    }

    if constexpr (Index + 1 < count) {
      write_runtime<Index + 1>(pack, index, value);
      return;
    }

    ESCAD_ASSERT(false, "RegisterArray runtime index out of range");
  }
};

} // namespace regs