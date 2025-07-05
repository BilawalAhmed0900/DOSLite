#include <cstdint>

#include "../../Utils/logger.h"
#include "../CPU8068.h"

void CPU8068::test_rm_reg(const uint8_t mod_rm, const uint8_t width) {
  if (width != 8 && width != 16) {
    mylog("Unsupported width in test_rm_reg");
    return;
  }
  const uint8_t mode = ((mod_rm >> 6) & 0b011);
  const uint8_t reg = ((mod_rm >> 3) & 0b111);
  const uint8_t r_m = ((mod_rm >> 0) & 0b111);

  if (mode == 0b11) {
    if (width == 16) {
      const uint16_t lhs = *reg16[r_m];
      const uint16_t rhs = *reg16[reg];
      const uint32_t result = lhs & rhs;
      set_flags_logical(result, width);
      SetCF(0);
      SetOF(0);
    } else {
      const uint8_t lhs = *reg8[r_m];
      const uint8_t rhs = *reg8[reg];
      const uint16_t result = lhs & rhs;
      set_flags_logical(result, width);
      SetCF(0);
      SetOF(0);
    }
  } else if (mode == 0b00 || mode == 0b01 || mode == 0b10) {
    uint16_t address;
    if (!get_address_mode_rm(mode, r_m, address)) {
      mylog("Unsupported r/m bit");
      return;
    }

    if (width == 8) {
      const uint8_t lhs = mem8(DS, address);
      const uint8_t rhs = *reg8[reg];
      const uint16_t result = lhs & rhs;
      set_flags_logical(result, width);
      SetCF(0);
      SetOF(0);
    } else if (width == 16) {
      const uint16_t lhs = mem16(DS, address);
      const uint16_t rhs = *reg16[reg];
      const uint32_t result = lhs & rhs;
      set_flags_logical(result, width);
      SetCF(0);
      SetOF(0);
    }
  } else {
    mylog("Unsupported 0x84, 0x85");
  }
}
