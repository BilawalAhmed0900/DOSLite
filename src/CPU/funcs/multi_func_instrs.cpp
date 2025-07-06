#include <array>
#include <cstdint>
#include <functional>

#include "../../Utils/logger.h"
#include "../CPU8068.h"

void CPU8068::instr_80_81_82(const uint8_t mod_rm, const uint8_t width) {
  if (width != 8 && width != 16) {
    mylog("Unsupported width in instr_80_81_82");
    return;
  }
  const uint8_t mode = ((mod_rm >> 6) & 0b011);
  const uint8_t reg = ((mod_rm >> 3) & 0b111);
  const uint8_t r_m = ((mod_rm >> 0) & 0b111);

  if (mode == 0b11) {
    switch (reg) {
      case 0b000: {
        if (width == 8) {
          const uint8_t lhs = *reg8[r_m];
          const uint8_t rhs = mem8(CS, IP++);
          const uint16_t result = lhs + rhs;
          set_flags_add(lhs, rhs, result, width);

          *reg8[r_m] = static_cast<uint8_t>(result);
        } else {
          const uint16_t lhs = *reg16[r_m];
          const uint16_t rhs = mem16(CS, IP);
          IP += 2;

          const uint32_t result = lhs + rhs;
          set_flags_add(lhs, rhs, result, width);
          *reg16[r_m] = static_cast<uint16_t>(result);
        }

        break;
      }
      case 0b001: {
        if (width == 8) {
          const uint8_t lhs = *reg8[r_m];
          const uint8_t rhs = mem8(CS, IP++);
          const uint8_t result = lhs | rhs;
          set_flags_logical(result, width);

          *reg8[r_m] = result;
        } else {
          const uint16_t lhs = *reg16[r_m];
          const uint16_t rhs = mem16(CS, IP);
          IP += 2;

          const uint16_t result = lhs | rhs;
          set_flags_logical(result, width);
          *reg16[r_m] = result;
        }

        break;
      }
      case 0b010: {
        const uint8_t cf = CF();
        if (width == 8) {
          const uint8_t lhs = *reg8[r_m];
          const uint8_t rhs = mem8(CS, IP++);
          const uint16_t result = lhs + rhs + cf;
          set_flags_add(lhs, rhs + cf, result, width);

          *reg8[r_m] = static_cast<uint8_t>(result);
        } else {
          const uint16_t lhs = *reg16[r_m];
          const uint16_t rhs = mem16(CS, IP);
          IP += 2;

          const uint32_t result = lhs + rhs + cf;
          set_flags_add(lhs, rhs + cf, result, width);
          *reg16[r_m] = static_cast<uint16_t>(result);
        }

        break;
      }
      case 0b011: {
        const uint8_t cf = CF();
        if (width == 8) {
          const uint8_t lhs = *reg8[r_m];
          const uint8_t rhs = mem8(CS, IP++);
          const uint16_t result = lhs - rhs - cf;
          set_flags_sub(lhs, rhs + cf, result, width);

          *reg8[r_m] = static_cast<uint8_t>(result);
        } else {
          const uint16_t lhs = *reg16[r_m];
          const uint16_t rhs = mem16(CS, IP);
          IP += 2;

          const uint32_t result = lhs - rhs - cf;
          set_flags_sub(lhs, rhs + cf, result, width);
          *reg16[r_m] = static_cast<uint16_t>(result);
        }

        break;
      }
      case 0b100: {
        if (width == 8) {
          const uint8_t lhs = *reg8[r_m];
          const uint8_t rhs = mem8(CS, IP++);
          const uint8_t result = lhs & rhs;
          set_flags_logical(result, width);

          *reg8[r_m] = result;
        } else {
          const uint16_t lhs = *reg16[r_m];
          const uint16_t rhs = mem16(CS, IP);
          IP += 2;

          const uint16_t result = lhs & rhs;
          set_flags_logical(result, width);
          *reg16[r_m] = result;
        }

        break;
      }
      case 0b101: {
        if (width == 8) {
          const uint8_t lhs = *reg8[r_m];
          const uint8_t rhs = mem8(CS, IP++);
          const uint16_t result = lhs - rhs;
          set_flags_sub(lhs, rhs, result, width);

          *reg8[r_m] = static_cast<uint8_t>(result);
        } else {
          const uint16_t lhs = *reg16[r_m];
          const uint16_t rhs = mem16(CS, IP);
          IP += 2;

          const uint32_t result = lhs - rhs;
          set_flags_sub(lhs, rhs, result, width);
          *reg16[r_m] = static_cast<uint16_t>(result);
        }

        break;
      }
      case 0b110: {
        if (width == 8) {
          const uint8_t lhs = *reg8[r_m];
          const uint8_t rhs = mem8(CS, IP++);
          const uint8_t result = lhs ^ rhs;
          set_flags_logical(result, width);

          *reg8[r_m] = result;
        } else {
          const uint16_t lhs = *reg16[r_m];
          const uint16_t rhs = mem16(CS, IP);
          IP += 2;

          const uint16_t result = lhs ^ rhs;
          set_flags_logical(result, width);
          *reg16[r_m] = result;
        }

        break;
      }
      case 0b111: {
        if (width == 8) {
          const uint8_t lhs = *reg8[r_m];
          const uint8_t rhs = mem8(CS, IP++);
          const uint16_t result = lhs - rhs;
          set_flags_sub(lhs, rhs, result, width);
        } else {
          const uint16_t lhs = *reg16[r_m];
          const uint16_t rhs = mem16(CS, IP);
          IP += 2;

          const uint32_t result = lhs - rhs;
          set_flags_sub(lhs, rhs, result, width);
        }

        break;
      }
      default:
        mylog("Unsupported 0x80, 0x81, 0x82");
    }
  } else if (mode == 0b00 || mode == 0b01 || mode == 0b10) {
    uint16_t address;
    if (!get_address_mode_rm(mode, r_m, address)) {
      mylog("Unsupported r/m bit");
      return;
    }

    switch (reg) {
      case 0b000: {
        if (width == 8) {
          const uint8_t lhs = mem8(DS, address);
          const uint8_t rhs = mem8(CS, IP++);
          const uint16_t result = lhs + rhs;
          set_flags_add(lhs, rhs, result, width);

          mem8(DS, address) = static_cast<uint8_t>(result);
        } else {
          const uint16_t lhs = mem16(DS, address);
          const uint16_t rhs = mem16(CS, IP);
          IP += 2;

          const uint32_t result = lhs + rhs;
          set_flags_add(lhs, rhs, result, width);
          mem16(DS, address) = static_cast<uint16_t>(result);
        }

        break;
      }
      case 0b001: {
        if (width == 8) {
          const uint8_t lhs = mem8(DS, address);
          const uint8_t rhs = mem8(CS, IP++);
          const uint8_t result = lhs | rhs;
          set_flags_logical(result, width);

          mem8(DS, address) = result;
        } else {
          const uint16_t lhs = mem16(DS, address);
          const uint16_t rhs = mem16(CS, IP);
          IP += 2;

          const uint16_t result = lhs | rhs;
          set_flags_logical(result, width);
          mem16(DS, address) = result;
        }

        break;
      }
      case 0b010: {
        const uint8_t cf = CF();
        if (width == 8) {
          const uint8_t lhs = mem8(DS, address);
          const uint8_t rhs = mem8(CS, IP++);
          const uint16_t result = lhs + rhs + cf;
          set_flags_add(lhs, rhs + cf, result, width);

          mem8(DS, address) = static_cast<uint8_t>(result);
        } else {
          const uint16_t lhs = mem16(DS, address);
          const uint16_t rhs = mem16(CS, IP);
          IP += 2;

          const uint32_t result = lhs + rhs + cf;
          set_flags_add(lhs, rhs + cf, result, width);
          mem16(DS, address) = static_cast<uint16_t>(result);
        }

        break;
      }
      case 0b011: {
        const uint8_t cf = CF();
        if (width == 8) {
          const uint8_t lhs = mem8(DS, address);
          const uint8_t rhs = mem8(CS, IP++);
          const uint16_t result = lhs - rhs - cf;
          set_flags_sub(lhs, rhs + cf, result, width);

          mem8(DS, address) = static_cast<uint8_t>(result);
        } else {
          const uint16_t lhs = mem16(DS, address);
          const uint16_t rhs = mem16(CS, IP);
          IP += 2;

          const uint32_t result = lhs - rhs - cf;
          set_flags_sub(lhs, rhs + cf, result, width);
          mem16(DS, address) = static_cast<uint16_t>(result);
        }

        break;
      }
      case 0b100: {
        if (width == 8) {
          const uint8_t lhs = mem8(DS, address);
          const uint8_t rhs = mem8(CS, IP++);
          const uint8_t result = lhs & rhs;
          set_flags_logical(result, width);

          mem8(DS, address) = result;
        } else {
          const uint16_t lhs = mem16(DS, address);
          const uint16_t rhs = mem16(CS, IP);
          IP += 2;

          const uint16_t result = lhs & rhs;
          set_flags_logical(result, width);
          mem16(DS, address) = result;
        }

        break;
      }
      case 0b101: {
        if (width == 8) {
          const uint8_t lhs = mem8(DS, address);
          const uint8_t rhs = mem8(CS, IP++);
          const uint16_t result = lhs - rhs;
          set_flags_sub(lhs, rhs, result, width);

          mem8(DS, address) = static_cast<uint8_t>(result);
        } else {
          const uint16_t lhs = mem16(DS, address);
          const uint16_t rhs = mem16(CS, IP);
          IP += 2;

          const uint32_t result = lhs - rhs;
          set_flags_sub(lhs, rhs, result, width);
          mem16(DS, address) = static_cast<uint16_t>(result);
        }

        break;
      }
      case 0b110: {
        if (width == 8) {
          const uint8_t lhs = mem8(DS, address);
          const uint8_t rhs = mem8(CS, IP++);
          const uint8_t result = lhs ^ rhs;
          set_flags_logical(result, width);

          mem8(DS, address) = result;
        } else {
          const uint16_t lhs = mem16(DS, address);
          const uint16_t rhs = mem16(CS, IP);
          IP += 2;

          const uint16_t result = lhs ^ rhs;
          set_flags_logical(result, width);
          mem16(DS, address) = result;
        }

        break;
      }
      case 0b111: {
        if (width == 8) {
          const uint8_t lhs = mem8(DS, address);
          const uint8_t rhs = mem8(CS, IP++);
          const uint16_t result = lhs - rhs;
          set_flags_sub(lhs, rhs, result, width);
        } else {
          const uint16_t lhs = mem16(DS, address);
          const uint16_t rhs = mem16(CS, IP);
          IP += 2;

          const uint32_t result = lhs - rhs;
          set_flags_sub(lhs, rhs, result, width);
        }

        break;
      }
      default:
        mylog("Unsupported 0x80, 0x81, 0x82");
    }
  }
}

