#include <cstdint>

#include "../../Utils/logger.h"
#include "../CPU8068.h"

void CPU8068::mov_es_di_ds_si(uint8_t width) {
  if (width != 8 && width != 16) {
    mylog("Unsupported width in mov_es_di_ds_si");
    return;
  }

  if (width == 16) {
    mem16(ES, DI) = mem16(DS, SI);
    if (DF()) {
      DI -= 2;
      SI -= 2;
    } else {
      DI += 2;
      SI += 2;
    }
  } else {
    mem8(ES, DI) = mem8(DS, SI);
    if (DF()) {
      DI -= 1;
      SI -= 1;
    } else {
      DI += 1;
      SI += 1;
    }
  }
}

void CPU8068::lods_ds_si(uint8_t width) {
  if (width != 8 && width != 16) {
    mylog("Unsupported width in lods_ds_si");
    return;
  }

  if (width == 16) {
    AX = mem16(DS, SI);
    if (DF()) {
      SI -= 2;
    } else {
      SI += 2;
    }
  } else if (width == 8) {
    AL = mem8(DS, SI);
    if (DF()) {
      SI -= 1;
    } else {
      SI += 1;
    }
  }
}

void CPU8068::stos_es_di(uint8_t width) {
  if (width != 8 && width != 16) {
    mylog("Unsupported width in stos_es_di");
    return;
  }

  if (width == 16) {
    mem16(ES, DI) = AX;
    if (DF()) {
      DI -= 2;
    } else {
      DI += 2;
    }
  } else if (width == 8) {
    mem8(ES, DI) = AL;
    if (DF()) {
      DI -= 1;
    } else {
      DI += 1;
    }
  }
}

void CPU8068::cmps_es_di_ds_si(uint8_t width) {
  if (width != 8 && width != 16) {
    mylog("Unsupported width in cmps_es_di_ds_si");
    return;
  }

  if (width == 16) {
    const uint32_t lhs = mem16(DS, SI);
    const uint32_t rhs = mem16(ES, DI);
    const uint32_t result = lhs - rhs;
    set_flags_sub(lhs, rhs, result, 16);
    if (DF()) {
      DI -= 2;
      SI -= 2;
    } else {
      DI += 2;
      SI += 2;
    }
  } else if (width == 8) {
    const uint16_t lhs = mem8(DS, SI);
    const uint16_t rhs = mem8(ES, DI);
    const uint16_t result = lhs - rhs;
    set_flags_sub(lhs, rhs, result, 8);
    if (DF()) {
      DI -= 1;
      SI -= 1;
    } else {
      DI += 1;
      SI += 1;
    }
  }
}

void CPU8068::scas_es_di(uint8_t width) {
  if (width != 8 && width != 16) {
    mylog("Unsupported width in scas_es_di");
    return;
  }

  if (width == 16) {
    const uint32_t lhs = AX;
    const uint32_t rhs = mem16(ES, DI);
    const uint32_t result = lhs - rhs;
    set_flags_sub(lhs, rhs, result, 16);
    if (DF()) {
      DI -= 2;
    } else {
      DI += 2;
    }
  } else if (width == 8) {
    const uint16_t lhs = AL;
    const uint16_t rhs = mem8(ES, DI);
    const uint16_t result = lhs - rhs;
    set_flags_sub(lhs, rhs, result, 8);
    if (DF()) {
      DI -= 1;
    } else {
      DI += 1;
    }
  }
}
