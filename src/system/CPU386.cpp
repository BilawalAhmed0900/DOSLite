#include "CPU386.h"

CPU386::CPU386() { Reset(); }

void CPU386::Reset() {
  EAX = ECX = EDX = EBX = ESP = EBP = ESI = EDI = 0;

  EFLAGS = 0;
  FLAGS_R1 = 1;

  CR0 = CR2 = CR3 = 0;
  CR0_NW = 1;
  CR0_CD = 1;
  CS = SS = DS = ES = FS = GS = 0;

  gdtr = {0, 0};

  /*
    According to Intel specs, at reset:
    - CS:IP = 0xFFFF:FFF0 (physical address 0xFFFFFFF0)
    - The CPU sets address lines A20-A31 high initially
    - On the first far jump, the CPU drops bits 20-31 to
      switch to the lower 1MB address space (where BIOS lives)

    This is quite complex to emulate exactly,
    so instead, we directly set CS:IP to 0xF000:0xFFF0,
    matching the BIOS address in the 1MB real-mode memory map.
  */
  CS = 0xF000;
  IP = 0xFFF0;
}
