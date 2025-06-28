#include "../CPU8068.h"

#include <cstdint>

#include "../../Utils/logger.h"

uint16_t CPU8068::sign_extend(const uint8_t val) {
  return static_cast<uint16_t>(static_cast<int16_t>(static_cast<int8_t>(val)));
}

/*
 *  Here mod 0b11 is not calculated, that is based upon instruction
 *  If the instruction is for reg8, then lower registers are used
 *  else whole 16-bit register is used
 *
 *  Detail: https://en.wikipedia.org/wiki/ModR/M
 *      16-bit mode
 *
 *  R/M                       MOD
 *             00           01[a]             10          11
 *  000     [BX+SI]     [BX+SI+disp8]   [BX+SI+disp16]  AL / AX
 *  001     [BX+DI]     [BX+DI+disp8]   [BX+DI+disp16]  CL / CX
 *  010     [BP+SI]     [BP+SI+disp8]   [BP+SI+disp16]  DL / DX
 *  011     [BP+DI]     [BP+DI+disp8]   [BP+DI+disp16]  BL / BX
 *  100     [SI]        [SI+disp8]      [SI+disp16]     AH / SP
 *  101     [DI]        [DI+disp8]      [DI+disp16]     CH / BP
 *  110     [disp16]    [BP+disp8]      [BP+disp16]     DH / SI
 *  111     [BX]        [BX+disp8]      [BX+disp16]     BH / DI
 */
bool CPU8068::get_address_mode_rm(const uint8_t mode, const uint8_t r_m,
                                  uint16_t& address) {
  switch (r_m) {
    case 0b000:
      address = BX + SI;
      break;
    case 0b001:
      address = BX + DI;
      break;
    case 0b010:
      address = BP + SI;
      break;
    case 0b011:
      address = BP + DI;
      break;
    case 0b100:
      address = SI;
      break;
    case 0b101:
      address = DI;
      break;
    case 0b110: {
      if (mode == 0b01 || mode == 0b10) {
        address = BP;
        break;
      }
      if (mode == 0b00) {
        address = mem16(CS, IP);
        IP += 2;
        break;
      }
      return false;
    }
    case 0b111:
      address = BX;
      break;
    default:
      return false;
  }
  if (mode == 0b01) {
    address += static_cast<int8_t>(mem8(CS, IP++));
  } else if (mode == 0b10) {
    address += static_cast<int16_t>(mem16(CS, IP));
    IP += 2;
  }

  return true;
}
