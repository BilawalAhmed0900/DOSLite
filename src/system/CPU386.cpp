#include "CPU386.h"

CPU386::CPU386() { Reset(); }

void CPU386::Reset() {
  EAX = ECX = EDX = EBX = ESP = EBP = ESI = EDI = 0;

  EFLAGS = 0;
  FLAGS_R1 = 1;

  CR0 = 0;
  CS = SS = DS = ES = FS = GS = 0;

  gdtr = {0, 0};

  CS = 0xF000;
  IP = 0xFFF0;
}
