//
// Created by Bilawal Ahmed on 18/May/2025.
//

#include "CPU8068.h"

#include <algorithm>

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
            case 0xA2 ... 0xA3: {
                const uint16_t address = mem16(CS, IP);
                IP += 2;

                if (opcode == 0xA3) {
                    mem16(DS, address) = AX;
                } else {
                    mem8(DS, address) = AL;
                }
                break;
            }
            case 0xB0 ... 0xB7:
                *reg8 [opcode - 0xB0] = mem8(CS, IP++);
                break;
            case 0xB8 ... 0xBF:
                *reg16[opcode - 0xB8] = mem16(CS, IP);
                IP += 2;
                break;
            case 0x88 ... 0x89: {
                const uint8_t mod_rm = mem8(CS, IP++);

                const bool is_16bit = (opcode == 0x89);
                mov_rm_reg(mod_rm, is_16bit ? 16 : 8);
                break;
            }
            case 0x8A ... 0x8B: {
                const uint8_t mod_rm = mem8(CS, IP++);

                const bool is_16bit = (opcode == 0x8B);
                mov_reg_rm(mod_rm, is_16bit ? 16 : 8);
                break;
            }
            // INT
            case 0xCD: {
                const uint8_t num = mem8(CS, IP++);
                interrupt(num);
                break;
            }
            // ADD
            case 0x04: {
                const uint8_t rhs = mem8(CS, IP++);
                const uint32_t result = AL + rhs;
                set_flags_add(AL, rhs, result, 8);
                AL = result & 0xFF;
                break;
            }
            case 0x05: {
                const uint16_t rhs = mem16(CS, IP);
                IP += 2;

                const uint32_t result = AX + rhs;
                set_flags_add(AX, rhs, result, 16);
                AX = result & 0xFFFF;
                break;
            }
            // INC
            case 0x40 ... 0x47:
                *reg16[opcode - 0x40] += 1;
                break;
            case 0x48 ... 0x4F:
                *reg16[opcode - 0x48] -= 1;
                break;
            default:
                mylog("Unsupported opcode '%.02X'", static_cast<int>(opcode));
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
        case 0x02: {
            std::cout << static_cast<char>(DL);
            break;
        }
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

static uint8_t count_set_bits(uint32_t num) {
    uint8_t count = 0;
    for (int i = 0; i < sizeof(num) * 8; i++) {
        count += (num >> i) & 1;
    }

    return count;
}

void CPU8068::set_flags_add(const uint16_t lhs, const uint16_t rhs, const uint32_t result, const uint16_t width) {
    // Since the maximum sum can only go maximum 1 bit ahead
    // e.g., 0xFF + 0xFF = 0x1FE,
    // Carry Flag
    uint8_t CF = (result >> width) & 0x1;

    // Whole result is 0 or not, after addition, in the given width
    // e.g., 0x80 + 0x80 = 0x100, i.e. 0 is the 8 bit width
    // Zero Flag
    uint8_t ZF = (result & ((1u << width) - 1)) == 0;

    // Left most digit in the width of the result is 1
    // Sign Flag
    uint8_t SF = (result & (1u << (width - 1))) != 0;

    // Auxiliary flag
    // 0x10 = 0b1'0000
    // Check if the 4th flag has been changed,
    // Neat trick by chatgpt :)
    uint8_t AF = ((lhs ^ rhs ^ result) & 0x10) != 0;

    uint8_t lhs_sign = (lhs    & (1u << (width - 1))) != 0;
    uint8_t rhs_sign = (rhs    & (1u << (width - 1))) != 0;
    uint8_t res_sign = (result & (1u << (width - 1))) != 0;
    // Overflow Flag
    uint8_t OF = (lhs_sign == rhs_sign) && (lhs_sign != res_sign);

    // Lowest 8 bit have even number of ones
    // Parity Flag
    uint8_t PF = count_set_bits(result & ((1 << width) - 1)) & 1;

    FLAGS &= ~(1 << 0);  // Clear out Carry Flag
    FLAGS &= ~(1 << 2);  // Clear out Parity Flag
    FLAGS &= ~(1 << 4);  // Clear out Auxiliary Flag
    FLAGS &= ~(1 << 6);  // Clear out Zero Flag
    FLAGS &= ~(1 << 7);  // Clear out Sign Flag
    FLAGS &= ~(1 << 11); // Clear out Overflow Flag
    FLAGS |= ((CF & 0x1) << 0);
    FLAGS |= ((PF & 0x1) << 2);
    FLAGS |= ((AF & 0x1) << 4);
    FLAGS |= ((ZF & 0x1) << 6);
    FLAGS |= ((SF & 0x1) << 7);
    FLAGS |= ((OF & 0x1) << 11);
}

