/**
 * @file Register.h
 * @author Martin Heubuch (martin.heubuch@escad.de)
 * @brief
 * @version 0.1
 * @date 2022-07-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "Fields.h"

namespace regs
{

  /**
   * @brief Register with bitfields
   *
   * @tparam reg_type underlying type
   */

  template <typename Reg_type>
  class SingleRegister
  {
  public:
    /**
     * @brief underlying access type
     *
     */
    using reg_type = Reg_type;

  private:
    reg_type _target;

  public:
    template <typename Field>
    reg_type read() { return Field::read(const_raw()); }

    template <typename Field, reg_type value>
    bool is() { return Field::template is<value>(const_raw()); }

    template <typename Field>
    void write(reg_type value) { Field::write(raw(), value); }

    template <typename Field, reg_type value>
    void write() { Field::template write<value>(raw()); }

    template <typename Field>
    void set() { Field::set(raw()); }

    template <typename Field>
    void clear() { Field::clear(raw()); }

    template <typename Field>
    void toggle() { Field::toggle(raw()); }



    const reg_type &const_raw()
    {
      return _target;
    }

    reg_type &raw()
    {
      return _target;
    }
  };

}