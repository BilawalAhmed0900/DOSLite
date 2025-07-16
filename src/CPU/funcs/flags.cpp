#include <cstdint>

#include "../../Utils/logger.h"
#include "../CPU8068.h"

static uint8_t count_set_bits(const uint32_t num) {
  uint8_t count = 0;
  for (int i = 0; i < sizeof(num) * 8; i++) {
    count += (num >> i) & 1;
  }

  return count;
}

bool CPU8068::is_AF(const uint16_t lhs, const uint16_t rhs,
                    const uint32_t result) {
  // Auxiliary flag
  // A carry flag that is used to check if carry has happened
  // from left nibble to right nibble
  //
  //   |        |
  //   |-Left   |-Right
  // 0b0000     0000
  //
  // Instead of manually checking, used the formula from here
  // https://retrocomputing.stackexchange.com/questions/11262/can-someone-explain-this-algorithm-used-to-compute-the-auxiliary-carry-flag
  return ((lhs ^ rhs ^ result) & 0x10) != 0;
}

void CPU8068::adjust_flags(const uint32_t result, const uint8_t width) {
  if (width != 8 && width != 16) {
    mylog("Unsupported width in adjust_flags");
    return;
  }

  // Since the maximum sum can only go maximum 1 bit ahead
  // e.g., 0xFF + 0xFF = 0x1FE,
  // Carry Flag
  const uint8_t CF = (result >> width) & 0x1;

  // Whole result is 0 or not, after addition, in the given width
  // e.g., 0x80 + 0x80 = 0x100, i.e. 0 is the 8 bit width
  // Zero Flag
  const uint8_t ZF = (result & ((1u << width) - 1)) == 0;

  // Left most digit in the width of the result is 1
  // Sign Flag
  const uint8_t SF = (result & (1u << (width - 1))) != 0;

  // Lowest 8 bit have even numbers of ones
  // Parity Flag
  const uint8_t PF = !(count_set_bits(result & 0xFF) & 1);

  SetCF(CF);
  SetPF(PF);
  SetZF(ZF);
  SetSF(SF);
}

void CPU8068::set_flags_add(const uint16_t lhs, const uint16_t rhs,
                            const uint32_t result, const uint8_t width) {
  if (width != 8 && width != 16) {
    mylog("Unsupported width in set_flags_add");
    return;
  }

  const uint8_t AF = is_AF(lhs, rhs, result) ? 1 : 0;
  const uint8_t lhs_sign = (lhs & (1u << (width - 1))) != 0;
  const uint8_t rhs_sign = (rhs & (1u << (width - 1))) != 0;
  const uint8_t res_sign = (result & (1u << (width - 1))) != 0;
  // Overflow Flag
  // Example: We went from a region of negativeness to positiveness
  // i.e., 0x80 - 0x1 = 0x7F that is positive if signess is concerned
  //       0x80 is negative
  //       0x01 is positive
  //       0x7F is positive
  const uint8_t OF = (lhs_sign == rhs_sign) && (lhs_sign != res_sign);

  SetAF(AF);
  SetOF(OF);

  adjust_flags(result, width);
}

void CPU8068::set_flags_sub(const uint16_t lhs, const uint16_t rhs,
                            const uint32_t result, const uint8_t width) {
  if (width != 8 && width != 16) {
    mylog("Unsupported width in set_flags_add");
    return;
  }

  const uint8_t AF = is_AF(lhs, rhs, result) ? 1 : 0;
  const uint8_t lhs_sign = (lhs & (1u << (width - 1))) != 0;
  const uint8_t rhs_sign = (rhs & (1u << (width - 1))) != 0;
  const uint8_t res_sign = (result & (1u << (width - 1))) != 0;
  // Overflow Flag
  // Example: We went from a region of negativeness to positiveness
  // i.e., 0x80 - 0x1 = 0x7F that is positive if signess is concerned
  //       0x80 is negative
  //       0x01 is positive
  //       0x7F is positive
  const uint8_t OF = (lhs_sign != rhs_sign) && (lhs_sign != res_sign);

  SetAF(AF);
  SetOF(OF);

  adjust_flags(result, width);
}

void CPU8068::set_flags_logical(const uint32_t result, const uint8_t width) {
  if (width != 8 && width != 16) {
    mylog("Unsupported width in set_flags_logical");
    return;
  }

  /*
    This will already set to 0 in adjust_flags,
    but to be extra explicit clear
  */
  SetCF(0);
  SetAF(0);
  SetOF(0);

  adjust_flags(result, width);
}

uint8_t CPU8068::CF() const { return FLAGS & 0x1; }

void CPU8068::SetCF(const uint8_t val) {
  FLAGS &= ~CF_MASK;
  FLAGS |= (val ? CF_MASK : 0);
}

uint8_t CPU8068::PF() const { return (FLAGS >> 2) & 0x1; }

void CPU8068::SetPF(const uint8_t val) {
  FLAGS &= ~PF_MASK;
  FLAGS |= (val ? PF_MASK : 0);
}

uint8_t CPU8068::AF() const { return (FLAGS >> 4) & 0x1; }

void CPU8068::SetAF(const uint8_t val) {
  FLAGS &= ~AF_MASK;
  FLAGS |= (val ? AF_MASK : 0);
}

uint8_t CPU8068::ZF() const { return (FLAGS >> 6) & 0x1; }

void CPU8068::SetZF(const uint8_t val) {
  FLAGS &= ~ZF_MASK;
  FLAGS |= (val ? ZF_MASK : 0);
}

uint8_t CPU8068::SF() const { return (FLAGS >> 7) & 0x1; }

void CPU8068::SetSF(const uint8_t val) {
  FLAGS &= ~SF_MASK;
  FLAGS |= (val ? SF_MASK : 0);
}

uint8_t CPU8068::DF() const { return (FLAGS >> 10) & 0x1; }

void CPU8068::SetDF(uint8_t val) {
  FLAGS &= ~DF_MASK;
  FLAGS |= (val ? DF_MASK : 0);
}

uint8_t CPU8068::OF() const { return (FLAGS >> 11) & 0x1; }

void CPU8068::SetOF(const uint8_t val) {
  FLAGS &= ~OF_MASK;
  FLAGS |= (val ? OF_MASK : 0);
}

void CPU8068::SetIF(const uint8_t val) {
  FLAGS &= ~IF_MASK;
  FLAGS |= (val ? IF_MASK : 0);
}