/**
 * @file Access.h
 * @author Martin Heubuch (martin.heubuch@escad.de)
 * @brief
 * @version 0.1
 * @date 2022-11-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <cstdint>
#include <limits>

namespace regs
{

    namespace detail
    {
        /**
         * @brief
         *
         * @tparam T
         */
        template <typename reg_type>
        struct type_mask
        {
            constexpr static const reg_type value = std::numeric_limits<reg_type>::max();
        };

        /**
         * @brief Boolean flag to detect register read/write.
         * @tparam reg_type register data type
         * @tparam mask mask for the operation
         * @tparam offset
         */
        template <typename reg_type, reg_type mask, unsigned offset>
        struct is_register_rw
            : std::integral_constant<bool,
                                     (mask == std::numeric_limits<reg_type>::max()) && (offset == unsigned{0})>
        {
        };

        template <typename reg_type, reg_type mask, unsigned offset, typename U>
        using is_register =
            typename std::enable_if<is_register_rw<reg_type, mask, offset>::value, U>::type;

        template <typename reg_type, reg_type mask, unsigned offset, typename U>
        using is_region =
            typename std::enable_if<!is_register_rw<reg_type, mask, offset>::value, U>::type;

        //* @todo add is_single_bit

        /**
         * @brief Read implementation
         *
         * @tparam reg_type
         * @tparam mask
         * @tparam offset
         */
        template <typename reg_type, reg_type mask, unsigned offset>
        struct Read
        {

            /**
             * @brief region read implementation
             *
             */

            template <typename U = void>
            static reg_type read(const reg_type &target,
                                 is_region<reg_type, mask, offset, U> * = nullptr) noexcept
            {
                const auto lhs = target & mask;
                return (lhs >> offset);
            }

            /**
             * @brief register read implementation
             *
             */

            template <typename U = void>
            static reg_type read(const reg_type &target,
                                 is_register<reg_type, mask, offset, U> * = nullptr) noexcept
            {
                return target;
            }
        };

        /**
         * @brief Write implementation
         *
         * @tparam reg_type
         * @tparam mask
         * @tparam offset
         */
        template <typename reg_type, reg_type mask, unsigned offset>
        struct Write
        {

            /**
             * @brief region write implementation
             *
             */

            template <typename U = void>
            static void write(reg_type &target, reg_type value,
                              is_region<reg_type, mask, offset, U> * = nullptr) noexcept
            {
                const auto shifted_value = value << offset;
                const auto lhs = (target & ~mask);
                const auto rhs = (shifted_value & mask);
                target = (lhs | rhs);
            }

            /**
             * @brief register write implementation
             *
             */

            template <typename U = void>
            static void write(reg_type &target, reg_type value,
                              is_register<reg_type, mask, offset, U> * = nullptr) noexcept
            {
                target = value;
            }
        };

        /**
         * @brief Write implementation fo constant values
         *
         * @tparam reg_type
         * @tparam mask
         * @tparam offset
         */
        template <typename reg_type, reg_type mask, unsigned offset, reg_type value>
        struct WriteConstant
        {

            /**
             * @brief region write implementation
             *
             */

            template <typename U = void>
            static void write(reg_type &target,
                              is_region<reg_type, mask, offset, U> * = nullptr) noexcept
            {
                const auto shifted_value = value << offset;
                const auto lhs = (target & ~mask);
                const auto rhs = (shifted_value & mask);
                target = (lhs | rhs);
            }

            /**
             * @brief register write implementation
             *
             */

            template <typename U = void>
            static void write(reg_type &target,
                              is_register<reg_type, mask, offset, U> * = nullptr) noexcept
            {
                target = value;
            }
        };

        /**
         * @brief Is implementation
         *
         * @tparam reg_type
         * @tparam mask
         * @tparam offset
         */
        template <typename reg_type, reg_type mask, unsigned offset, reg_type value>
        struct Is
        {

            /**
             * @brief region read implementation
             *
             */

            template <typename U = void>
            static bool is(const reg_type &target,
                           is_region<reg_type, mask, offset, U> * = nullptr) noexcept
            {
                const auto lhs = target & mask;
                return (lhs >> offset) == value;
            }

            /**
             * @brief register read implementation
             *
             */

            template <typename U = void>
            static bool is(const reg_type &target,
                           is_register<reg_type, mask, offset, U> * = nullptr) noexcept
            {
                return target == value;
            }
        };

    }

    struct read_only
    {

        template <typename reg_type, reg_type mask, unsigned offset>
        static reg_type read(const reg_type &target) noexcept
        {
            return detail::Read<reg_type, mask, offset>::read(target);
        }

        template <typename reg_type, reg_type mask, unsigned offset, reg_type value>
        [[nodiscard]] static bool is(const reg_type &target) noexcept
        {
            return detail::Is<reg_type, mask, offset, value>::is(target);
        }
    };

    struct read_write : read_only
    {

        template <typename reg_type, reg_type mask, unsigned offset>
        static void write(reg_type &target, const reg_type value) noexcept
        {
            detail::Write<reg_type, mask, offset>::write(target, value);
        }

        template <typename reg_type, reg_type mask, unsigned offset, reg_type value>
        static void write(reg_type &target) noexcept
        {
            detail::WriteConstant<reg_type, mask, offset, value>::write(target);
        }

        template <typename reg_type, reg_type mask>
        static void set(reg_type &target) noexcept
        {
            detail::WriteConstant<reg_type, mask, unsigned{0}, mask>::write(target);
        }

        template <typename reg_type, reg_type mask>
        static void clear(reg_type &target) noexcept
        {
            detail::WriteConstant<reg_type, mask, unsigned{0}, (reg_type)~mask>::write(target);
        }

        template <typename reg_type, reg_type mask>
        static void toggle(reg_type &target) noexcept
        {
            target = target ^ mask;
        }
    };

    struct write_only
    {

        template <typename reg_type, reg_type mask, unsigned offset>
        static void write(const reg_type &target, const reg_type value) noexcept
        {
            detail::Write<reg_type, type_mask<T>::value, unsigned{0}>::write(target, ((value << offset) & mask));
        }

        template <typename reg_type, reg_type mask, unsigned offset, reg_type value>
        static bool write(const reg_type &target) noexcept
        {
            detail::WriteConstant<reg_type, type_mask<T>::value, offset, ((value << offset) & mask)>::write(target);
        }
    };

}