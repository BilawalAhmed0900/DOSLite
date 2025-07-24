#pragma once

#include <memory>
#include <vector>

#include "MemoryDevice.h"

class MemoryBus {
 public:
  void AddMemoryDevice(std::unique_ptr<MemoryDevice>&& device);

 private:
  std::vector<std::unique_ptr<MemoryDevice>> memory_devices;
};
