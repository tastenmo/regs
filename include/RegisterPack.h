#pragma once

#include "Register.h"
#include "RegisterBase.h"

namespace regs {

template <typename RegPack, typename Reg, unsigned Offset, typename Reg_type>
class PackedRegister : public RegisterBase<Reg, Reg_type> {
 public:

 using reg = Reg;
 using reg_pack = RegPack;
 using reg_type = Reg_type;

 static constexpr size_t size = sizeof(reg_type); 

 using target_type = byte_array<size>;


 private:

 const reg_pack& _pack;



  public:

    /**
   * @brief Default constructor with initialization
   *
   */
  constexpr PackedRegister(const reg_pack &pack) : _pack(pack) {}
  

  auto span(){
    return std::span{_pack._target.begin() + Offset, sizeof(reg_type)};
  }

  target_type const_target() const {

    target_type res;

    std::copy(_pack._target.begin() + Offset, _pack._target.begin() + Offset + size, res.begin());

    return res; 
     
     }
};

template <size_t size> class RegisterPack {
public:
  using target_type = byte_array<size>;
  using target_span = std::span<std::byte, size>;

//private:
  target_type _target;

public:
  /**
   * @brief Default constructor with initialization
   *
   */
  explicit RegisterPack() : _target() {}

  /**
   * @brief Tagged constructor without initialization, to be used with placement
   * new
   *
   */
  explicit RegisterPack(noInit) {}

  auto span() { return std::span{_target}; }

  



};
} // namespace regs