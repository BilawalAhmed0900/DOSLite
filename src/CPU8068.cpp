//
// Created by Bilawal Ahmed on 18/May/2025.
//

#include "CPU8068.h"

#include "logger.h"
#include "ProgramExitedException.h"

CPU8068::CPU8068(): memory(MEMORY_SIZE, 0) {
    AX = BX = CX = DX = 0;
    SP = BP = SI = DI = 0;
    CS = DS = SS = ES = 0;
    IP = 0;
    FLAGS = 0;
}

void CPU8068::reset_registers() {
    AX = BX = CX = DX = 0;
    SP = BP = SI = DI = 0;
}

void CPU8068::execute() {
    while (true) {
        const uint8_t opcode = mem8(CS, IP++);
        switch (opcode) {
            // MOV
            case 0xB0:
                AL = mem8(CS, IP++);
                break;
            case 0xB1:
                CL = mem8(CS, IP++);
                break;
            case 0xB2:
                DL = mem8(CS, IP++);
                break;
            case 0xB3:
                BL = mem8(CS, IP++);
                break;
            case 0xB4:
                AH = mem8(CS, IP++);
                break;
            case 0xB5:
                CH = mem8(CS, IP++);
                break;
            case 0xB6:
                DH = mem8(CS, IP++);
                break;
            case 0xB7:
                BH = mem8(CS, IP++);
                break;
            case 0xB8:
                AX = mem16(CS, IP);
                IP += 2;
                break;
            case 0xB9:
                CX = mem16(CS, IP);
                IP += 2;
                break;
            case 0xBA:
                DX = mem16(CS, IP);
                IP += 2;
                break;
            case 0xBB:
                BX = mem16(CS, IP);
                IP += 2;
                break;
            case 0xBC:
                SP = mem16(CS, IP);
                IP += 2;
                break;
            case 0xBD:
                BP = mem16(CS, IP);
                IP += 2;
                break;
            case 0xBE:
                SI = mem16(CS, IP);
                IP += 2;
                break;
            case 0xBF:
                DI = mem16(CS, IP);
                IP += 2;
                break;
            // INT
            case 0xCD: {
                const uint8_t num = mem8(CS, IP++);
                interrupt(num);
                break;
            }
            default:
                mylog("Unsupported opcode '%.02X'", (int)opcode);
                return;
        }
    }
}

uint8_t &CPU8068::mem8(const uint16_t CS, const uint16_t IP) {
    return memory[(CS * SEGMENT_SIZE) + IP];
}

uint16_t &CPU8068::mem16(const uint16_t CS, const uint16_t IP) {
    return *reinterpret_cast<uint16_t*>(&memory[(CS * SEGMENT_SIZE) + IP]);
}

void CPU8068::interrupt(const uint8_t num) {
    switch (num) {
        case 0x21:
            dos_interrupt();
            break;
        default:
            mylog("Unsupported interrupt");
            break;
    }
}

void CPU8068::dos_interrupt() {
    switch (AH) {
        case 0x09: {
            const uint8_t *string = &mem8(DS, DX);

            constexpr int32_t MAX_STRING_LENGTH = SEGMENT_SIZE;
            int32_t len = 0;
            while (string[len++] != '$') {
                if (len > MAX_STRING_LENGTH) {
                    mylog("String too long, no printing");
                    return;
                }
            }

            while (*string != '$') {
                if (isprint(*string) || *string == '\t' || *string == '\r' || *string == '\n' || *string == '\a') {
                    std::cout << static_cast<char>(*string++);
                } else if (*string == '\b') {
                    string++;
                    // Go back one, add space ' ', go back once, emulates deleting one character
                    std::cout << "\x1b[1D \x1b[1D";
                } else {
                    string++;
                }
            }
            break;
        }
        case 0x4C:
            throw ProgramExitedException{AL};
        default:
            mylog("Unsupported interrupt");
            break;
    }
}
