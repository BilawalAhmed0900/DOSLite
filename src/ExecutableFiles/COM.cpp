//
// Created by Bilawal Ahmed on 17/Jun/2025.
//

#include "COM.h"

#include <fstream>

std::optional<COM> COM::open(const std::string_view &path) {
    std::ifstream iff(path.data(), std::ios::binary);
    if (!iff.is_open()) {
        return std::nullopt;
    }

    COM com;
    iff.seekg(0, std::ios::end);
    const std::streampos size = iff.tellg();
    iff.seekg(0, std::ios::beg);

    com.buffer.resize(size);
    iff.read(reinterpret_cast<char *>(com.buffer.data()), size);
    if (iff.gcount() != size) {
        return std::nullopt;
    }

    return com;
}
