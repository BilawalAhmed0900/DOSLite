#include "MemoryBus.h"

#include <memory>
#include <utility>

#include "MemoryDevice.h"

void MemoryBus::AddMemoryDevice(std::unique_ptr<MemoryDevice>&& device) {
  this->memory_devices.push_back(std::move(device));
}
