//
// Created by Bilawal Ahmed on 17/Jun/2025.
//

#include "LoadToCpu.h"

#include <algorithm>

#include "../CPU/CPU8068.h"
#include "logger.h"

void LoadToCPU::load(CPU8068& cpu, const COM& com) {
  if (com.buffer.size() > 0x10000) {
    mylog("COM file cannot exceed 64kB memory in size");
    return;
  }
  if (com.buffer.size() > cpu.memory.size()) {
    mylog("COM file cannot exceed CPU memory restriction");
    return;
  }

  cpu.reset_registers();

  cpu.CS = cpu.DS = cpu.ES = cpu.SS = 0;
  cpu.IP = 0x100;  // as per specs
  std::copy(com.buffer.begin(), com.buffer.end(), cpu.memory.begin() + cpu.IP);
  cpu.SP = CPU8068::SEGMENT_SIZE - 1;

  // Interrupts enabled and reserved set to 1
  cpu.FLAGS = 0b0000'0010'0000'0010;
}
