#include <iostream>
#include <string_view>

#include "COM.h"
#include "CPU8068.h"
#include "EnableCursorControl.h"
#include "LoadToCpu.h"
#include "logger.h"
#include "MZExe.h"
#include "ProgramExitedException.h"

int main(const int argc, const char *argv[])
{
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
	}

	try {
		EnableCursorControl _;
		cpu.execute();
	}
	catch (const ProgramExitedException& e) {
		return e.code;
	}
}
