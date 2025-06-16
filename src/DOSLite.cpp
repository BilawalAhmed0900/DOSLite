#include <iostream>
#include <string_view>

#include "MZExe.h"

int main(const int argc, const char *argv[])
{
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
		return 1;
	}

	const std::string_view input_filename{argv[1]};
	std::optional<MZExe> mz{MZExe::open(input_filename)};

	return 0;
}
