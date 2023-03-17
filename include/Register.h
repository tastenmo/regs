#pragma once

#include "RegisterBase.h"

namespace regs {

/**
 * @brief Tag for no initailzing construction
 *
 */
struct noInit {};

template <typename Reg, typename Reg_type>
class Register : public RegisterBase<Reg, Reg_type> {
 public:

 using reg = Reg;
 using reg_type = Reg_type;

 static constexpr size_t size = sizeof(reg_type); 

 using target_type = byte_array<size>;
 using target_span = std::span<std::byte, size>;

 private:
  target_type _target;

  public:
  /**
   * @brief Default constructor with initialization
   *
   */
  explicit Register() : _target() {}

  /**
   * @brief Tagged constructor without initialization, to be used with placement
   * new
   *
   */
  explicit Register(noInit) {}

  auto span(){
    return std::span{_target};
  }





};




} // namespace regs