#pragma once

#include <string>

#include "MemoryBus.h"

class System {
 public:
  void Initialize(const std::string& bios_path);

 private:
  MemoryBus memory_bus;
};
