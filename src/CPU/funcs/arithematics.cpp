#include "../CPU8068.h"

#include <cstdint>

#include "../../Utils/logger.h"

// Intel® 64 and IA-32 Architectures Software Developer’s Manual
/*
  IF 64-Bit Mode
  THEN
    #UD;
  ELSE
    old_AL := AL;
    old_CF := CF;
    CF := 0;
    IF (((AL AND 0FH) > 9) or AF = 1)
    THEN
      AL := AL + 6;
      CF := old_CF or (Carry from AL := AL + 6);
      AF := 1;
    ELSE
      AF := 0;
    FI;
    IF ((old_AL > 99H) or (old_CF = 1))
    THEN
      AL := AL + 60H;
      CF := 1;
    ELSE
      CF := 0;
    FI;
  FI;
*/
void CPU8068::DAA() {
  const uint8_t oldAL = AL;
  uint8_t oldCF = CF();
  SetCF(0);

  uint16_t newAL = AL;
  if (((AL & 0xF) > 9) || AF()) {
    newAL += 6;
    SetCF(oldCF | ((newAL >> 8) & 0x1));
    SetAF(is_AF(oldAL, 0x06, newAL));
  } else {
    SetAF(0);
  }

  if ((oldAL > 0x99) || oldCF) {
    newAL += 0x60;
    SetCF(1);
  } else {
    SetCF(0);
  }

  oldCF = CF();
  adjust_flags(newAL, 8);
  SetCF(oldCF);
  AL = static_cast<uint8_t>(newAL);
}

// Intel® 64 and IA-32 Architectures Software Developer’s Manual
/*
IF 64-Bit Mode
THEN
  #UD;
ELSE
  old_AL := AL;
  old_CF := CF;
  CF := 0;
  IF (((AL AND 0FH) > 9) or AF = 1)
  THEN
    AL := AL - 6;
    CF := old_CF or (Borrow from AL := AL − 6);
    AF := 1;
  ELSE
    AF := 0;
  FI;
  IF ((old_AL > 99H) or (old_CF = 1))
  THEN
    AL := AL − 60H;
    CF := 1;
  FI;
FI;
*/
void CPU8068::DAS() {
  const uint8_t oldAL = AL;
  uint8_t oldCF = CF();
  SetCF(0);

  uint16_t newAL = oldAL;
  if (((oldAL & 0xF) > 9) || AF()) {
    newAL = oldAL - 0x06;

    SetCF(oldCF | ((oldAL < 0x06) ? 1 : 0));
    SetAF(is_AF(oldAL, 0x06, newAL));
  }

  if ((oldAL > 0x99) || oldCF) {
    newAL -= 0x60;
    SetCF(1);
  }

  oldCF = CF();
  adjust_flags(newAL, 8);
  SetCF(oldCF);
  AL = static_cast<uint8_t>(newAL);
}

// Intel® 64 and IA-32 Architectures Software Developer’s Manual
/*
IF 64-Bit Mode
THEN
  #UD;
ELSE
  IF ((AL AND 0FH) > 9) or (AF = 1)
  THEN
    AX := AX + 106H;
    AF := 1;
    CF := 1;
  ELSE
    AF := 0;
    CF := 0;
  FI;
  AL := AL AND 0FH;
FI;
*/
void CPU8068::AAA() {
  if (((AL & 0x0F) > 9) || AF()) {
    AX += 0x106;
    SetAF(1);
    SetCF(1);
  } else {
    SetCF(0);
    SetAF(0);
  }

  AL &= 0x0F;
}

// Intel® 64 and IA-32 Architectures Software Developer’s Manual
/*
IF 64-bit mode
THEN
  #UD;
ELSE
  IF ((AL AND 0FH) > 9) or (AF = 1)
  THEN
    AX := AX – 6;
    AH := AH – 1;
    AF := 1;
    CF := 1;
    AL := AL AND 0FH;
  ELSE
    CF := 0;
    AF := 0;
    AL := AL AND 0FH;
  FI;
FI;
*/
void CPU8068::AAS() {
  if (((AL & 0x0F) > 9) || AF()) {
    AX -= 0x6;
    AH -= 0x1;
    SetAF(1);
    SetCF(1);
  } else {
    SetCF(0);
    SetAF(0);
  }

  AL &= 0x0F;
}

/*
IF 64-Bit Mode
THEN
  #UD;
ELSE
  tempAL := AL;
  AH := tempAL / imm8; (* imm8 is set to 0AH for the AAM mnemonic *)
  AL := tempAL MOD imm8;
FI;
*/
void CPU8068::AAM(const uint8_t base) {
  // Base 10 is mostly default
  if (base != 0x0A) {
    mylog("AAM called with non-default base, %d", (int)base);
  }

  const uint8_t al = AL;
  AH = al / base;
  AL = al % base;

  set_flags_logical(AL, 8);
}

/*
IF 64-Bit Mode
THEN
  #UD;
ELSE
  tempAL := AL;
  tempAH := AH;
  AL := (tempAL + (tempAH ∗ imm8)) AND FFH;
  (* imm8 is set to 0AH for the AAD mnemonic.*)
  AH := 0;
FI;
*/
void CPU8068::AAD(const uint8_t base) {
  // Base 10 is mostly default
  if (base != 0x0A) {
    mylog("AAD called with non-default base, %d", (int)base);
  }

  const uint8_t al = AL;
  const uint8_t ah = AH;
  AL = (al + (ah * base)) & 0xFF;
  AH = 0;

  set_flags_logical(AL, 8);
}
