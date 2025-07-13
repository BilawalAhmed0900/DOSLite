#include <cstdint>

#include "../../Utils/logger.h"
#include "../CPU8068.h"

void CPU8068::pop_rm(uint8_t mod_rm) {
  const uint8_t mode = ((mod_rm >> 6) & 0b011);
  const uint8_t reg = ((mod_rm >> 3) & 0b111);
  const uint8_t r_m = ((mod_rm >> 0) & 0b111);

  if (reg != 0b000) {
    mylog("Invalid reg value in CPU8068::pop_rm");
    return;
  }

  const uint16_t val = mem16(SS, SP);
  SP += 2;

  if (mode == 0b11) {
    *reg16[r_m] = val;
  } else if (mode == 0b00 || mode == 0b01 || mode == 0b10) {
    uint16_t address;
    uint16_t segment;
    if (!get_address_mode_rm(mode, r_m, segment, address)) {
      mylog("Unsupported r/m bit");
      return;
    }

    mem16(segment, address) = val;
  } else {
    mylog("Unsupported 0x8F");
  }
}
