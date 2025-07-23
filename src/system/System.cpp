#include "System.h"

#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../utils/Logger.h"
#include "BIOSROM.h"
#include "MemoryDevice.h"

void System::LoadBios(const std::string& path) {
  std::ifstream iff(path);
  if (!iff.is_open()) {
    MYLOG("Cannot open bios file: %s", path.c_str());
    return;
  }

  std::vector<uint8_t> bios_data;
  iff.seekg(0, std::ios::end);
  bios_data.resize(iff.tellg());
  iff.seekg(0, std::ios::beg);

  iff.read(reinterpret_cast<char*>(bios_data.data()), bios_data.size());
  if (iff.gcount() != bios_data.size()) {
    MYLOG("Cannot read bios file: %s", path.c_str());
    return;
  }

  if (bios_data.size() >= BIOSROM::MAX_REAL_SIZE) {
    std::vector<uint8_t> real_bios_data{
        bios_data.end() - BIOSROM::MAX_REAL_SIZE, bios_data.end()};
    std::unique_ptr<MemoryDevice> bios =
        std::make_unique<BIOSROM>(real_bios_data);
    this->memory_devices.push_back(std::move(bios));
  } else {
    std::unique_ptr<MemoryDevice> bios = std::make_unique<BIOSROM>(bios_data);
    this->memory_devices.push_back(std::move(bios));
  }
}
