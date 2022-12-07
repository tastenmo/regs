#pragma once

#include "Fields.h"

namespace regs {

/**
 * @brief Register with bitfields
 *
 * @tparam reg_type underlying type
 */

template <typename Reg, typename Reg_type>
class RegisterBase {
 public:
  /**
   * @brief underlying access type
   *
   */
  using reg = Reg;
  using reg_type = Reg_type;

 private:
  reg_type _target;

 public:
  // template <typename T, template <typename, typename> class Cont >
  template <typename TField>
  reg_type read() {
    static_assert(std::is_same_v<reg, typename TField::reg>, "invalid Field");
    return TField::read(_target);
  }

  template <typename TField>
  void write(reg_type value) {
    static_assert(std::is_same_v<reg, typename TField::reg>, "invalid Field");
    TField::write(_target, value);
    // fields.write(_target, value);
  }

  //    template <typename... TFields, typename... Args>
  //    void write(Args... values) {
  //        write<TFields...>(values...);
  //    }

  template <typename TField, reg_type value>
  void write() {
    static_assert(std::is_same_v<reg, typename TField::reg>, "invalid Field");

    TField::template write_constant<value>(_target);
  }

  template <typename TField, reg_type value>
  bool is() {
    static_assert(std::is_same_v<reg, typename TField::reg>, "invalid Field");
    return TField::template is<value>(_target);
  }
};

}  // namespace regs