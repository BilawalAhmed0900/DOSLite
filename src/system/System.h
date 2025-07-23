#pragma once

#include <memory>
#include <vector>
#include <string>

#include "MemoryDevice.h"

class System {
 public:
  void LoadBios(const std::string& path);

 private:
  std::vector<std::unique_ptr<MemoryDevice>> memory_devices;
};
