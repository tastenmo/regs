/**
 * @file Fields.h
 * @author Martin Heubuch (martin.heubuch@escad.de)
 * @brief
 * @version 0.1
 * @date 2022-11-18
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "Bytes.h"
#include <bit>
#include <span>

namespace regs {

struct read_only {};

struct read_write : read_only {};

struct write_only {};

namespace details {

template <typename T>
concept is_readable =
    std::same_as<T, read_only> || std::derived_from<T, read_only>;

template <typename T>
concept is_writeable = std::is_same_v<T, read_write>;

template <typename T>
concept is_writeonly = std::is_same_v<T, write_only>;

template <unsigned bit_offset, unsigned width>
concept is_trivially_accessible = bit_offset == 0 && (width % 8) == 0;

} // namespace details

/**
 * @brief A region of width bits within a register
 *
 * @tparam offset offset to the first bit
 * @tparam width bit width of the field
 */
template <typename Reg, unsigned offset, unsigned width,
          unsigned start_byte = 0, typename Access = read_write,
          typename Value_type = typename Reg::reg_type>
struct Field {
  using reg = Reg;
  // using reg_type = typename Reg::reg_type;

  using access = Access;

  using value_type = Value_type;

  static constexpr size_t value_size = sizeof(value_type);

  using target_type = typename Reg::target_type;

  static_assert(width >= 1, "invalid width");
  static_assert(offset + width <= sizeof(target_type) * 8,
                "invalid width/offset");

  // byte_offset
  static constexpr unsigned byte_offset = start_byte + offset / 8;

  static constexpr unsigned bit_offset = offset % 8;

  static constexpr unsigned byte_count = ((bit_offset + width + 7) / 8);

  static constexpr unsigned count_mask_bytes = sizeof(target_type);

  static constexpr auto maskBytes() {
    byte_array<count_mask_bytes> _maskBytes;

    int offset_ = bit_offset;
    int width_ = width;

    int bytes_masked = 0;

    for (unsigned i = 0; i < count_mask_bytes; i++) {
      if (i >= byte_offset) {
        uint8_t mask_ = ((1 << width_) - 1) << offset_;

        _maskBytes[i] = std::byte{mask_};

        bytes_masked++;

        // clear the offset and shrink size for a second byte
        offset_ = 0;
        width_ = bit_offset + width - (bytes_masked * 8);
        if (width_ <= 0) {
          width_ = 0;
        }

      } else {
        _maskBytes[i] = std::byte{0x00};
      }
    }

    return _maskBytes;
  }

  static constexpr auto read_masked(std::span<const std::byte> const target)
    requires std::integral<value_type>
  {
    value_type result;

    byte_array<count_mask_bytes> Bytes;

    auto mask = maskBytes();

    for (unsigned i = 0; i < count_mask_bytes; i++) {
      // Bytes[i] = target[byte_offset + i] & mask[i];
      Bytes[i] = target[i] & mask[i];
    }

    auto shifted = array_shift_right<count_mask_bytes, unsigned, value_size>(
        Bytes, offset);

    result = std::bit_cast<value_type>(shifted);

    return result;
  }

  static constexpr auto read_masked(std::span<const std::byte> const target)
    requires std::is_enum_v<value_type>
  {
    // value_type result;

    byte_array<count_mask_bytes> Bytes;

    auto mask = maskBytes();

    for (unsigned i = 0; i < count_mask_bytes; i++) {
      // Bytes[i] = target[byte_offset + i] & mask[i];
      Bytes[i] = target[i] & mask[i];
    }

    auto shifted = array_shift_right(Bytes, offset);

    auto number = std::bit_cast<std::underlying_type_t<value_type>>(shifted);

    return value_type{number};
  }

  static constexpr auto read_trivial(std::span<const std::byte> const target)
    requires std::integral<value_type>
  {
    value_type result;

    auto Bytes = to_byte_array<byte_count>(target.subspan(byte_offset, byte_count));
    //auto Bytes = to_byte_array<byte_count>(target);

    result = std::bit_cast<value_type>(Bytes);

    return result;
  }

