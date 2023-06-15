/**
 * @file Bytes.h
 * @author Martin Heubuch (martin.heubuch@escad.de)
 * @brief
 * @version 0.1
 * @date 2022-12-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <cstdint>
#include <cstddef>
#include <iostream>
#include <limits>
#include <utility>

#include "Utility.h"

namespace regs {

/**
 * @brief result type for shift with carry
 *
 */
using shift_with_carry = std::pair<std::byte, bool>;

/**
 * @brief shift right one bit with carry
 *
 * @param src
 * @return constexpr shift_with_carry
 */
constexpr shift_with_carry shift_right(std::byte src) noexcept {
  return std::make_pair(src >> 1, (src & std::byte{0x01}) == std::byte{0x01});
}

/**
 * @brief shift left one bit with carry
 * 
 * @param src 
 * @return constexpr shift_with_carry 
 */
constexpr shift_with_carry shift_left(std::byte src) noexcept {
  return std::make_pair(src << 1, (src & std::byte{0x80}) == std::byte{0x80});
}

/**
 * @brief stream operator for printing std::byte
 * 
 * @param output 
 * @param src 
 * @return std::ostream& 
 */
std::ostream& operator<<(std::ostream& output, std::byte src) {
  output << std::to_integer<unsigned>(src);
  return output;
}

/**
 * @brief byte_array based on std::array
 * 
 * @tparam Size 
 */
template <std::size_t Size>
struct byte_array {
  std::byte _bytes[Size ? Size : 1];

  using value_type = std::byte;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = pointer;
  using const_iterator = const_pointer;

  constexpr pointer data() noexcept { return _bytes; }
  constexpr const_pointer data() const noexcept { return _bytes; }

  constexpr void fill(value_type v) noexcept {
    for (auto& b : *this) {
      b = v;
    }
  }

  constexpr size_type size() const noexcept { return Size; }
  constexpr bool empty() const noexcept { return size() == 0; }
  constexpr size_type max_size() const noexcept { return size(); }

  constexpr iterator begin() noexcept { return data(); }
  constexpr const_iterator begin() const noexcept { return data(); }
  constexpr const_iterator cbegin() const noexcept { return begin(); }
  constexpr iterator end() noexcept { return data() + size(); }
  constexpr const_iterator end() const noexcept { return data() + size(); }
  constexpr const_iterator cend() const noexcept { return end(); }

  constexpr reference operator[](size_type idx) noexcept {
    ESCAD_ASSERT(idx < size(), "Past-the-end access of byte_array");
    return _bytes[idx];
  }

  constexpr const_reference operator[](size_type idx) const noexcept {
    ESCAD_ASSERT(idx < size(), "Past-the-end access of byte_array");
    return _bytes[idx];
  }

  friend std::ostream& operator<<(std::ostream& output, const byte_array& arr) {
    for (value_type element : arr) {
      output << element << ", ";
    }

    return output;
  }
};

/**
 * @brief 
 * 
 * @tparam T 
 * @tparam Bs 
 */
template <std::same_as<std::byte> T, std::same_as<std::byte>... Bs>
byte_array(T, Bs...) -> byte_array<sizeof...(Bs) + 1>;

template <std::size_t Size, class IntegerType, std::size_t ToSize = Size>
constexpr byte_array<ToSize> array_shift_right(const byte_array<Size>& src,
                                             IntegerType shift) noexcept {
    byte_array<Size> result = src;
    byte_array<ToSize> out;

    // std::cout << "shift: " << shift << ", size: " << Size << std::endl;

    bool carry[Size];

    for (IntegerType i = 0; i < shift; i++) {
        // std::cout << "iteration i: " << i << std::endl;

        for (int j = (int)Size - 1; j >= 0; j--) {
            // std::cout << "iteration j: " << j << std::endl;

            auto shifted = shift_right(result[j]);

            carry[j] = shifted.second;
            // std::cout << "byte[" << j << "] = 0x" << std::hex <<
            // shifted.first << " carry "
            //           << carry[j] << std::endl;

            if (j < ((int)Size) - 1) {
                if (carry[j + 1]) {
                    shifted.first |= std::byte{0x80};
                }
            }
            result[j] = shifted.first;

            // std::cout << "result[" << j << "] = 0x" << std::hex <<
            // shifted.first << " carry "
            //           << carry[j] << std::endl;
        }
    }
    std::copy(result.begin(), result.begin() + ToSize, out.begin());

    return out;
}

template <std::size_t Size, class IntegerType>
constexpr byte_array<Size> array_shift_left(const byte_array<Size>& src,
                                            IntegerType shift) noexcept {
    byte_array<Size> result = src;

    // std::cout << "shift: " << shift << ", size: " << Size << std::endl;

    bool carry[Size];

    for (IntegerType i = 0; i < shift; i++) {
        // std::cout << "iteration i: " << i << std::endl;

        for (unsigned j = 0; j < Size; j++) {
            // std::cout << "iteration j: " << j << std::endl;

            auto shifted = shift_left(result[j]);
            carry[j] = shifted.second;

            if (j > 0) {
                if (carry[j - 1]) {
                    shifted.first |= std::byte{0x01};
                }
            }

            result[j] = shifted.first;

            // std::cout << "result[" << j << "] = 0x" << std::hex <<
            // shifted.first << " carry "
            //           << carry[j] << std::endl;
        }
    }

    return result;
}

constexpr std::byte operator""_b(unsigned long long int value) noexcept {
  return std::byte{static_cast<unsigned char>(value)};
}

}  // namespace regs
