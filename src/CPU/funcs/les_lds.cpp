#include <cstdint>

#include "../../Utils/logger.h"
#include "../CPU8068.h"

void CPU8068::les_lds(const uint8_t mod_rm, const bool is_lds) {
  const uint8_t mode = ((mod_rm >> 6) & 0b011);
  const uint8_t reg = ((mod_rm >> 3) & 0b111);
  const uint8_t r_m = ((mod_rm >> 0) & 0b111);

  // DI and SI are not valid destinations
  if (mode == 0b11 || reg >= 6) {
    mylog("Unsupported mode or reg bit");
    return;
  }

  uint16_t addr_offset;
  uint16_t addr_segment;
  if (!get_address_mode_rm(mode, r_m, addr_segment, addr_offset)) {
    mylog("Unsupported r/m bit");
    return;
  }

  const uint16_t data = mem16(addr_segment, addr_offset);
  const uint16_t segment = mem16(addr_segment, addr_offset + 2);

  *reg16[reg] = data;
  if (is_lds) {
    DS = segment;
  } else {
    ES = segment;
  }
}