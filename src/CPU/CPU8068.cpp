//
// Created by Bilawal Ahmed on 18/May/2025.
//

#include "CPU8068.h"

#include <iostream>

#include "../Utils/logger.h"
#include "../Exceptions/ProgramExitedException.h"

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
            // mov moffs8       AX  (0xA2)
            // mov moffs16/32   AL  (0xA3)
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
            // B0 + r
            // mov r8, imm8
            case 0xB0 ... 0xB7:
                *reg8 [opcode - 0xB0] = mem8(CS, IP++);
                break;
            // B8 + r
            // mov r16, imm16
            case 0xB8 ... 0xBF:
                *reg16[opcode - 0xB8] = mem16(CS, IP);
                IP += 2;
                break;
            // mov r/m8      r8   (0x88)
            // mov r/m16/32  r32  (0x89)
            case 0x88 ... 0x89: {
                const uint8_t mod_rm = mem8(CS, IP++);

                const bool is_16bit = (opcode == 0x89);
                mov_rm_reg(mod_rm, is_16bit ? 16 : 8);
                break;
            }
            // mov r8     r/m8        (0x8A)
            // mov r16/32 r/m16/32    (0x8B)
            case 0x8A ... 0x8B: {
                const uint8_t mod_rm = mem8(CS, IP++);

                const bool is_16bit = (opcode == 0x8B);
                mov_reg_rm(mod_rm, is_16bit ? 16 : 8);
                break;
            }
            // mov r/m8     imm8        (0xC6)
            // mov r/m16/32 imm16/32    (0xC7)
            case 0xC6 ... 0xC7: {
                const uint8_t mod_rm = mem8(CS, IP++);

                const bool is_16bit = (opcode == 0xC7);
                mov_rm_imm(mod_rm, is_16bit ? 16 : 8);
                break;
            }
            // INT
            // int imm8
            case 0xCD: {
                const uint8_t num = mem8(CS, IP++);
                interrupt(num);
                break;
            }
            // ADD
            // add AL imm8
            case 0x04: {
                const uint8_t rhs = mem8(CS, IP++);
                const uint16_t result = AL + rhs;
                set_flags_add(AL, rhs, result, 8);
                AL = result & 0xFF;
                break;
            }
            // add eAX imm16/32
            case 0x05: {
                const uint16_t rhs = mem16(CS, IP);
                IP += 2;

                const uint32_t result = AX + rhs;
                set_flags_add(AX, rhs, result, 16);
                AX = result & 0xFFFF;
                break;
            }
            // SUB
            // sub AL imm8
            case 0x2C: {
                const uint8_t rhs = mem8(CS, IP++);
                const uint16_t result = AL - rhs;
                set_flags_sub(AL, rhs, result, 8);
                AL = result & 0xFF;
                break;
            }
            // sub eAX imm16/32
            case 0x2D: {
                const uint16_t rhs = mem16(CS, IP);
                IP += 2;

                const uint32_t result = AX - rhs;
                set_flags_sub(AX, rhs, result, 16);
                AX = result & 0xFFFF;
                break;
            }
            // INC
            // INC r16/32
            case 0x40 ... 0x47: {
                const uint8_t oldCF = CF();

                const uint32_t result = *reg16[opcode - 0x40] + 1;
                set_flags_add(*reg16[opcode - 0x40], 1, result, 16);
                *reg16[opcode - 0x40] = result;

                SetCF(oldCF);
                break;
            }
            // DEC
            // DEC r16/32
            case 0x48 ... 0x4F: {
                const uint8_t oldCF = CF();

                const uint32_t result = *reg16[opcode - 0x48] - 1;
                set_flags_sub(*reg16[opcode - 0x48], 1, result, 16);
                *reg16[opcode - 0x40] = result;

                SetCF(oldCF);
                break;
            }
            // JMP
            // jmp e8
            case 0xEB: {
                const int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
                IP += offset;
                break;
            }
            // jmp e16
            case 0xE9: {
                const int16_t offset = static_cast<int16_t>(mem16(CS, IP));
                IP += 2;
                IP += offset;
                break;
            }
            // DAA
            case 0x27:
                DAA();
                break;
            // DAS
            case 0x2F:
                DAS();
                break;
            // AAA
            case 0x37:
                AAA();
                break;
            // AAS
            case 0x3F:
                AAS();
                break;
            // NOP
            case 0x90:
                break;
            // CMP
            // cmp AL imm8
            case 0x3C: {
                const uint8_t rhs = mem8(CS, IP++);
                const uint16_t result = static_cast<uint16_t>(AL) - rhs;
                set_flags_sub(AL, rhs, result, 8);
                break;
            }
            // cmp eAX imm16/32
            case 0x3D: {
                const uint16_t rhs = mem16(CS, IP);
                IP += 2;

                const uint32_t result = static_cast<uint32_t>(AX) - rhs;
                set_flags_sub(AX, rhs, result, 16);
                break;
            }
            // JE
            // je rel8
            case 0x74: {
                int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
                if (ZF()) {
                    IP += offset;
                }
                break;
            }
            // jne rel8
            case 0x75: {
                int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
                if (!ZF()) {
                    IP += offset;
                }
                break;
            }
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

