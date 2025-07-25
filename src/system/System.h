#pragma once

#include <string>

#include "CPU386.h"
#include "MemoryBus.h"

class System {
 public:
  void Initialize(const std::string& bios_path);

 private:
  CPU386 cpu;
  MemoryBus memory_bus;
};
