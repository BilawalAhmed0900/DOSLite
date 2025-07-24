#include "ROM.h"

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "../utils/Logger.h"
#include "MemoryDevice.h"
#include "Callbacks.h"

ROM::ROM(const uint32_t starting_addr, const uint32_t ending_addr,
         const std::vector<uint8_t>& data)
    : MemoryDevice(), starting_addr(starting_addr), ending_addr(ending_addr) {
  if (starting_addr >= ending_addr) {
    MYLOG("Invalid starting address: %d, and ending address: %d",
          (int)starting_addr, (int)ending_addr);
    throw std::runtime_error("Invalid starting and ending address in ROM::ROM");
  }

  const uint32_t max_size = static_cast<int32_t>(ending_addr) -
                            static_cast<int32_t>(starting_addr) -
                            1 /* since end exclusive */;
  if (data.size() > max_size) {
    MYLOG("Invalid ROM size. Max size allowed: %d", (int)max_size);
    throw std::runtime_error("Invalid size for data in ROM::ROM");
  }

  const uint32_t base = max_size - static_cast<uint32_t>(data.size());
  this->data.resize(max_size);
  std::copy(data.begin(), data.end(), this->data.begin() + base);
}

bool ROM::owns(const size_t address) {
  MYLOG("ROM::owns called with address: %d", (int)address);
  return address >= starting_addr && address < ending_addr;
}

void ROM::write08(const size_t address, const uint8_t val) {
  MYLOG("ROM::write08 called with address: %d, and val: %d", (int)address,
        (int)val);
}

void ROM::write16(const size_t address, const uint16_t val) {
  MYLOG("ROM::write16 called with address: %d, and val: %d", (int)address,
        (int)val);
}

void ROM::write32(const size_t address, const uint32_t val) {
  MYLOG("ROM::write32 called with address: %d, and val: %d", (int)address,
        (int)val);
}

void ROM::read08(const size_t address, Read08Callback& callback) {
  const uint8_t result = data[address - starting_addr];
  MYLOG("ROM::read08 called with address: %d, returning: %.02X", (int)address,
        (int)result);
  callback(result);
}

void ROM::read16(const size_t address, Read16Callback& callback) {
  const uint16_t result =
      *reinterpret_cast<uint16_t*>(&data[address - starting_addr]);
  MYLOG("ROM::read16 called with address: %d, returning: %.02X", (int)address,
        (int)result);
  callback(result);
}

void ROM::read32(const size_t address, Read32Callback& callback) {
  const uint32_t result =
      *reinterpret_cast<uint32_t*>(&data[address - starting_addr]);
  MYLOG("ROM::read32 called with address: %d, returning: %.02X", (int)address,
        (int)result);
  callback(result);
}