static uint8_t count_set_bits(const uint32_t num) {
    uint8_t count = 0;
    for (int i = 0; i < sizeof(num) * 8; i++) {
        count += (num >> i) & 1;
    }

    return count;
}

static bool is_AF(const uint16_t lhs, const uint16_t rhs, uint32_t const result) {
    // Auxiliary flag
    // A carry flag that is used to check if carry has happened
    // from left nibble to right nibble
    //
    //   |        |
    //   |-Left   |-Right
    // 0b0000     0000
    //
    // Instead of manually checking, used the formula from here
    // https://retrocomputing.stackexchange.com/questions/11262/can-someone-explain-this-algorithm-used-to-compute-the-auxiliary-carry-flag
    return ((lhs ^ rhs ^ result) & 0x10) != 0;
}

void CPU8068::adjust_flags(const uint32_t result, const uint8_t width) {
    if (width != 8 && width != 16) {
        mylog("Unsupported width in adjust_flags");
        return;
    }

    // Since the maximum sum can only go maximum 1 bit ahead
    // e.g., 0xFF + 0xFF = 0x1FE,
    // Carry Flag
    const uint8_t CF = (result >> width) & 0x1;

    // Whole result is 0 or not, after addition, in the given width
    // e.g., 0x80 + 0x80 = 0x100, i.e. 0 is the 8 bit width
    // Zero Flag
    const uint8_t ZF = (result & ((1u << width) - 1)) == 0;

    // Left most digit in the width of the result is 1
    // Sign Flag
    const uint8_t SF = (result & (1u << (width - 1))) != 0;

    // Lowest 8 bit have even numbers of ones
    // Parity Flag
    const uint8_t PF = !(count_set_bits(result & 0xFF) & 1);

    SetCF(CF);
    SetPF(PF);
    SetZF(ZF);
    SetSF(SF);
}

void CPU8068::set_flags_add(const uint16_t lhs, const uint16_t rhs, const uint32_t result, const uint16_t width) {
    if (width != 8 && width != 16) {
        mylog("Unsupported width in set_flags_add");
        return;
    }

    const uint8_t AF = is_AF(lhs, rhs, result) ? 1 : 0;
    const uint8_t lhs_sign = (lhs    & (1u << (width - 1))) != 0;
    const uint8_t rhs_sign = (rhs    & (1u << (width - 1))) != 0;
    const uint8_t res_sign = (result & (1u << (width - 1))) != 0;
    // Overflow Flag
    // Example: We went from a region of negativeness to positiveness
    // i.e., 0x80 - 0x1 = 0x7F that is positive if signess is concerned
    //       0x80 is negative
    //       0x01 is positive
    //       0x7F is positive
    const uint8_t OF = (lhs_sign == rhs_sign) && (lhs_sign != res_sign);


    SetAF(AF);
    SetOF(OF);

    adjust_flags(result, width);
}

