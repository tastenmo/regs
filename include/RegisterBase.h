#pragma once

#include "Bytes.h"
#include "Fields.h"
#include <algorithm>

namespace regs {

/**
 * @brief Register with bitfields
 *
 * @tparam reg_type underlying type
 */

template <typename Derived, typename Reg_type> class RegisterBase {
public:
  /**
   * @brief underlying access type
   *
   */
  using reg = Derived;
  using reg_type = Reg_type;

public:
  /**
   * @brief Field read
   *
   * @tparam TField
   * @return requires
   */
  template <typename TField>
    requires std::same_as<reg, typename TField::reg>
  TField::value_type read() {
    return TField::read(static_cast<Derived *>(this)->span());
  }

  template <typename TArray, std::size_t Index>
    requires std::same_as<reg, typename TArray::reg>
  typename TArray::value_type read() {
    return TArray::template read<Index>(static_cast<Derived *>(this)->span());
  }

  template <typename TArray>
    requires std::same_as<reg, typename TArray::reg>
  typename TArray::value_type read(std::size_t index) {
    return TArray::read(static_cast<Derived *>(this)->span(), index);
  }

  template <typename TField>
    requires std::same_as<reg, typename TField::reg>
  void write(TField::value_type value) {

    TField::write(static_cast<Derived *>(this)->span(), value);
  }

  template <typename TArray, std::size_t Index>
    requires std::same_as<reg, typename TArray::reg>
  void write(typename TArray::value_type value) {
    TArray::template write<Index>(static_cast<Derived *>(this)->span(), value);
  }

  template <typename TArray>
    requires std::same_as<reg, typename TArray::reg>
  void write(std::size_t index, typename TArray::value_type value) {
    TArray::write(static_cast<Derived *>(this)->span(), index, value);
  }

  template <typename TField, TField::value_type value>
    requires std::same_as<reg, typename TField::reg>
  void write() {
    // static_assert(std::is_same_v<reg, typename TField::reg>, "invalid
    // Field");
    TField::template write_constant<value>(
        static_cast<Derived *>(this)->span());
  }

  template <typename TField, TField::value_type value>
    requires std::same_as<reg, typename TField::reg>

  bool is() {
    //    static_assert(std::is_same_v<reg, typename TField::reg>, "invalid
    //    Field");
    return TField::template is<value>(static_cast<Derived *>(this)->span());
  }

  template <typename TArray, std::size_t Index, typename TArray::value_type value>
    requires std::same_as<reg, typename TArray::reg>
  bool is() {
    return TArray::template is<Index, value>(
        static_cast<Derived *>(this)->span());
  }

  reg_type read() {
    return std::bit_cast<reg_type>(static_cast<Derived *>(this)->const_target());
  }

  void write(reg_type value) {
    auto bytes = std::bit_cast<byte_array<sizeof(reg_type)>>(value);
    auto target = static_cast<Derived *>(this)->span();
    std::copy(bytes.begin(), bytes.end(), target.begin());
  }

};

} // namespace regs