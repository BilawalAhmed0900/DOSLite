#include <optional>
#include <string_view>

#include "CPU/CPU8068.h"
#include "Exceptions/ProgramExitedException.h"
#include "ExecutableFiles/COM.h"
#include "ExecutableFiles/MZExe.h"
#include "Utils/EnableCursorControl.h"
#include "Utils/LoadToCpu.h"
#include "Utils/logger.h"

int main(const int argc, const char* argv[]) {
  if (argc < 3) {
    mylog("Usage: %s <filename>", argv[0]);
    return 1;
  }

  const std::string_view input_filename{argv[2]};
  const char mode = argv[1][0];

  CPU8068 cpu;
  if (mode == 'c') {
    std::optional<COM> com{COM::open(input_filename)};
    if (!com) {
      mylog("Cannot open COM '%s'", input_filename.data());
      return -1;
    }

    LoadToCPU::load(cpu, com.value());
  } else if (mode == 'e') {
    std::optional<MZExe> mz{MZExe::open(input_filename)};
    if (!mz) {
      mylog("Cannot open MZ file '%s'", input_filename.data());
      return -1;
    }
  } else {
    mylog("Usage: %s <filename>", argv[0]);
    return 1;
  }

  try {
    EnableCursorControl _;
    cpu.execute();
  } catch (const ProgramExitedException& e) {
    return e.code;
  }
}
