//
// Created by Bilawal Ahmed on 18/May/2025.
//

#ifndef CPU8068_H
#define CPU8068_H

#include <cstdint>
#include <vector>

class LoadToCPU;

#pragma pack(push, 1)
class CPU8068 {
public:
    CPU8068();
    void reset_registers();
    void execute();

    uint8_t& mem8(uint16_t CS, uint16_t IP);
    uint16_t& mem16(uint16_t CS, uint16_t IP);

    // For now, not implementing the interrupt table
    void interrupt(uint8_t num);
    void dos_interrupt();

    friend class LoadToCPU;

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

    constexpr static size_t MEMORY_SIZE = 1 * 1024 * 1024;
    constexpr static size_t SEGMENT_SIZE = 64 * 1024;
    std::vector<uint8_t> memory;
};
#pragma pack(pop)

#endif //CPU8068_H
