//
// Created by Bilawal Ahmed on 18/May/2025.
//

#include "CPU8068.h"

#include <cctype>
#include <cstdint>
#include <iostream>

#include "../Exceptions/ProgramExitedException.h"
#include "../Utils/logger.h"

CPU8068::CPU8068() : memory(MEMORY_SIZE, 0) {
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
        // mov AL   moffs8   (0xA0)
        // mov AX   moffs16  (0xA1)
      case 0xA0:
      case 0xA1: {
        const uint16_t address = mem16(CS, IP);
        IP += 2;

        const bool is_16bit = (opcode == 0xA1);
        if (is_16bit) {
          AX = mem16(DS, address);
        } else {
          AL = mem8(DS, address);
        }
        break;
      }
        // mov moffs8       AX  (0xA2)
        // mov moffs16/32   AL  (0xA3)
      case 0xA2:
      case 0xA3: {
        const uint16_t address = mem16(CS, IP);
        IP += 2;

        if (opcode == 0xA3) {
          mem16(DS, address) = AX;
        } else {
          mem8(DS, address) = AL;
        }
        break;
      }
        // MOVS   m8  m8         (0xA4)
        // MOVSB  m8  m8         (0xA4)
        // MOVS   m16 m16        (0xA5)
        // MOVSW  m16 m16        (0xA5)
      case 0xA4:
      case 0xA5: {
        const bool is_16bit = (opcode == 0xA5);
        mov_es_di_ds_si(is_16bit ? 16 : 8);
        break;
      }
        // B0 + r
        // mov r8, imm8
      case 0xB0:
      case 0xB1:
      case 0xB2:
      case 0xB3:
      case 0xB4:
      case 0xB5:
      case 0xB6:
      case 0xB7: {
        *reg8[opcode - 0xB0] = mem8(CS, IP++);
        break;
      }
        // B8 + r
        // mov r16, imm16
      case 0xB8:
      case 0xB9:
      case 0xBA:
      case 0xBB:
      case 0xBC:
      case 0xBD:
      case 0xBE:
      case 0xBF: {
        *reg16[opcode - 0xB8] = mem16(CS, IP);
        IP += 2;
        break;
      }
        // mov r/m8      r8   (0x88)
        // mov r/m16/32  r32  (0x89)
      case 0x88:
      case 0x89: {
        const uint8_t mod_rm = mem8(CS, IP++);

        const bool is_16bit = (opcode == 0x89);
        mov_rm_reg(mod_rm, is_16bit ? 16 : 8);
        break;
      }
        // mov r8     r/m8        (0x8A)
        // mov r16/32 r/m16/32    (0x8B)
      case 0x8A:
      case 0x8B: {
        const uint8_t mod_rm = mem8(CS, IP++);

        const bool is_16bit = (opcode == 0x8B);
        mov_reg_rm(mod_rm, is_16bit ? 16 : 8);
        break;
      }
        // MOV m16   Sreg
        // MOV r16   Sreg
      case 0x8C: {
        const uint8_t mod_rm = mem8(CS, IP++);
        mov_rm_sreg(mod_rm, 16);
        break;
      }
        // MOV Sreg m16
        // MOV Sreg r16
      case 0x8E: {
        const uint8_t mod_rm = mem8(CS, IP++);
        mov_sreg_rm(mod_rm, 16);
        break;
      }
        // mov r/m8     imm8        (0xC6)
        // mov r/m16/32 imm16/32    (0xC7)
      case 0xC6:
      case 0xC7: {
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
        // ADD  r/m8  r8
        // ADD  r/m16 r16
      case 0x00:
      case 0x01: {
        const uint8_t mod_rm = mem8(CS, IP++);

        const bool is_16bit = (opcode == 0x01);
        add_rm_reg(mod_rm, (is_16bit) ? 16 : 8);
        break;
      }
        // ADD  r8  r/m8
        // ADD  r16 r/m16
      case 0x02:
      case 0x03: {
        const uint8_t mod_rm = mem8(CS, IP++);

        const bool is_16bit = (opcode == 0x03);
        add_reg_rm(mod_rm, (is_16bit) ? 16 : 8);
        break;
      }
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
        // SUB  r/m8  r8
        // SUB  r/m16 r16
      case 0x28:
      case 0x29: {
        const uint8_t mod_rm = mem8(CS, IP++);

        const bool is_16bit = (opcode == 0x29);
        sub_rm_reg(mod_rm, (is_16bit) ? 16 : 8);
        break;
      }
        // SUB  r8  r/m8
        // SUB  r16 r/m16
      case 0x2A:
      case 0x2B: {
        const uint8_t mod_rm = mem8(CS, IP++);

        const bool is_16bit = (opcode == 0x2B);
        sub_reg_rm(mod_rm, (is_16bit) ? 16 : 8);
        break;
      }
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
      case 0x40:
      case 0x41:
      case 0x42:
      case 0x43:
      case 0x44:
      case 0x45:
      case 0x46:
      case 0x47: {
        const uint8_t oldCF = CF();

        const uint32_t result = *reg16[opcode - 0x40] + 1;
        set_flags_add(*reg16[opcode - 0x40], 1, result, 16);
        *reg16[opcode - 0x40] = result;

        SetCF(oldCF);
        break;
      }
        // DEC
        // DEC r16/32
      case 0x48:
      case 0x49:
      case 0x4A:
      case 0x4B:
      case 0x4C:
      case 0x4D:
      case 0x4E:
      case 0x4F: {
        const uint8_t oldCF = CF();

        const uint32_t result = *reg16[opcode - 0x48] - 1;
        set_flags_sub(*reg16[opcode - 0x48], 1, result, 16);
        *reg16[opcode - 0x48] = result;

        SetCF(oldCF);
        break;
      }
        // JMP
        // JZ e8
      case 0x74: {
        const int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
        if (ZF()) {
          IP += offset;
        }
        break;
      }
        // JNZ e8
      case 0x75: {
        const int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
        if (!ZF()) {
          IP += offset;
        }
        break;
      }
        // JBE e8
      case 0x76: {
        const int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
        if (CF() || ZF()) {
          IP += offset;
        }
        break;
      }
        // JNBE e8
      case 0x77: {
        const int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
        if (!CF() && !ZF()) {
          IP += offset;
        }
        break;
      }
        // JS e8
      case 0x78: {
        const int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
        if (SF()) {
          IP += offset;
        }
        break;
      }
        // JNS e8
      case 0x79: {
        const int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
        if (!SF()) {
          IP += offset;
        }
        break;
      }
        // JP e8
      case 0x7A: {
        const int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
        if (PF()) {
          IP += offset;
        }
        break;
      }
        // JNP e8
      case 0x7B: {
        const int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
        if (!PF()) {
          IP += offset;
        }
        break;
      }
        // JL e8
      case 0x7C: {
        const int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
        if (SF() != OF()) {
          IP += offset;
        }
        break;
      }
        // JNL e8
      case 0x7D: {
        const int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
        if (SF() == OF()) {
          IP += offset;
        }
        break;
      }
        // JLE e8
      case 0x7E: {
        const int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
        if (ZF() && (SF() != OF())) {
          IP += offset;
        }
        break;
      }
        // JNLE e8
      case 0x7F: {
        const int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
        if (!ZF() && (SF() == OF())) {
          IP += offset;
        }
        break;
      }
        // jmp e16
      case 0xE9: {
        const int16_t offset = static_cast<int16_t>(mem16(CS, IP));
        IP += 2;
        IP += offset;
        break;
      }
        // jmpf ptr16:16/32
      case 0xEA: {
        const uint16_t new_IP = mem16(CS, IP);
        IP += 2;

        const uint16_t new_CS = mem16(CS, IP);
        IP += 2;

        IP = new_IP;
        CS = new_CS;
        break;
      }
        // jmp e8
      case 0xEB: {
        const int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
        IP += offset;
        break;
      }
        // loopnz eCX
      case 0xE0: {
        const int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
        if (((--CX) != 0) && !ZF()) {
          IP += offset;
        }
        break;
      }
        // loopz eCX
      case 0xE1: {
        const int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
        if (((--CX) != 0) && ZF()) {
          IP += offset;
        }
        break;
      }
        // loop eCX
      case 0xE2: {
        const int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
        if ((--CX) != 0) {
          IP += offset;
        }
        break;
      }
        // loop eCX
      case 0xE3: {
        const int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
        if (CX == 0) {
          IP += offset;
        }
        break;
      }
        // callf ptr16:16/32
      case 0x9A: {
        const uint16_t new_IP = mem16(CS, IP);
        IP += 2;

        const uint16_t new_CS = mem16(CS, IP);
        IP += 2;

        SP -= 2;
        mem16(SS, SP) = CS;
        SP -= 2;
        mem16(SS, SP) = IP;

        IP = new_IP;
        CS = new_CS;
        break;
      }
        // RET
        // retf imm16
      case 0xCA: {
        const uint16_t val = mem16(CS, IP);
        IP += 2;

        IP = mem16(SS, SP);
        SP += 2;
        CS = mem16(SS, SP);
        SP += 2;

        SP += val;
        break;
      }
        // retf
      case 0xCB: {
        IP = mem16(SS, SP);
        SP += 2;
        CS = mem16(SS, SP);
        SP += 2;

        break;
      }
        // DAA
      case 0x27: {
        DAA();
        break;
      }
        // DAS
      case 0x2F: {
        DAS();
        break;
      }
        // AAA
      case 0x37: {
        AAA();
        break;
      }
        // AAS
      case 0x3F: {
        AAS();
        break;
      }
        // NOP
      case 0x90: {
        break;
      }
        // CMP
        // cmp r/m8  r8      (0x38)
        // cmp r/m16 r16     (0x39)
      case 0x38:
      case 0x39: {
        const uint8_t mod_rm = mem8(CS, IP++);
        const bool is_16bit = (opcode == 0x39);
        cmp_rm_reg(mod_rm, is_16bit ? 16 : 8);
        break;
      }
        // cmp r8  r/m8     (0x3A)
        // cmp r16 r/m16    (0x3B)
      case 0x3A:
      case 0x3B: {
        const uint8_t mod_rm = mem8(CS, IP++);
        const bool is_16bit = (opcode == 0x3B);
        cmp_reg_rm(mod_rm, is_16bit ? 16 : 8);
        break;
      }
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
        /*
         * 80   /6   XOR   r/m8   imm8
         * 80   /0   ADD   r/m8   imm8
         * 80   /1   OR    r/m8   imm8
         * 80   /7   CMP   r/m8   imm8
         * 80   /2   ADC   r/m8   imm8
         * 80   /3   SBB   r/m8   imm8
         * 80   /4   AND   r/m8   imm8
         * 80   /5   SUB   r/m8   imm8
         */

        /*
         * 81   /6   XOR   r/m16   imm16
         * 81   /0   ADD   r/m16   imm16
         * 81   /1   OR    r/m16   imm16
         * 81   /7   CMP   r/m16   imm16
         * 81   /2   ADC   r/m16   imm16
         * 81   /3   SBB   r/m16   imm16
         * 81   /4   AND   r/m16   imm16
         * 81   /5   SUB   r/m16   imm16
         */
      case 0x80:
      case 0x82:  // There is historical reason, but in 99% percent, 0x82 is
                  // same as 0x80. 0x80 uses a sign-extended immediate   (but
                  // both uses 8 bits imm8 and 8 bit register, so no need to
                  // extend) 0x82 uses a zero - extended immediate
      case 0x81: {
        const uint8_t mod_rm = mem8(CS, IP++);
        const bool is_16bit = (opcode == 0x81);
        instr_80_81_82(mod_rm, (is_16bit) ? 16 : 8);

        break;
      }
      case 0x83: /* Sign extended */ {
        const uint8_t mod_rm = mem8(CS, IP++);
        instr_83(mod_rm);
        break;
      }
        // CLC
      case 0xF8: {
        SetCF(0);
        break;
      }
        // STC
      case 0xF9: {
        SetCF(1);
        break;
      }
        // CLD
      case 0xFC: {
        SetDF(0);
        break;
      }
        // STD
      case 0xFD: {
        SetDF(1);
        break;
      }
      default:
        mylog("Unsupported opcode '%.02X'", static_cast<int>(opcode));
        return;
    }
  }
}

uint8_t& CPU8068::mem8(const uint16_t CS, const uint16_t IP) {
  return memory[(CS * SEGMENT_SIZE) + IP];
}

uint16_t& CPU8068::mem16(const uint16_t CS, const uint16_t IP) {
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
      const uint8_t* string = &mem8(DS, DX);

      constexpr int32_t MAX_STRING_LENGTH = SEGMENT_SIZE;
      int32_t len = 0;
      while (string[len++] != '$') {
        if (len > MAX_STRING_LENGTH) {
          mylog("String too long, no printing");
          return;
        }
      }

      while (*string != '$') {
        if (isprint(*string) || *string == '\t' || *string == '\r' ||
            *string == '\n' || *string == '\a') {
          std::cout << static_cast<char>(*string++);
        } else if (*string == '\b') {
          string++;
          // Go back one, add space ' ', go back once, emulates deleting one
          // character
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
