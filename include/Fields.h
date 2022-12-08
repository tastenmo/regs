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

#include "Access.h"

namespace regs {
/**
 * @brief A region of width bits within a register
 *
 * @tparam offset offset to the first bit
 * @tparam width bit width of the field
 */
template <typename Register, unsigned offset, unsigned width,
          typename Access = read_write>
struct Field {
  using reg = Register;
  using reg_type = typename Register::reg_type;
  using access = Access;

  static_assert(width >= 1, "invalid width");
  static_assert(offset + width <= sizeof(reg_type) * 8, "invalid width/offset");

  explicit Field(Register const& reg) : Register(reg) {}

  /**
   * @brief shifted mask of this field
   *
   */
  static constexpr reg_type mask =
      reg_type((((reg_type)1 << width) - 1) << offset);

  static inline reg_type read(const reg_type& target) noexcept {
    return access::template read<reg_type, mask, offset>(target);
  }

  template <reg_type value>
  static inline bool is(const reg_type& target) noexcept {
    return access::template is<reg_type, mask, offset, value>(target);
  }

  static inline void write(reg_type& target, reg_type value) noexcept {
    access::template write<reg_type, mask, offset>(target, value);
  }

  template <reg_type value>
  static inline void write_constant(reg_type& target) noexcept {
    access::template write<reg_type, mask, offset, value>(target);
  }

  static inline void set(reg_type& target) noexcept {
    access::template set<reg_type, mask>(target);
  }

  static inline void clear(reg_type& target) noexcept {
    access::template clear<reg_type, mask>(target);
  }

  static inline void toggle(reg_type& target) noexcept {
    access::template toggle<reg_type, mask>(target);
  }
};
}  // namespace regs