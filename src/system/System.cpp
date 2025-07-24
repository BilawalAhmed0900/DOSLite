#include "System.h"

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../utils/LoadFile.h"
#include "../utils/Logger.h"
#include "BIOSROM.h"

void System::Initialize(const std::string& bios_path) {
  {
    std::vector<uint8_t> bios_data;
    if (!LoadFile(bios_path, bios_data)) {
      MYLOG("Loading of bios '%s' failed", bios_path.c_str());
      throw std::runtime_error("Cannot load bios");
    }

    if (bios_data.size() > BIOSROM::MAX_REAL_SIZE) {
      std::unique_ptr<BIOSROM> bios =
          std::make_unique<BIOSROM>(std::vector<uint8_t>{
              bios_data.end() - BIOSROM::MAX_REAL_SIZE, bios_data.end()});
      memory_bus.AddMemoryDevice(std::move(bios));
    } else {
      std::unique_ptr<BIOSROM> bios = std::make_unique<BIOSROM>(bios_data);
      memory_bus.AddMemoryDevice(std::move(bios));
    }
  }
}