void CPU8068::instr_83(const uint8_t mod_rm) {
  const uint8_t mode = ((mod_rm >> 6) & 0b011);
  const uint8_t reg = ((mod_rm >> 3) & 0b111);
  const uint8_t r_m = ((mod_rm >> 0) & 0b111);

  if (mode == 0b11) {
    switch (reg) {
      case 0b000: {
        const uint16_t lhs = *reg16[r_m];
        const uint16_t rhs = sign_extend(mem8(CS, IP++));

        const uint32_t result = lhs + rhs;
        set_flags_add(lhs, rhs, result, 16);
        *reg16[r_m] = static_cast<uint16_t>(result);

        break;
      }
      case 0b001: {
        const uint16_t lhs = *reg16[r_m];
        const uint16_t rhs = sign_extend(mem8(CS, IP++));

        const uint16_t result = lhs | rhs;
        set_flags_logical(result, 16);
        *reg16[r_m] = result;

        break;
      }
      case 0b010: {
        const uint8_t cf = CF();
        const uint16_t lhs = *reg16[r_m];
        const uint16_t rhs = sign_extend(mem8(CS, IP++));

        const uint32_t result = lhs + rhs + cf;
        set_flags_add(lhs, rhs + cf, result, 16);
        *reg16[r_m] = static_cast<uint16_t>(result);

        break;
      }
      case 0b011: {
        const uint8_t cf = CF();
        const uint16_t lhs = *reg16[r_m];
        const uint16_t rhs = sign_extend(mem8(CS, IP++));

        const uint32_t result = lhs - rhs - cf;
        set_flags_sub(lhs, rhs + cf, result, 16);
        *reg16[r_m] = static_cast<uint16_t>(result);

        break;
      }
      case 0b100: {
        const uint16_t lhs = *reg16[r_m];
        const uint16_t rhs = sign_extend(mem8(CS, IP++));

        const uint16_t result = lhs & rhs;
        set_flags_logical(result, 16);
        *reg16[r_m] = result;

        break;
      }
      case 0b101: {
        const uint16_t lhs = *reg16[r_m];
        const uint16_t rhs = sign_extend(mem8(CS, IP++));

        const uint32_t result = lhs - rhs;
        set_flags_sub(lhs, rhs, result, 16);
        *reg16[r_m] = static_cast<uint16_t>(result);

        break;
      }
      case 0b110: {
        const uint16_t lhs = *reg16[r_m];
        const uint16_t rhs = sign_extend(mem8(CS, IP++));

        const uint16_t result = lhs ^ rhs;
        set_flags_logical(result, 16);
        *reg16[r_m] = result;

        break;
      }
      case 0b111: {
        const uint16_t lhs = *reg16[r_m];
        const uint16_t rhs = sign_extend(mem8(CS, IP++));

        const uint32_t result = lhs - rhs;
        set_flags_sub(lhs, rhs, result, 16);

        break;
      }
      default:
        mylog("Unsupported 0x83");
    }
  } else if (mode == 0b00 || mode == 0b01 || mode == 0b10) {
    uint16_t address;
    if (!get_address_mode_rm(mode, r_m, address)) {
      mylog("Unsupported r/m bit");
      return;
    }

    switch (reg) {
      case 0b000: {
        const uint16_t lhs = mem16(DS, address);
        const uint16_t rhs = sign_extend(mem8(CS, IP++));

        const uint32_t result = lhs + rhs;
        set_flags_add(lhs, rhs, result, 16);
        mem16(DS, address) = static_cast<uint16_t>(result);

        break;
      }
      case 0b001: {
        const uint16_t lhs = mem16(DS, address);
        const uint16_t rhs = sign_extend(mem8(CS, IP++));

        const uint16_t result = lhs | rhs;
        set_flags_logical(result, 16);
        mem16(DS, address) = result;

        break;
      }
      case 0b010: {
        const uint8_t cf = CF();
        const uint16_t lhs = mem16(DS, address);
        const uint16_t rhs = sign_extend(mem8(CS, IP++));

        const uint32_t result = lhs + rhs + cf;
        set_flags_add(lhs, rhs + cf, result, 16);
        mem16(DS, address) = static_cast<uint16_t>(result);

        break;
      }
      case 0b011: {
        const uint8_t cf = CF();
        const uint16_t lhs = mem16(DS, address);
        const uint16_t rhs = sign_extend(mem8(CS, IP++));

        const uint32_t result = lhs - rhs - cf;
        set_flags_sub(lhs, rhs + cf, result, 16);
        mem16(DS, address) = static_cast<uint16_t>(result);

        break;
      }
      case 0b100: {
        const uint16_t lhs = mem16(DS, address);
        const uint16_t rhs = sign_extend(mem8(CS, IP++));

        const uint16_t result = lhs & rhs;
        set_flags_logical(result, 16);
        mem16(DS, address) = result;

        break;
      }
      case 0b101: {
        const uint16_t lhs = mem16(DS, address);
        const uint16_t rhs = sign_extend(mem8(CS, IP++));

        const uint32_t result = lhs - rhs;
        set_flags_sub(lhs, rhs, result, 16);
        mem16(DS, address) = static_cast<uint16_t>(result);

        break;
      }
      case 0b110: {
        const uint16_t lhs = mem16(DS, address);
        const uint16_t rhs = sign_extend(mem8(CS, IP++));

        const uint16_t result = lhs ^ rhs;
        set_flags_logical(result, 16);
        mem16(DS, address) = result;

        break;
      }
      case 0b111: {
        const uint16_t lhs = mem16(DS, address);
        const uint16_t rhs = sign_extend(mem8(CS, IP++));

        const uint32_t result = lhs - rhs;
        set_flags_sub(lhs, rhs, result, 16);

        break;
      }
      default:
        mylog("Unsupported 0x83");
    }
  }
}

