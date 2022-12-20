#pragma once

#include "Bytes.h"
#include "Fields.h"

namespace regs {

/**
 * @brief Tag for no initailzing construction
 *
 */
struct noInit {};

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

  static constexpr size_t size = sizeof(Reg_type);

  using target_type = byte_array<size>;

 private:
  target_type _target;

 public:
  /**
   * @brief Default constructor with initialization
   *
   */
  explicit RegisterBase() : _target() {}

  /**
   * @brief Tagged constructor without initialization, to be used with placement
   * new
   *
   */
  explicit RegisterBase(noInit) {}

  /**
   * @brief Field read
   * 
   * @tparam TField 
   * @return requires 
   */
  template <typename TField>
    requires std::same_as<reg, typename TField::reg>
  reg_type read() {
    return TField::read(_target);
  }

  template <typename TField>
    requires std::same_as<reg, typename TField::reg>
  void write(reg_type value) {
    
    TField::write(_target, value);

  }


  template <typename TField, reg_type value>
    requires std::same_as<reg, typename TField::reg>
  void write() {
    //static_assert(std::is_same_v<reg, typename TField::reg>, "invalid Field");
    TField::template write_constant<value>(_target);
  }

  template <typename TField, reg_type value>
      requires std::same_as<reg, typename TField::reg>

  bool is() {
//    static_assert(std::is_same_v<reg, typename TField::reg>, "invalid Field");
    return TField::template is<value>(_target);
  }
};

}  // namespace regs