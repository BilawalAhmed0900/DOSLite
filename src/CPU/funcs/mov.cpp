#include <cstdint>

#include "../../Utils/logger.h"
#include "../CPU8068.h"

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
    uint16_t segment;
    if (!get_address_mode_rm(mode, r_m, segment, address)) {
      mylog("Unsupported r/m bit");
      return;
    }

    if (width == 8) {
      mem8(segment, address) = *reg8[reg];
    } else if (width == 16) {
      mem16(segment, address) = *reg16[reg];
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
    uint16_t segment;
    if (!get_address_mode_rm(mode, r_m, segment, address)) {
      mylog("Unsupported r/m bit");
      return;
    }

    if (width == 8) {
      mem8(segment, address) = mem8(CS, IP++);
    } else if (width == 16) {
      mem16(segment, address) = mem16(CS, IP);
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
    uint16_t segment;
    if (!get_address_mode_rm(mode, r_m, segment, address)) {
      mylog("Unsupported r/m bit");
      return;
    }

    if (width == 8) {
      *reg8[reg] = mem8(segment, address);
    } else if (width == 16) {
      *reg16[reg] = mem16(segment, address);
    }
  } else {
    mylog("Unsupported 0x8A, 0x8B");
  }
}

void CPU8068::mov_rm_sreg(uint8_t mod_rm, uint8_t width) {
  if (width != 16) {
    mylog("Unsupported width in mov_rm_sreg");
    return;
  }

  const uint8_t mode = ((mod_rm >> 6) & 0b011);
  const uint8_t reg = ((mod_rm >> 3) & 0b111);
  const uint8_t r_m = ((mod_rm >> 0) & 0b111);

  /*
    only this combination with CS is allowed
    mov ax, cs
  */
  if (reg == 0b001) {
    if (mode != 0b11) {
      mylog("Unsupported reg in mov_rm_sreg");
      return;
    }

    if (r_m != 0b000) {
      mylog("Unsupported r_m in mov_rm_sreg");
      return;
    }

    *reg16[r_m] = CS;
    return;
  }

  if (reg != 0b000 && reg != 0b010 && reg != 0b011) {
    mylog("Unsupported reg in mov_rm_sreg");
    return;
  }

  const uint16_t segment = (reg == 0b000)   ? ES
                           : (reg == 0b010) ? SS
                           : (reg == 0b011) ? DS
                                            : static_cast<uint16_t>(-1);
  if (segment == static_cast<uint16_t>(-1)) {
    mylog("Unsupported reg in mov_rm_sreg");
    return;
  }

  if (mode == 0b11) {
    *reg16[r_m] = segment;
  } else if (mode == 0b00 || mode == 0b01 || mode == 0b10) {
    uint16_t address;
    uint16_t segment;
    if (!get_address_mode_rm(mode, r_m, segment, address)) {
      mylog("Unsupported r/m bit");
      return;
    }

    mem16(segment, address) = segment;
  } else {
    mylog("Unsupported 0x8C");
  }
}

void CPU8068::mov_sreg_rm(uint8_t mod_rm, uint8_t width) {
  if (width != 16) {
    mylog("Unsupported width in mov_sreg_rm");
    return;
  }

  const uint8_t mode = ((mod_rm >> 6) & 0b011);
  const uint8_t reg = ((mod_rm >> 3) & 0b111);
  const uint8_t r_m = ((mod_rm >> 0) & 0b111);

  if (reg != 0b000 && reg != 0b010 && reg != 0b011) {
    mylog("Unsupported reg in mov_sreg_rm");
    return;
  }

  uint16_t *resultant_segment_register = (reg == 0b000)   ? &ES
                      : (reg == 0b010) ? &SS
                      : (reg == 0b011) ? &DS
                                       : nullptr;
  if (resultant_segment_register == nullptr) {
    mylog("Unsupported reg in mov_sreg_rm");
    return;
  }

  if (mode == 0b11) {
    *resultant_segment_register = *reg16[r_m];
  } else if (mode == 0b00 || mode == 0b01 || mode == 0b10) {
    uint16_t address;
    uint16_t segment;
    if (!get_address_mode_rm(mode, r_m, segment, address)) {
      mylog("Unsupported r/m bit");
      return;
    }

    *resultant_segment_register = mem16(segment, address);
  } else {
    mylog("Unsupported 0x8E");
  }
}
