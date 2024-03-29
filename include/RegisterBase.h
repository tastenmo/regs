#pragma once

#include "Bytes.h"
#include "Fields.h"

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

  template <typename TField>
    requires std::same_as<reg, typename TField::reg>
  void write(TField::value_type value) {

    TField::write(static_cast<Derived *>(this)->span(), value);
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

  reg_type read() {
    return std::bit_cast<reg_type>(static_cast<Derived *>(this)->const_target());
  }

};

} // namespace regs