#include <cstdint>

#include "../../Utils/logger.h"
#include "../CPU8068.h"
#include "../CPUMode.h"

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
                                  uint16_t& segment,
                                  uint16_t& address) {
  segment = DS;
  switch (r_m) {
    case 0b000:
      address = BX + SI;
      break;
    case 0b001:
      address = BX + DI;
      break;
    case 0b010:
      address = BP + SI;
      segment = SS;
      break;
    case 0b011:
      address = BP + DI;
      segment = SS;
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
        segment = SS;
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

uint32_t CPU8068::ROL(uint32_t val, uint8_t width, uint8_t count,
                      uint8_t& last_bit_rotated) {
  if (width != 8 && width != 16) {
    mylog("Incorrect width in ROL, returning original value");
    return val;
  }

  if (count == 0) {
    return val;
  }

  uint32_t result = val;
  for (uint8_t i = 0; i < count; i++) {
    uint8_t msb = (result >> (width - 1)) & 0x1;
    result <<= 1;
    result &= ((static_cast<uint32_t>(1) << static_cast<uint32_t>(width)) - static_cast<uint32_t>(1));
    result |= msb;
    last_bit_rotated = msb;
  }

  return result;
}

uint32_t CPU8068::ROR(uint32_t val, uint8_t width, uint8_t count,
                      uint8_t& last_bit_rotated) {
  if (width != 8 && width != 16) {
    mylog("Incorrect width in ROR, returning original value");
    return val;
  }

  if (count == 0) {
    return val;
  }

  uint32_t result = val;
  for (uint8_t i = 0; i < count; i++) {
    uint8_t lsb = result & 0x1;
    result >>= 1;
    result |= (static_cast<uint32_t>(lsb) << (static_cast<uint32_t>(width) - static_cast<uint32_t>(1)));
    last_bit_rotated = lsb;
  }

  return result;
}

uint32_t CPU8068::RCL(uint32_t val, uint8_t width, uint8_t count,
                      uint8_t& last_bit_rotated) {
  if (width != 8 && width != 16) {
    mylog("Incorrect width in RCL, returning original value");
    return val;
  }

  if (count == 0) {
    return val;
  }

  val |= (CF() << width);
  width += 1;

  uint32_t result = val;
  for (uint8_t i = 0; i < count; i++) {
    uint8_t msb = (result >> (width - 1)) & 0x1;
    result <<= 1;
    result &= ((static_cast<uint32_t>(1) << static_cast<uint32_t>(width)) -
               static_cast<uint32_t>(1));
    result |= msb;
    last_bit_rotated = msb;
  }

  width--;
  SetCF(result >> width);
  result &= ((static_cast<uint32_t>(1) << static_cast<uint32_t>(width)) -
             static_cast<uint32_t>(1));

  return result;
}

uint32_t CPU8068::RCR(uint32_t val, uint8_t width, uint8_t count,
                      uint8_t& last_bit_rotated) {
  if (width != 8 && width != 16) {
    mylog("Incorrect width in RCR, returning original value");
    return val;
  }

  if (count == 0) {
    return val;
  }

  val |= (CF() << width);
  width += 1;

  uint32_t result = val;
  for (uint8_t i = 0; i < count; i++) {
    uint8_t lsb = result & 0x1;
    result >>= 1;
    result |= (static_cast<uint32_t>(lsb)
               << (static_cast<uint32_t>(width) - static_cast<uint32_t>(1)));
    last_bit_rotated = lsb;
  }

  width--;
  SetCF(result >> width);
  result &= ((static_cast<uint32_t>(1) << static_cast<uint32_t>(width)) -
             static_cast<uint32_t>(1));

  return result;
}

uint32_t CPU8068::SHL(uint32_t val, uint8_t width, uint8_t count,
                      uint8_t& last_bit_rotated) {
  if (width != 8 && width != 16) {
    mylog("Incorrect width in SHL, returning original value");
    return val;
  }

  if (count == 0) {
    return val;
  }

  uint32_t result = val;
  for (uint8_t i = 0; i < count; i++) {
    uint8_t msb = (result >> (width - 1)) & 0x1;
    result <<= 1;
    result &= ((static_cast<uint32_t>(1) << static_cast<uint32_t>(width)) -
               static_cast<uint32_t>(1));
    last_bit_rotated = msb;
  }

  return result;
}

uint32_t CPU8068::SHR(uint32_t val, uint8_t width, uint8_t count,
                      uint8_t& last_bit_rotated) {
  if (width != 8 && width != 16) {
    mylog("Incorrect width in SHR, returning original value");
    return val;
  }

  if (count == 0) {
    return val;
  }

  uint32_t result = val;
  for (uint8_t i = 0; i < count; i++) {
    uint8_t lsb = result & 0x1;
    result >>= 1;
    last_bit_rotated = lsb;
  }

  return result;
}

uint32_t CPU8068::SAR(uint32_t val, uint8_t width, uint8_t count,
                      uint8_t& last_bit_rotated) {
  if (width != 8 && width != 16) {
    mylog("Incorrect width in SAR, returning original value");
    return val;
  }

  if (count == 0) {
    return val;
  }

  const uint8_t sign = (val >> (width - 1)) & 0x1;

  uint32_t result = val;
  for (uint8_t i = 0; i < count; i++) {
    uint8_t lsb = result & 0x1;
    result >>= 1;
    result |= (static_cast<uint32_t>(sign) << static_cast<uint32_t>(width - 1));
    last_bit_rotated = lsb;
  }

  return result;
}

void CPU8068::update_segment_register(uint16_t reg) {
  if (cpu_mode == CPU_MODE::CPU_8086 || cpu_mode == CPU_MODE::CPU_80186) {
    mylog("CPU8068::update_segment_register called with reg: %ld",
          static_cast<long int>(reg));
  } else if (cpu_mode == CPU_MODE::CPU_80286) {
    mylog("CPU8068::update_segment_register called with reg: %ld",
          static_cast<long int>(reg));
  }
}
