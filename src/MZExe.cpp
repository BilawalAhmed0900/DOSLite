//
// Created by Bilawal Ahmed on 22/May/2025.
//

#include "MZExe.h"

#include <cstring>
#include <filesystem>

#include "logger.h"

template <typename Type>
void read(std::ifstream &iif, Type& var) {
    iif.read(reinterpret_cast<char*>(&var), sizeof(Type));
    if (iif.gcount() != sizeof(Type)) {
        mylog("Cannot read from file");
        std::terminate();
    }
}

template <typename Type>
void read(std::ifstream &iif, std::vector<Type>& var) {
    iif.read(reinterpret_cast<char*>(var.data()), var.size() * sizeof(Type));
    if (iif.gcount() != var.size() * sizeof(Type)) {
        mylog("Cannot read from file");
        std::terminate();
    }
}

bool MZExe::get_mz_header(std::ifstream& file) {
    char MZ[2+1]{};
    file.read(MZ, 2);
    if (std::strcmp(MZ, "MZ") != 0 && std::strcmp(MZ, "ZM") != 0) {
        return false;
    }
    return true;
}

std::optional<MZExe> MZExe::open(const std::string_view &path) {
    std::ifstream file{path.data()};
    if (!file.is_open()) return std::nullopt;
    if (!get_mz_header(file)) return std::nullopt;

    MZExe mz{};
    read(file, mz.NumLastPageBytes);
    mylog("0x02: %d", static_cast<int>(mz.NumLastPageBytes));
    read(file, mz.NumPages);
    mylog("0x04: %d", static_cast<int>(mz.NumPages));
    read(file, mz.RelocationItems);
    mylog("0x06: %d", static_cast<int>(mz.RelocationItems));
    read(file, mz.NumHeaderParagraphs);
    mylog("0x08: %d", static_cast<int>(mz.NumHeaderParagraphs));
    read(file, mz.NumMinParagraphRequired);
    mylog("0x0A: %d", static_cast<int>(mz.NumMinParagraphRequired));
    read(file, mz.NumMaxParagraphRequested);
    mylog("0x0C: %d", static_cast<int>(mz.NumMaxParagraphRequested));
    read(file, mz.InitialSS);
    mylog("0x0E: %d", static_cast<int>(mz.InitialSS));
    read(file, mz.InitialSP);
    mylog("0x10: %d", static_cast<int>(mz.InitialSP));
    read(file, mz.Checksum);
    mylog("0x12: %d", static_cast<int>(mz.Checksum));
    read(file, mz.InitialIP);
    mylog("0x14: %d", static_cast<int>(mz.InitialIP));
    read(file, mz.InitialCS);
    mylog("0x16: %d", static_cast<int>(mz.InitialCS));
    read(file, mz.RelocationTableOffset);
    mylog("0x18: %d", static_cast<int>(mz.RelocationTableOffset));
    read(file, mz.Overlay);
    mylog("0x1A: %d", static_cast<int>(mz.Overlay));
    read(file, mz.OverlayInformation);
    mylog("0x1C: %d", static_cast<int>(mz.OverlayInformation));

    file.seekg(std::ios::end);
    const std::streampos size = file.tellg();
    file.seekg(std::ios::beg);
    mz.buffer.resize(size);
    read(file, mz.buffer);
    return mz;
}

