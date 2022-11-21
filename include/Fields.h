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

namespace regs
{
    /**
     * @brief A region of width bits within a register
     *
     * @tparam offset offset to the first bit
     * @tparam width bit width of the field
     */
    template <typename Reg_type, unsigned offset, unsigned width, typename Access>
    struct Field
    {

        using reg_type = Reg_type;
        using access = Access;

        static_assert(width >= 1, "invalid width");
        static_assert(offset + width <= sizeof(reg_type) * 8,
                      "invalid width/offset");

        /**
         * @brief shifted mask of this field
         *
         */
        static constexpr reg_type mask = reg_type((((reg_type)1 << width) - 1) << offset);

        static reg_type read(const reg_type& target) noexcept
        {
            return access::template read<reg_type, mask, offset>(
                target);
        }

        template <reg_type value>
        static bool is(const reg_type& target) noexcept
        {
            return access::template is<reg_type, mask, offset, value>(
                target);
        }

        static void write(reg_type& target, reg_type value) noexcept
        {
            access::template write<reg_type, mask, offset>(
                target, value);
        }

        template <reg_type value>
        static void write(reg_type& target) noexcept
        {
            access::template write<reg_type, mask, offset, value>(
                target);
        }

        static void set(reg_type& target) noexcept
        {
            access::template set<reg_type, mask>(
                target);
        }

        static void clear(reg_type& target) noexcept
        {
            access::template clear<reg_type, mask>(
                target);
        }

        static void toggle(reg_type& target) noexcept
        {
            access::template toggle<reg_type, mask>(
                target);
        }
    };
}