void CPU8068::instr_d0_d1(uint8_t mod_rm, uint8_t width) {
  if (width != 8 && width != 16) {
    mylog("Unsupported width in instr_d0_d1");
    return;
  }

  const uint8_t mode = ((mod_rm >> 6) & 0b011);
  const uint8_t reg = ((mod_rm >> 3) & 0b111);
  const uint8_t r_m = ((mod_rm >> 0) & 0b111);

  switch (reg) {
    // ROL
    case 0b000: {
      /*
        The rotate left (ROL) and rotate through carry left (RCL) instructions
        shift all the bits toward more-significant bit positions, except for the
        most-significant bit, which is rotated to the least-significant bit
        location. The rotate right (ROR) and rotate through carry right (RCR)
        instructions shift all the bits toward less significant bit positions,
        except for the least-significant bit, which is rotated to the
        most-significant bit location.

        The OF flag is defined only for the 1-bit rotates; it is undefined in
        all other cases (except RCL and RCR instructions only: a zero-bit rotate
        does nothing, that is affects no flags). For left rotates, the OF flag
        is set to the exclusive OR of the CF bit (after the rotate) and the
        most-significant bit of the result. For right rotates, the OF flag is
        set to the exclusive OR of the two most-significant bits of the result.
      */
      if (width == 8) {
        uint16_t val = static_cast<uint16_t>(*reg8[r_m]);

        const uint8_t old_msb = (val & 0b1000'0000) ? 1 : 0;
        val <<= 1;
        const uint8_t new_msb = (val & 0b1000'0000) ? 1 : 0;
        val |= old_msb;

        SetCF(old_msb);
        SetOF(old_msb ^ new_msb);
        *reg8[r_m] = static_cast<uint8_t>(val);
      } else if (width == 16) {
        uint32_t val = static_cast<uint32_t>(*reg16[r_m]);

        const uint8_t old_msb = (val & 0b1000'0000'0000'0000) ? 1 : 0;
        val <<= 1;
        const uint8_t new_msb = (val & 0b1000'0000'0000'0000) ? 1 : 0;
        val |= old_msb;

        SetCF(old_msb);
        SetOF(old_msb ^ new_msb);
        *reg16[r_m] = static_cast<uint16_t>(val);
      }
      break;
    }
      // ROR
    case 0b001: {
      if (width == 8) {
        uint8_t val = *reg8[r_m];

        const uint8_t old_lsb = val & 0x1;
        val >>= 1;
        val |= ((old_lsb) ? 0b1000'0000 : 0);

        SetCF(old_lsb);
        SetOF(((val >> 7) & 0x1) ^ ((val >> 6) & 0x1));
        *reg8[r_m] = val;
      } else if (width == 16) {
        uint16_t val = *reg16[r_m];

        const uint8_t old_lsb = val & 0x1;
        val >>= 1;
        val |= ((old_lsb) ? 0b1000'0000'0000'0000 : 0);

        SetCF(old_lsb);
        SetOF(((val >> 15) & 0x1) ^ ((val >> 14) & 0x1));
        *reg16[r_m] = val;
      }
      break;
    }
      // RCL
    case 0b010: {
      if (width == 8) {
        uint8_t val = *reg8[r_m];
        const uint8_t cf = CF();
        const uint8_t old_msb = (val & 0b1000'0000) ? 1 : 0;
        val = (val << 1) | (cf ? 1 : 0);
        const uint8_t new_msb = (val & 0b1000'0000) ? 1 : 0;

        SetCF(old_msb);
        SetOF(old_msb ^ new_msb);
        *reg8[r_m] = val;
      } else if (width == 16) {
        uint16_t val = *reg16[r_m];
        const uint8_t cf = CF();
        const uint8_t old_msb = (val & 0b1000'0000'0000'0000) ? 1 : 0;
        val = (val << 1) | (cf ? 1 : 0);
        const uint8_t new_msb = (val & 0b1000'0000'0000'0000) ? 1 : 0;

        SetCF(old_msb);
        SetOF(old_msb ^ new_msb);
        *reg16[r_m] = val;
      }
      break;
    }
      // RCR
    case 0b011: {
      if (width == 8) {
        uint8_t val = *reg8[r_m];

        const uint8_t old_lsb = val & 0x1;
        val >>= 1;
        val |= (CF() ? 0b1000'0000 : 0);
        SetCF(old_lsb);

        SetOF(((val >> 7) & 0x1) ^ ((val >> 6) & 0x1));
        *reg8[r_m] = val;
      } else if (width == 16) {
        uint16_t val = *reg16[r_m];

        const uint8_t old_lsb = val & 0x1;
        val >>= 1;
        val |= (CF() ? 0b1000'0000'0000'0000 : 0);
        SetCF(old_lsb);

        SetOF(((val >> 15) & 0x1) ^ ((val >> 14) & 0x1));
        *reg16[r_m] = val;
      }
      break;
    }
      // SHL
    case 0b100: {
      if (width == 8) {
        uint16_t val = static_cast<uint16_t>(*reg8[r_m]);

        const uint8_t old_msb = (val & 0b1000'0000) ? 1 : 0;
        val <<= 1;
        const uint8_t new_msb = (val & 0b1000'0000) ? 1 : 0;

        // since this also set CF based upon latest result
        // but SHL has its own requirement
        set_flags_logical(val, width);
        SetCF(old_msb);
        SetOF(old_msb ^ new_msb);
        *reg8[r_m] = static_cast<uint8_t>(val);
      } else if (width == 16) {
        uint32_t val = static_cast<uint32_t>(*reg16[r_m]);

        const uint8_t old_msb = (val & 0b1000'0000'0000'0000) ? 1 : 0;
        val <<= 1;
        const uint8_t new_msb = (val & 0b1000'0000'0000'0000) ? 1 : 0;

        // since this also set CF based upon latest result
        // but SHL has its own requirement
        set_flags_logical(val, width);
        SetCF(old_msb);
        SetOF(old_msb ^ new_msb);
        *reg16[r_m] = static_cast<uint16_t>(val);
      }
      break;
    }
      // SHR
    case 0b101: {
      if (width == 8) {
        uint8_t val = *reg8[r_m];

        const uint8_t old_lsb = val & 0x1;
        val >>= 1;

        // since this also set CF based upon latest result
        // but SHR has its own requirement
        set_flags_logical(val, width);
        SetCF(old_lsb);
        SetOF(((val >> 7) & 0x1) ^ ((val >> 6) & 0x1));
        *reg8[r_m] = val;
      } else if (width == 16) {
        uint16_t val = *reg16[r_m];

        const uint8_t old_lsb = val & 0x1;
        val >>= 1;

        // since this also set CF based upon latest result
        // but SHR has its own requirement
        set_flags_logical(val, width);
        SetCF(old_lsb);
        SetOF(((val >> 15) & 0x1) ^ ((val >> 14) & 0x1));
        *reg16[r_m] = val;
      }
      break;
    }
    case 0b110:
    case 0b111: {
      mylog("Invalid value of reg in r/m byte in 0xD0 0xD1");
      break;
    }
    default:
      mylog("Unsupported 0xD0 0xD1");
  }
}
