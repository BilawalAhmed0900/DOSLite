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
  } else {
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
  } else {
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
