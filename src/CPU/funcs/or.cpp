#include <cstdint>

#include "../../Utils/logger.h"
#include "../CPU8068.h"

void CPU8068::or_rm_reg(const uint8_t mod_rm, const uint8_t width) {
  if (width != 8 && width != 16) {
    mylog("Unsupported width in or_rm_reg");
    return;
  }
  const uint8_t mode = ((mod_rm >> 6) & 0b011);
  const uint8_t reg = ((mod_rm >> 3) & 0b111);
  const uint8_t r_m = ((mod_rm >> 0) & 0b111);

  if (mode == 0b11) {
    if (width == 8) {
      const uint8_t lhs = *reg8[r_m];
      const uint8_t rhs = *reg8[reg];
      const uint16_t result =
          (static_cast<uint16_t>(lhs) | static_cast<uint16_t>(rhs));
      set_flags_logical(result, width);

      *reg8[r_m] = static_cast<uint8_t>(result);
    } else if (width == 16) {
      const uint16_t lhs = *reg16[r_m];
      const uint16_t rhs = *reg16[reg];
      const uint32_t result =
          (static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
      set_flags_logical(result, width);

      *reg16[r_m] = static_cast<uint16_t>(result);
    }
  } else if (mode == 0b00 || mode == 0b01 || mode == 0b10) {
    uint16_t address;
    uint16_t segment;
    if (!get_address_mode_rm(mode, r_m, segment, address)) {
      mylog("Unsupported r/m bit");
      return;
    }

    if (width == 8) {
      const uint8_t lhs = mem8(segment, address);
      const uint8_t rhs = *reg8[reg];
      const uint16_t result =
          (static_cast<uint16_t>(lhs) | static_cast<uint16_t>(rhs));
      set_flags_logical(result, width);

      mem8(segment, address) = static_cast<uint8_t>(result);
    } else if (width == 16) {
      const uint16_t lhs = mem16(segment, address);
      const uint16_t rhs = *reg16[reg];
      const uint32_t result =
          (static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
      set_flags_logical(result, width);

      mem16(segment, address) = static_cast<uint16_t>(result);
    }
  } else {
    mylog("Unsupported 0x08, 0x09");
  }
}

void CPU8068::or_reg_rm(const uint8_t mod_rm, const uint8_t width) {
  if (width != 8 && width != 16) {
    mylog("Unsupported width in or_reg_rm");
    return;
  }
  const uint8_t mode = ((mod_rm >> 6) & 0b011);
  const uint8_t reg = ((mod_rm >> 3) & 0b111);
  const uint8_t r_m = ((mod_rm >> 0) & 0b111);

  if (mode == 0b11) {
    if (width == 8) {
      const uint8_t lhs = *reg8[reg];
      const uint8_t rhs = *reg8[r_m];
      const uint16_t result =
          (static_cast<uint16_t>(lhs) | static_cast<uint16_t>(rhs));
      set_flags_logical(result, width);

      *reg8[reg] = static_cast<uint8_t>(result);
    } else if (width == 16) {
      const uint16_t lhs = *reg16[reg];
      const uint16_t rhs = *reg16[r_m];
      const uint32_t result =
          (static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
      set_flags_logical(result, width);

      *reg16[reg] = static_cast<uint16_t>(result);
    }
  } else if (mode == 0b00 || mode == 0b01 || mode == 0b10) {
    uint16_t address;
    uint16_t segment;
    if (!get_address_mode_rm(mode, r_m, segment, address)) {
      mylog("Unsupported r/m bit");
      return;
    }

    if (width == 8) {
      const uint8_t lhs = *reg8[reg];
      const uint8_t rhs = mem8(segment, address);
      const uint16_t result =
          (static_cast<uint16_t>(lhs) | static_cast<uint16_t>(rhs));
      set_flags_logical(result, width);

      *reg8[reg] = static_cast<uint8_t>(result);
    } else if (width == 16) {
      const uint16_t lhs = *reg16[reg];
      const uint16_t rhs = mem16(segment, address);
      const uint32_t result =
          (static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
      set_flags_logical(result, width);

      *reg16[reg] = static_cast<uint16_t>(result);
    }
  } else {
    mylog("Unsupported 0x0A, 0x0B");
  }
}
