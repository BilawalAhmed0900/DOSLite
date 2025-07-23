#pragma once

#include <cstdint>
#include <vector>

#include "MemoryBus.h"
#include "MemoryDevice.h"

class ROM : public MemoryDevice {
 public:
  ROM(const uint32_t starting_addr, const uint32_t ending_addr,
      const std::vector<uint8_t>& data);
  virtual ~ROM() = default;

  bool owns(const size_t address) override;

  void write08(const size_t address, const uint8_t val) override;
  void write16(const size_t address, const uint16_t val) override;
  void write32(const size_t address, const uint32_t val) override;

  void read08(const size_t address, Read08Callback& callback) override;
  void read16(const size_t address, Read16Callback& callback) override;
  void read32(const size_t address, Read32Callback& callback) override;

 private:
  uint32_t starting_addr;
  uint32_t ending_addr;
  std::vector<uint8_t> data;
};
