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

}  // namespace details

/**
 * @brief A region of width bits within a register
 *
 * @tparam offset offset to the first bit
 * @tparam width bit width of the field
 */
template <typename Reg, unsigned offset, unsigned width,
          typename Access = read_write,
          typename value_type = typename Reg::reg_type>
struct Field {
    using reg = Reg;
    // using reg_type = typename Reg::reg_type;

    using access = Access;

    using target_type = typename Reg::target_type;

    static_assert(width >= 1, "invalid width");
    static_assert(offset + width <= sizeof(target_type) * 8,
                  "invalid width/offset");

    // byte_offset
    static constexpr unsigned byte_offset = offset / 8;

    static constexpr unsigned bit_offset = offset % 8;

    static constexpr unsigned count_mask_bytes = sizeof(target_type);

    static constexpr auto maskBytes() {
        byte_array<count_mask_bytes> _maskBytes;

        int offset_ = bit_offset;
        int width_ = width;

        int bytes_masked = 0;

        for (unsigned i = 0; i < count_mask_bytes; i++) {
            std::cout << "i: " << i << " bit_offset_: " << offset_
                      << " byte_offset: " << byte_offset
                      << " width_: " << width_ << std::endl;

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

        std::cout << "MaskByte: " << _maskBytes << std::endl;

        return _maskBytes;
    }

    static constexpr auto read_masked(std::span<const std::byte> const target) {
        value_type result;

        std::cout << "read_masked - count_mask_bytes: " << count_mask_bytes << std::endl;

        byte_array<count_mask_bytes> Bytes;

        auto mask = maskBytes();

        for (unsigned i = 0; i < count_mask_bytes; i++) {
            // Bytes[i] = target[byte_offset + i] & mask[i];
            Bytes[i] = target[i] & mask[i];
        }

        std::cout << "read_masked - Bytes: " << Bytes << std::endl;

        auto shifted = array_shift_right(Bytes, offset);

        std::cout << "read_masked - Shifted: " << shifted << std::endl;

        result = std::bit_cast<value_type>(shifted);

        return result;
    }

    static constexpr void write_masked(std::span<std::byte> target, value_type value) {
        using ValueArray = byte_array<count_mask_bytes>;

        std::cout << "write_masked - count_mask_bytes: " << count_mask_bytes << std::endl;

        ValueArray value_bytes;

        value_bytes = std::bit_cast<ValueArray>(value);

        std::cout << "write_masked - value_bytes: " << value_bytes << std::endl;

        auto shifted = array_shift_left(value_bytes, offset);

        std::cout << "write_masked - shifted values: " << shifted << std::endl;

        auto mask = maskBytes();

        std::cout << "write_masked - mask: " << mask << std::endl;

        for (unsigned i = 0; i < count_mask_bytes; i++) {
            std::cout << "target[" << i << "] = " << target[i];
            target[i] = (target[i] & ~mask[i]) | (shifted[i] & mask[i]);
            std::cout << "--> " << target[i] << std::endl;
        }

        return;
    }

    static inline value_type read(std::span<const std::byte> const target) noexcept
        requires details::is_readable<access>
    {
        return read_masked(target);
    }

    static inline void write(std::span<std::byte> target, value_type value) noexcept
        requires details::is_writeable<access>
    {
        write_masked(target, value);
    }

    template <value_type value>
    static inline void write_constant(std::span<std::byte> target) noexcept
        requires details::is_writeable<access>
    {
        write_masked(target, value);
    }

    template <value_type value>
    static inline bool is(std::span<const std::byte> const target) noexcept
        requires details::is_readable<access>
    {
        return read_masked(target) == value;
    }
};
}  // namespace regs