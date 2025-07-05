#include <cstdint>

#include "../../Utils/logger.h"
#include "../CPU8068.h"

void CPU8068::xchg_reg_rm(uint8_t mod_rm, uint8_t width) {
  if (width != 8 && width != 16) {
    mylog("Unsupported width in cmp_reg_rm");
    return;
  }
  const uint8_t mode = ((mod_rm >> 6) & 0b011);
  const uint8_t reg = ((mod_rm >> 3) & 0b111);
  const uint8_t r_m = ((mod_rm >> 0) & 0b111);

  if (mode == 0b11) {
    if (width == 16) {
      const uint16_t temp = *reg16[reg];
      uint16_t& lhs = *reg16[reg];
      uint16_t& rhs = *reg16[r_m];
      lhs = rhs;
      rhs = temp;
    } else {
      const uint8_t temp = *reg8[reg];
      uint8_t& lhs = *reg8[reg];
      uint8_t& rhs = *reg8[r_m];
      lhs = rhs;
      rhs = temp;
    }
  } else if (mode == 0b00 || mode == 0b01 || mode == 0b10) {
    uint16_t address;
    if (!get_address_mode_rm(mode, r_m, address)) {
      mylog("Unsupported r/m bit");
      return;
    }

    if (width == 8) {
      const uint8_t temp = *reg8[reg];
      uint8_t& lhs = *reg8[reg];
      uint8_t& rhs = mem8(DS, address);
      lhs = rhs;
      rhs = temp;
    } else if (width == 16) {
      const uint16_t temp = *reg16[reg];
      uint16_t& lhs = *reg16[reg];
      uint16_t& rhs = mem16(DS, address);
      lhs = rhs;
      rhs = temp;
    }
  } else {
    mylog("Unsupported 0x86, 0x87");
  }
}
