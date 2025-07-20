#include <cstdint>

#include "../../Utils/logger.h"
#include "../CPU8068.h"

void CPU8068::lea_reg_rm(uint8_t mod_rm) {
  const uint8_t mode = ((mod_rm >> 6) & 0b011);
  const uint8_t reg = ((mod_rm >> 3) & 0b111);
  const uint8_t r_m = ((mod_rm >> 0) & 0b111);

  if (mode == 0b11) {
    mylog("Unsupported mode bit");
    return;
  }

  uint16_t address;
  uint16_t segment;
  if (!get_address_mode_rm(mode, r_m, segment, address)) {
    mylog("Unsupported r/m bit");
    return;
  }

  (void)segment;
  *reg16[reg] = address;
}
