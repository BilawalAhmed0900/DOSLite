#include "../CPU8068.h"

#include <cstdint>

#include "../../Utils/logger.h"

void CPU8068::mov_rm_reg(const uint8_t mod_rm, const uint8_t width) {
  if (width != 8 && width != 16) {
    mylog("Unsupported width in mov_rm_reg");
    return;
  }
  const uint8_t mode = ((mod_rm >> 6) & 0b011);
  const uint8_t reg = ((mod_rm >> 3) & 0b111);
  const uint8_t r_m = ((mod_rm >> 0) & 0b111);

  if (mode == 0b11) {
    *reg8[r_m] = *reg8[reg];
  } else if (mode == 0b00 || mode == 0b01 || mode == 0b10) {
    uint16_t address;
    if (!get_address_mode_rm(mode, r_m, address)) {
      mylog("Unsupported r/m bit");
      return;
    }

    if (width == 8) {
      mem8(DS, address) = *reg8[reg];
    } else if (width == 16) {
      mem16(DS, address) = *reg16[reg];
    }
  } else {
    mylog("Unsupported 0x88, 0x89");
  }
}

void CPU8068::mov_rm_imm(const uint8_t mod_rm, const uint8_t width) {
  if (width != 8 && width != 16) {
    mylog("Unsupported width in mov_rm_reg");
    return;
  }
  const uint8_t mode = ((mod_rm >> 6) & 0b011);
  const uint8_t reg = ((mod_rm >> 3) & 0b111);
  const uint8_t r_m = ((mod_rm >> 0) & 0b111);

  if (reg != 0) {
    mylog("Unsupported reg in mov_rm_imm");
    return;
  }

  if (mode == 0b11) {
    *reg8[r_m] = mem8(CS, IP++);
  } else if (mode == 0b00 || mode == 0b01 || mode == 0b10) {
    uint16_t address;
    if (!get_address_mode_rm(mode, r_m, address)) {
      mylog("Unsupported r/m bit");
      return;
    }

    if (width == 8) {
      mem8(DS, address) = mem8(CS, IP++);
    } else if (width == 16) {
      mem16(DS, address) = mem16(CS, IP);
      IP += 2;
    }
  } else {
    mylog("Unsupported 0xC6, 0xC7");
  }
}

void CPU8068::mov_reg_rm(const uint8_t mod_rm, const uint8_t width) {
  if (width != 8 && width != 16) {
    mylog("Unsupported width in mov_reg_rm");
    return;
  }
  const uint8_t mode = ((mod_rm >> 6) & 0b011);
  const uint8_t reg = ((mod_rm >> 3) & 0b111);
  const uint8_t r_m = ((mod_rm >> 0) & 0b111);

  if (mode == 0b11) {
    *reg8[reg] = *reg8[r_m];
  } else if (mode == 0b00 || mode == 0b01 || mode == 0b10) {
    uint16_t address;
    if (!get_address_mode_rm(mode, r_m, address)) {
      mylog("Unsupported r/m bit");
      return;
    }

    if (width == 8) {
      *reg8[reg] = mem8(DS, address);
    } else if (width == 16) {
      *reg16[reg] = mem16(DS, address);
    }
  } else {
    mylog("Unsupported 0x8A, 0x8B");
  }
}