void CPU8068::mov_rm_reg(const uint8_t mod_rm, const uint8_t width) {
    if (width != 8 && width != 16) {
        mylog("Unsupported width in mov_rm_reg");
        return;
    }
    const uint8_t mode = ((mod_rm >> 6) & 0b011);
    const uint8_t reg  = ((mod_rm >> 3) & 0b111);
    const uint8_t r_m  = ((mod_rm >> 0) & 0b111);

    if (mode == 0b11) {
        *reg8[r_m] = *reg8[reg];
    } else if (mode == 0b00 || mode == 0b01 || mode == 0b10) {
        uint16_t address;
        switch (r_m) {
            case 0b000: address = BX + SI; break;
            case 0b001: address = BX + DI; break;
            case 0b010: address = BP + SI; break;
            case 0b011: address = BP + DI; break;
            case 0b100: address = SI;      break;
            case 0b101: address = DI;      break;
            case 0b110: {
                if (mode == 0b01 || mode == 0b10) {
                    address = BP;
                    break;
                }
                if (mode == 0b00) {
                    address = mem16(CS, IP);
                    IP += 2;
                    break;
                }
                mylog("Unsupported r/m bit");
                return;
            }
            case 0b111: address = BX;      break;
            default:
                mylog("Unsupported r/m bit");
                return;
        }
        if (mode == 0b01) {
            address += static_cast<int8_t>(mem8(CS, IP++));
        } else if (mode == 0b10) {
            address += static_cast<int16_t>(mem16(CS, IP));
            IP += 2;
        }

        if (width == 8) {
            mem8(DS, address) = *reg8[reg];
        } else if (width == 16) {
            mem16(DS, address) = *reg16[reg];
        }
    } else {
        mylog("Unsupported 0x88, 0x89");
    }
}

void CPU8068::mov_reg_rm(const uint8_t mod_rm, const uint8_t width) {
    if (width != 8 && width != 16) {
        mylog("Unsupported width in mov_reg_rm");
        return;
    }
    const uint8_t mode = ((mod_rm >> 6) & 0b011);
    const uint8_t reg  = ((mod_rm >> 3) & 0b111);
    const uint8_t r_m  = ((mod_rm >> 0) & 0b111);

    if (mode == 0b11) {
        *reg8[reg] = *reg8[r_m];
    } else if (mode == 0b00 || mode == 0b01 || mode == 0b10) {
        uint16_t address;
        switch (r_m) {
            case 0b000: address = BX + SI; break;
            case 0b001: address = BX + DI; break;
            case 0b010: address = BP + SI; break;
            case 0b011: address = BP + DI; break;
            case 0b100: address = SI;      break;
            case 0b101: address = DI;      break;
            case 0b110: {
                if (mode == 0b01 || mode == 0b10) {
                    address = BP;
                    break;
                }
                if (mode == 0b00) {
                    address = mem16(CS, IP);
                    IP += 2;
                    break;
                }
                mylog("Unsupported r/m bit");
                return;
            }
            case 0b111: address = BX;      break;
            default:
                mylog("Unsupported r/m bit");
                return;
        }
        if (mode == 0b01) {
            address += static_cast<int8_t>(mem8(CS, IP++));
        } else if (mode == 0b10) {
            address += static_cast<int16_t>(mem16(CS, IP));
            IP += 2;
        }

        if (width == 8) {
            *reg8[reg] = mem8(DS, address);
        } else if (width == 16) {
            *reg16[reg] = mem16(DS, address);
        }
    } else {
        mylog("Unsupported 0x8A, 0x8B");
    }
}