  static constexpr auto read_trivial(std::span<const std::byte> const target)
    requires std::is_enum_v<value_type>
  {
    auto number = std::bit_cast<std::underlying_type_t<value_type>>(
        target.subspan(byte_offset, byte_count));

    return value_type{number};
  }

  static constexpr void write_masked(std::span<std::byte> target,
                                     value_type value)
    requires std::integral<value_type>
  {
    using ValueArray = byte_array<value_size>;

    ValueArray values;

    values = std::bit_cast<ValueArray>(value);

    byte_array<count_mask_bytes> value_bytes;

    std::copy(values.begin(), values.begin() + value_size, value_bytes.begin());

    auto shifted = array_shift_left(value_bytes, offset);

    auto mask = maskBytes();

    for (unsigned i = 0; i < count_mask_bytes; i++) {
      target[i] = (target[i] & ~mask[i]) | (shifted[i] & mask[i]);
    }

    return;
  }

  static constexpr void write_masked(std::span<std::byte> target,
                                     value_type value)
    requires std::is_enum_v<value_type>
  {
    using ValueArray = byte_array<count_mask_bytes>;

    ValueArray value_bytes;

    auto number = std::bit_cast<std::underlying_type_t<value_type>>(value);

    value_bytes = std::bit_cast<ValueArray>(number);

    auto shifted = array_shift_left(value_bytes, offset);

    auto mask = maskBytes();

    for (unsigned i = 0; i < count_mask_bytes; i++) {
      target[i] = (target[i] & ~mask[i]) | (shifted[i] & mask[i]);
    }

    return;
  }

  static constexpr void write_trivial(std::span<std::byte> target,
                                      value_type value)
    requires std::integral<value_type>
  {
    using ValueArray = byte_array<byte_count>;

    ValueArray values;

    values = std::bit_cast<ValueArray>(value);

    auto sub_target = target.subspan(byte_offset, byte_count);

    std::copy(values.begin(), values.begin() + byte_count, sub_target.begin());

    return;
  }

  static constexpr void write_trivial(std::span<std::byte> target,
                                      value_type value)
    requires std::is_enum_v<value_type>
  {
    using ValueArray = byte_array<byte_count>;

    ValueArray values;

    auto number = std::bit_cast<std::underlying_type_t<value_type>>(value);

    values = std::bit_cast<ValueArray>(number);
    auto sub_target = target.subspan(byte_offset, byte_count);

    std::copy(values.begin(), values.begin() + byte_count, sub_target.begin());

    return;
  }

  static inline value_type
  read(std::span<const std::byte> const target) noexcept
    requires details::is_readable<access> &&
             (!details::is_trivially_accessible<bit_offset, width>)
  {
    return read_masked(target);
  }

  static inline value_type
  read(std::span<const std::byte> const target) noexcept
    requires details::is_readable<access> &&
             details::is_trivially_accessible<bit_offset, width>
  {
    return read_trivial(target);
  }

  static inline void write(std::span<std::byte> target,
                           value_type value) noexcept
    requires details::is_writeable<access> &&
             (!details::is_trivially_accessible<bit_offset, width>)
  {
    write_masked(target, value);
  }

    static inline void write(std::span<std::byte> target,
                           value_type value) noexcept
    requires details::is_writeable<access> &&
             details::is_trivially_accessible<bit_offset, width>
  {
    write_trivial(target, value);
  }

  template <value_type value>
  static inline void write_constant(std::span<std::byte> target) noexcept
    requires details::is_writeable<access> &&
             (!details::is_trivially_accessible<bit_offset, width>)
  {
    write_masked(target, value);
  }

  template <value_type value>
  static inline void write_constant(std::span<std::byte> target) noexcept
    requires details::is_writeable<access> &&
             details::is_trivially_accessible<bit_offset, width>
  {
    write_trivial(target, value);
  }

  template <value_type value>
  static inline bool is(std::span<const std::byte> const target) noexcept
    requires details::is_readable<access>  &&
             (!details::is_trivially_accessible<bit_offset, width>)
  {
    return read_masked(target) == value;
  }

    template <value_type value>
  static inline bool is(std::span<const std::byte> const target) noexcept
    requires details::is_readable<access>  &&
             details::is_trivially_accessible<bit_offset, width>
  {
    return read_trivial(target) == value;
  }
};
} // namespace regs