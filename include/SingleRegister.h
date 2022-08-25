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

namespace regs
{

  /**
   * @brief Register with bitfields
   *
   * @tparam reg_type underlying type
   */

  template <typename reg_type>
  class SingleRegister
  {
  public:
    /**
     * @brief underlying access type
     *
     */
    using access_t = reg_type;

  protected:
    access_t value;

  public:
    access_t read() { return value; }
    void write(access_t &v) { value = v; }

    /**
     * @brief A bitregion within a register
     *
     * @tparam offset offset to the first bit
     * @tparam width bit width field
     */
    template <unsigned offset, unsigned width>
    class Bitfield
    {
      static_assert(width >= 1, "invalid width");
      static_assert(offset + width <= sizeof(access_t) * 8,
                    "invalid width/offset");

    public:
      /**
       * @brief unshifted mask of this field
       *
       */
      static constexpr access_t mask = ((access_t)1 << width) - 1;

      /**
       * @brief shifted mask of the bit field
       *
       */
      static constexpr access_t reg_mask = (access_t)(mask << offset);

      /**
       * @brief negation of 'regmask to clear bits
       *
       */
      static constexpr access_t clear_mask = (access_t)~reg_mask;

      using Compound_reg = SingleRegister<reg_type>;

      /**
       * Get register with this bitfield set to 'value' and rest left 0
       *
       * Useful to combine successive access to multiple
       * bitfields into one operation.
       */

      /**
       * @brief Get the shifted and masked value of this bitfield
       *
       * @param value
       * @return access_t
       */
      static inline access_t bits(access_t const value)
      {
        return (value & mask) << offset;
      }

      /**
       * @brief Get a register value 'reg' masked according to this bitfield
       *
       * @note '0x1234' masked according to a
       * 'Register<uint32_t>::Bitfield<5,7>' returns '0x0220'.
       */
      static inline access_t masked(access_t const reg) { return reg & reg_mask; }

      /**
       * @brief brief description value of this bitfield from 'reg'
       */
      static inline access_t get(access_t const reg)
      {
        return (reg >> offset) & mask;
      }

      /**
       * @brief Get register value 'reg' with this bitfield set to zero
       */
      static inline void clear(access_t &reg) { reg &= clear_mask; }

      /**
       * @brief Get register value 'reg' with this bitfield set to 'value'
       */
      static inline void set(access_t &reg, access_t const value = ~0)
      {
        clear(reg);
        reg |= (value & mask) << offset;
      };
    };

    template <unsigned offset_>
    class Bit
    {
      static_assert(offset_ + 1 <= sizeof(access_t) * 8,
                    "invalid offset");

    public:

      static constexpr unsigned offset = offset_;
      
      static constexpr access_t mask = ((access_t)1 << 1) - 1;

      static constexpr access_t reg_mask = (access_t)(1 << offset);

      static constexpr access_t clear_mask = (access_t)~reg_mask;

    };

    template <typename TBit>
    inline access_t get() const{
      return (value >> TBit::offset) & TBit::mask;

    }

    template <typename TBit>
    inline void clear() { value &= TBit::clear_mask; }

    template <typename TBit>
    inline void set()
      {
        value &= TBit::clear_mask;
        value |= (1 & TBit::mask) << TBit::offset;
      }

  };

}