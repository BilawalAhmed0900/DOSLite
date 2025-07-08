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

/*
  All of this copied from Intel specs to clarify how this instructions work
*/
/*
  The ROL instruction rotates the bits in the operand to the left (toward more
  significant bit locations). The ROR instruction rotates the operand right
  (toward less significant bit locations).

  The RCL instruction rotates the bits in the operand to the left, through the
  CF flag. This instruction treats the CF flag as a one-bit extension on the
  upper end of the operand. Each bit that exits from the most significant bit
  location of the operand moves into the CF flag. At the same time, the bit in
  the CF flag enters the least significant bit location of the operand.

  The RCR instruction rotates the bits in the operand to the right through the
  CF flag.

  For all the rotate instructions, the CF flag always contains the value of the
  last bit rotated out of the operand, even if the instruction does not use the
  CF flag as an extension of the operand. The value of this flag can then be
  tested by a conditional jump instruction (JC or JNC).

  For the ROL and ROR instructions, the original value of the CF flag is not a
  part of the result, but the CF flag receives a copy of the bit that was
  shifted from one end to the other.

  The OF flag is defined only for the 1-bit rotates; it is undefined in all
  other cases (except RCL and RCR instructions only: a zero-bit rotate does
  nothing, that is affects no flags). For left rotates, the OF flag is set to
  the exclusive OR of the CF bit (after the rotate) and the most-significant bit
  of the result. For right rotates, the OF flag is set to the exclusive OR of
  the two most-significant bits of the result.

  The OF flag is affected only on 1-bit shifts. For left shifts, the OF flag is
  set to 0 if the most-significant bit of the result is the same as the CF flag
  (that is, the top two bits of the original operand were the same); otherwise,
  it is set to 1. For the SAR instruction, the OF flag is cleared for all 1-bit
  shifts. For the SHR instruction, the OF flag is set to the most-significant
  bit of the original operand.
*/
void CPU8068::instr_d0_d1_d2_d3_c0_c1(uint8_t mod_rm, uint8_t width, uint8_t count) {
  if (width != 8 && width != 16) {
    mylog("Unsupported width in instr_d2_d3_c0_c1");
    return;
  }

  /*
    The 8086 does not mask the rotation count. However, all other IA-32
    processors (starting with the Intel 286 processor) do mask the rotation
    count to 5 bits, resulting in a maximum count of 31. This masking is done in
    all operating modes (intimesuding the virtual-8086 mode) to reduce the maximum
    execution time of the instructions.
  */
  count &= 0b11111;

  if (count == 0) {
    mylog("times == 0 in instr_d2_d3_c0_c1");
    return;
  }

  const uint8_t mode = ((mod_rm >> 6) & 0b011);
  const uint8_t reg = ((mod_rm >> 3) & 0b111);
  const uint8_t r_m = ((mod_rm >> 0) & 0b111);

  switch (reg) {
    // ROL
    case 0b000: {
      if (width == 8) {
        uint8_t last_bit_rotated = 0;
        uint32_t val = ROL(*reg8[r_m], width, count, last_bit_rotated);
        SetCF(last_bit_rotated);
        if (count == 1) {
          const uint8_t new_msb =
              (val >> static_cast<uint32_t>(width - 1)) & 0x1;
          SetOF(last_bit_rotated ^ new_msb);
        }
        *reg8[r_m] = static_cast<uint8_t>(val);
      } else if (width == 16) {
        uint8_t last_bit_rotated = 0;
        uint32_t val = ROL(*reg16[r_m], width, count, last_bit_rotated);
        SetCF(last_bit_rotated);
        if (count == 1) {
          const uint8_t new_msb =
              (val >> static_cast<uint32_t>(width - 1)) & 0x1;
          SetOF(last_bit_rotated ^ new_msb);
        }
        *reg16[r_m] = static_cast<uint16_t>(val);
      }
      break;
    }
      // ROR
    case 0b001: {
      if (width == 8) {
        uint8_t last_bit_rotated = 0;
        uint32_t val = ROR(*reg8[r_m], width, count, last_bit_rotated);
        SetCF(last_bit_rotated);
        if (count == 1) {
          SetOF(((val >> (width - 1)) & 0x1) ^ ((val >> (width - 2)) & 0x1));
        }
        *reg8[r_m] = static_cast<uint8_t>(val);
      } else if (width == 16) {
        uint8_t last_bit_rotated = 0;
        uint32_t val = ROR(*reg16[r_m], width, count, last_bit_rotated);
        SetCF(last_bit_rotated);
        if (count == 1) {
          SetOF(((val >> (width - 1)) & 0x1) ^ ((val >> (width - 2)) & 0x1));
        }
        *reg16[r_m] = static_cast<uint16_t>(val);
      }
      break;
    }
      // RCL
    case 0b010: {
      if (width == 8) {
        uint8_t last_bit_rotated = 0;
        uint32_t val = RCL(*reg8[r_m], width, count, last_bit_rotated);
        SetCF(last_bit_rotated);
        SetOF(last_bit_rotated ^ ((val >> (width - 1)) & 0x1));
        *reg8[r_m] = static_cast<uint8_t>(val);
      } else if (width == 16) {
        uint8_t last_bit_rotated = 0;
        uint32_t val = RCL(*reg16[r_m], width, count, last_bit_rotated);
        SetCF(last_bit_rotated);
        SetOF(last_bit_rotated ^ ((val >> (width - 1)) & 0x1));
        *reg16[r_m] = static_cast<uint16_t>(val);
      }
      break;
    }
      // RCR
    case 0b011: {
      if (width == 8) {
        uint8_t last_bit_rotated = 0;
        uint32_t val = RCR(*reg8[r_m], width, count, last_bit_rotated);
        /*
          Since CF is part of this operation,
          the last_bit_rotated is actually
          already in the CF, just setting again
          for timesarity
        */
        SetCF(last_bit_rotated);

        /*
          It is same as the ROR above
          but the most 2 significant bit
          intimesudes the CF which is last_bit_rotated
          here
        */
        SetOF(last_bit_rotated ^ ((val >> (width - 1)) & 0x1));
        *reg8[r_m] = static_cast<uint8_t>(val);
      } else if (width == 16) {
        uint8_t last_bit_rotated = 0;
        uint32_t val = RCR(*reg16[r_m], width, count, last_bit_rotated);
        /*
          Since CF is part of this operation,
          the last_bit_rotated is actually
          already in the CF, just setting again
          for timesarity
        */
        SetCF(last_bit_rotated);

        /*
          It is same as the ROR above
          but the most 2 significant bit
          intimesudes the CF which is last_bit_rotated
          here
        */
        SetOF(last_bit_rotated ^ ((val >> (width - 1)) & 0x1));
        *reg16[r_m] = static_cast<uint16_t>(val);
      }
      break;
    }
      // SHL
    case 0b100: {
      if (width == 8) {
        uint8_t last_bit_rotated = 0;
        uint32_t val = SHL(*reg8[r_m], width, count, last_bit_rotated);

        set_flags_logical(val, width);
        SetCF(last_bit_rotated);

        if (count == 1) {
          const uint8_t new_msb =
              (val >> static_cast<uint32_t>(width - 1)) & 0x1;
          SetOF(last_bit_rotated ^ new_msb);
        } else {
          SetOF(0); // undefined actually, so currently clearing
        }
        
        *reg8[r_m] = static_cast<uint8_t>(val);
      } else if (width == 16) {
        uint8_t last_bit_rotated = 0;
        uint32_t val = SHL(*reg16[r_m], width, count, last_bit_rotated);

        set_flags_logical(val, width);
        SetCF(last_bit_rotated);
        if (count == 1) {
          const uint8_t new_msb =
              (val >> static_cast<uint32_t>(width - 1)) & 0x1;
          SetOF(last_bit_rotated ^ new_msb);
        } else {
          SetOF(0);  // undefined actually, so currently clearing
        }
        *reg16[r_m] = static_cast<uint16_t>(val);
      }
      break;
    }
      // SHR
    case 0b101: {
      if (width == 8) {
        uint8_t last_bit_rotated = 0;
        const uint8_t old_msb =
            (*reg8[r_m] >> static_cast<uint32_t>(width - 1)) & 0x1;
        uint32_t val = SHR(*reg8[r_m], width, count, last_bit_rotated);

        set_flags_logical(val, width);
        SetCF(last_bit_rotated);

        if (count == 1) {
          SetOF(old_msb);
        }
        else {
          SetOF(0); // undefined actually, so currently clearing
        }
        
        *reg8[r_m] = static_cast<uint8_t>(val);
      } else if (width == 16) {
        uint8_t last_bit_rotated = 0;
        const uint8_t old_msb =
            (*reg16[r_m] >> static_cast<uint32_t>(width - 1)) & 0x1;
        uint32_t val = SHR(*reg16[r_m], width, count, last_bit_rotated);

        set_flags_logical(val, width);
        SetCF(last_bit_rotated);
        if (count == 1) {
          SetOF(old_msb);
        } else {
          SetOF(0); // undefined actually, so currently clearing
        }
        *reg16[r_m] = static_cast<uint16_t>(val);
      }
      break;
    }
    case 0b110: {
      if (width == 8) {
        uint8_t last_bit_rotated = 0;
        uint32_t val = SAR(*reg8[r_m], width, count, last_bit_rotated);

        set_flags_logical(val, width);
        SetCF(last_bit_rotated);
        if (count == 1) {
          SetOF(0);
        } else {
          SetOF(0); // undefined actually, so currently clearing
        }
        
        *reg8[r_m] = static_cast<uint8_t>(val);
      } else if (width == 16) {
        uint8_t last_bit_rotated = 0;
        uint32_t val = SAR(*reg16[r_m], width, count, last_bit_rotated);

        set_flags_logical(val, width);
        SetCF(last_bit_rotated);
        if (count == 1) {
          SetOF(0);
        } else {
          SetOF(0); // undefined actually, so currently clearing
        }
        *reg16[r_m] = static_cast<uint16_t>(val);
      }
      break;
    }
    case 0b111: {
      mylog("Invalid value of reg in r/m byte in 0xD0 0xD1 0xC0 0xC1");
      break;
    }
    default:
      mylog("Unsupported 0xD0 0xD1 0xC0 0xC1");
  }
}
