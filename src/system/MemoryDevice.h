#pragma once

#include <cstdint>
#include "MemoryBus.h"

class MemoryDevice {
 public:
  virtual ~MemoryDevice() {}

  virtual bool owns(const size_t address) = 0;

  virtual void write08(const size_t address, const uint8_t val) = 0;
  virtual void write16(const size_t address, const uint16_t val) = 0;
  virtual void write32(const size_t address, const uint32_t val) = 0;

  virtual void read08(const size_t address, Read08Callback& callback) = 0;
  virtual void read16(const size_t address, Read16Callback& callback) = 0;
  virtual void read32(const size_t address, Read32Callback& callback) = 0;
};