void CPU8068::set_flags_sub(const uint16_t lhs, const uint16_t rhs, const uint32_t result, const uint16_t width) {
    if (width != 8 && width != 16) {
        mylog("Unsupported width in set_flags_add");
        return;
    }

    const uint8_t AF = is_AF(lhs, rhs, result) ? 1 : 0;
    const uint8_t lhs_sign = (lhs    & (1u << (width - 1))) != 0;
    const uint8_t rhs_sign = (rhs    & (1u << (width - 1))) != 0;
    const uint8_t res_sign = (result & (1u << (width - 1))) != 0;
    // Overflow Flag
    // Example: We went from a region of negativeness to positiveness
    // i.e., 0x80 - 0x1 = 0x7F that is positive if signess is concerned
    //       0x80 is negative
    //       0x01 is positive
    //       0x7F is positive
    const uint8_t OF = (lhs_sign != rhs_sign) && (lhs_sign != res_sign);

    SetAF(AF);
    SetOF(OF);

    adjust_flags(result, width);
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
        if (!get_address_mode_rm(mode, r_m, address)) {
            mylog("Unsupported r/m bit");
            return;
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

void CPU8068::mov_rm_imm(const uint8_t mod_rm, const uint8_t width) {
    if (width != 8 && width != 16) {
        mylog("Unsupported width in mov_rm_reg");
        return;
    }
    const uint8_t mode = ((mod_rm >> 6) & 0b011);
    const uint8_t reg  = ((mod_rm >> 3) & 0b111);
    const uint8_t r_m  = ((mod_rm >> 0) & 0b111);

    if (reg != 0) {
        mylog("Unsupported reg in mov_rm_imm");
        return;
    }

    if (mode == 0b11) {
        *reg8[r_m] = mem8(CS, IP++);
    } else if (mode == 0b00 || mode == 0b01 || mode == 0b10) {
        uint16_t address;
        if (!get_address_mode_rm(mode, r_m, address)) {
            mylog("Unsupported r/m bit");
            return;
        }

        if (width == 8) {
            mem8(DS, address) = mem8(CS, IP++);
        } else if (width == 16) {
            mem16(DS, address) = mem16(CS, IP);
            IP += 2;
        }
    } else {
        mylog("Unsupported 0xC6, 0xC7");
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
        if (!get_address_mode_rm(mode, r_m, address)) {
            mylog("Unsupported r/m bit");
            return;
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

bool CPU8068::get_address_mode_rm(const uint8_t mode, const uint8_t r_m, uint16_t &address) {
	// https://en.wikipedia.org/wiki/ModR/M
	// 16-bit mode
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
            return false;
        }
        case 0b111: address = BX;      break;
        default:
            return false;
    }
    if (mode == 0b01) {
        address += static_cast<int8_t>(mem8(CS, IP++));
    } else if (mode == 0b10) {
        address += static_cast<int16_t>(mem16(CS, IP));
        IP += 2;
    }

    return true;
}

// Intel® 64 and IA-32 Architectures Software Developer’s Manual
/*
	IF 64-Bit Mode
	THEN
		#UD;
	ELSE
		old_AL := AL;
		old_CF := CF;
		CF := 0;
		IF (((AL AND 0FH) > 9) or AF = 1)
		THEN
			AL := AL + 6;
			CF := old_CF or (Carry from AL := AL + 6);
			AF := 1;
		ELSE
			AF := 0;
		FI;
		IF ((old_AL > 99H) or (old_CF = 1))
		THEN
			AL := AL + 60H;
			CF := 1;
		ELSE
			CF := 0;
		FI;
	FI;
*/
void CPU8068::DAA() {
    const uint8_t oldAL = AL;
    uint8_t oldCF = CF();
    SetCF(0);

    uint16_t newAL = AL;
    if (((AL & 0xF) > 9) || AF()) {
        newAL += 6;
        SetCF(oldCF | ((newAL >> 8) & 0x1));
        SetAF(is_AF(oldAL, 0x06, newAL));
    } else {
        SetAF(0);
    }

    if ((oldAL > 0x99) || oldCF) {
        newAL += 0x60;
        SetCF(1);
    } else {
        SetCF(0);
    }

    oldCF = CF();
    adjust_flags(newAL, 8);
    SetCF(oldCF);
    AL = static_cast<uint8_t>(newAL);
}

// Intel® 64 and IA-32 Architectures Software Developer’s Manual
/*
IF 64-Bit Mode
THEN
	#UD;
ELSE
	old_AL := AL;
	old_CF := CF;
	CF := 0;
	IF (((AL AND 0FH) > 9) or AF = 1)
	THEN
		AL := AL - 6;
		CF := old_CF or (Borrow from AL := AL − 6);
		AF := 1;
	ELSE
		AF := 0;
	FI;
	IF ((old_AL > 99H) or (old_CF = 1))
	THEN
		AL := AL − 60H;
		CF := 1;
	FI;
FI;
*/
void CPU8068::DAS() {
    const uint8_t oldAL = AL;
    uint8_t oldCF = CF();
    SetCF(0);

    uint16_t newAL = oldAL;
    if (((oldAL & 0xF) > 9) || AF()) {
        newAL = oldAL - 0x06;

        SetCF(oldCF | ((oldAL < 0x06) ? 1 : 0));
        SetAF(is_AF(oldAL, 0x06, newAL));
    }

    if ((oldAL > 0x99) || oldCF) {
        newAL -= 0x60;
        SetCF(1);
    }

    oldCF = CF();
    adjust_flags(newAL, 8);
    SetCF(oldCF);
    AL = static_cast<uint8_t>(newAL);
}

// Intel® 64 and IA-32 Architectures Software Developer’s Manual
/*
IF 64-Bit Mode
THEN
	#UD;
ELSE
	IF ((AL AND 0FH) > 9) or (AF = 1)
	THEN
		AX := AX + 106H;
		AF := 1;
		CF := 1;
	ELSE
		AF := 0;
		CF := 0;
	FI;
	AL := AL AND 0FH;
FI;
*/
void CPU8068::AAA() {
    if (((AL & 0x0F) > 9) || AF()) {
        AX += 0x106;
        SetAF(1);
        SetCF(1);
    } else {
        SetCF(0);
        SetAF(0);
    }

    AL &= 0x0F;
}

// Intel® 64 and IA-32 Architectures Software Developer’s Manual
/*
IF 64-bit mode
THEN
	#UD;
ELSE
	IF ((AL AND 0FH) > 9) or (AF = 1)
	THEN
		AX := AX – 6;
		AH := AH – 1;
		AF := 1;
		CF := 1;
		AL := AL AND 0FH;
	ELSE
		CF := 0;
		AF := 0;
		AL := AL AND 0FH;
	FI;
FI;
*/
void CPU8068::AAS() {
    if (((AL & 0x0F) > 9) || AF()) {
        AX -= 0x6;
        AH -= 0x1;
        SetAF(1);
        SetCF(1);
    } else {
        SetCF(0);
        SetAF(0);
    }

    AL &= 0x0F;
}

uint8_t CPU8068::CF() const {
    return FLAGS & 0x1;
}

void CPU8068::SetCF(const uint8_t val) {
    FLAGS &= ~CF_MASK;
    FLAGS |= (val ? CF_MASK : 0);
}

uint8_t CPU8068::PF() const {
    return (FLAGS >> 2) & 0x1;
}

void CPU8068::SetPF(const uint8_t val) {
    FLAGS &= ~PF_MASK;
    FLAGS |= (val ? PF_MASK : 0);
}

uint8_t CPU8068::ZF() const {
    return (FLAGS >> 6) & 0x1;
}

void CPU8068::SetZF(const uint8_t val) {
    FLAGS &= ~ZF_MASK;
    FLAGS |= (val ? ZF_MASK : 0);
}

uint8_t CPU8068::SF() const {
    return (FLAGS >> 7) & 0x1;
}

void CPU8068::SetSF(const uint8_t val) {
    FLAGS &= ~SF_MASK;
    FLAGS |= (val ? SF_MASK : 0);
}

uint8_t CPU8068::AF() const {
    return (FLAGS >> 4) & 0x1;
}

void CPU8068::SetAF(const uint8_t val) {
    FLAGS &= ~AF_MASK;
    FLAGS |= (val ? AF_MASK : 0);
}

uint8_t CPU8068::OF() const {
    return (FLAGS >> 11) & 0x1;
}

void CPU8068::SetOF(const uint8_t val) {
    FLAGS &= ~OF_MASK;
    FLAGS |= (val ? OF_MASK : 0);
}
