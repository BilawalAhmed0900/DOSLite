//
// Created by Bilawal Ahmed on 18/May/2025.
//

#ifndef CPU8068_H
#define CPU8068_H

#include <cstdint>
#include <vector>

#pragma pack(push, 1)
class CPU8068 {
public:
    CPU8068();

private:
    union {
        struct {
            uint8_t AL, AH;
        };
        uint16_t AX;
    };

    union {
        struct {
            uint8_t BL, BH;
        };
        uint16_t BX;
    };

    union {
        struct {
            uint8_t CL, CH;
        };
        uint16_t CX;
    };

    union {
        struct {
            uint8_t DL, DH;
        };
        uint16_t DX;
    };

    uint16_t SP, BP, SI, DI;
    uint16_t CS, DS, SS, ES;
    uint16_t IP;
    uint16_t FLAGS;

    constexpr size_t MEMORY_SIZE = 16 * 1024 * 1024;
    std::vector<uint8_t> memory;
};
#pragma pack(pop)

#endif //CPU8068_H
