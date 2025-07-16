//
// Created by Bilawal Ahmed on 18/May/2025.
//

#include "CPU8068.h"

#include <cctype>
#include <cstdint>
#include <iostream>

#include "../Exceptions/ProgramExitedException.h"
#include "../Utils/logger.h"
#include "CPUMode.h"

CPU8068::CPU8068(const CPU_MODE cpu_mode)
    : memory(MEMORY_SIZE, 0), cpu_mode(cpu_mode) {
  AX = BX = CX = DX = 0;
  SP = BP = SI = DI = 0;
  CS = DS = SS = ES = 0;
  IP = 0;
  FLAGS = 0;
  interrupt_delay = 0;
}

void CPU8068::reset_registers() {
  AX = BX = CX = DX = 0;
  SP = BP = SI = DI = 0;
  interrupt_delay = 0;
}

void CPU8068::execute() {
  while (true) {
    const uint8_t opcode = mem8(CS, IP++);
    if (interrupt_delay) --interrupt_delay;
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
        const uint16_t result =
            static_cast<uint16_t>(AL) + static_cast<uint16_t>(rhs);
        set_flags_add(AL, rhs, result, 8);
        AL = result & 0xFF;
        break;
      }
        // add eAX imm16/32
      case 0x05: {
        const uint16_t rhs = mem16(CS, IP);
        IP += 2;

        const uint32_t result =
            static_cast<uint32_t>(AX) + static_cast<uint32_t>(rhs);
        set_flags_add(AX, rhs, result, 16);
        AX = result & 0xFFFF;
        break;
      }
        // ADC
        // ADC  r/m8  r8
        // ADC  r/m16 r16
      case 0x10:
      case 0x11: {
        const uint8_t mod_rm = mem8(CS, IP++);

        const bool is_16bit = (opcode == 0x11);
        adc_rm_reg(mod_rm, (is_16bit) ? 16 : 8);
        break;
      }
        // ADC  r8  r/m8
        // ADC  r16 r/m16
      case 0x12:
      case 0x13: {
        const uint8_t mod_rm = mem8(CS, IP++);

        const bool is_16bit = (opcode == 0x13);
        adc_reg_rm(mod_rm, (is_16bit) ? 16 : 8);
        break;
      }
        // adc AL imm8
      case 0x14: {
        const uint8_t rhs = mem8(CS, IP++);
        const uint16_t result = static_cast<uint16_t>(AL) +
                                static_cast<uint16_t>(rhs) +
                                static_cast<uint16_t>(CF());
        set_flags_add(AL, rhs, result, 8);
        AL = result & 0xFF;
        break;
      }
        // adc eAX imm16/32
      case 0x15: {
        const uint16_t rhs = mem16(CS, IP);
        IP += 2;

        const uint32_t result = static_cast<uint32_t>(AX) +
                                static_cast<uint32_t>(rhs) +
                                static_cast<uint32_t>(CF());
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
        const uint16_t result =
            static_cast<uint16_t>(AL) - static_cast<uint16_t>(rhs);
        set_flags_sub(AL, rhs, result, 8);
        AL = result & 0xFF;
        break;
      }
        // sub eAX imm16/32
      case 0x2D: {
        const uint16_t rhs = mem16(CS, IP);
        IP += 2;

        const uint32_t result =
            static_cast<uint32_t>(AX) - static_cast<uint32_t>(rhs);
        set_flags_sub(AX, rhs, result, 16);
        AX = result & 0xFFFF;
        break;
      }
        // SBB
        // SBB  r/m8  r8
        // SBB  r/m16 r16
      case 0x18:
      case 0x19: {
        const uint8_t mod_rm = mem8(CS, IP++);

        const bool is_16bit = (opcode == 0x19);
        sbb_rm_reg(mod_rm, (is_16bit) ? 16 : 8);
        break;
      }
        // SBB  r8  r/m8
        // SBB  r16 r/m16
      case 0x1A:
      case 0x1B: {
        const uint8_t mod_rm = mem8(CS, IP++);

        const bool is_16bit = (opcode == 0x1B);
        sbb_reg_rm(mod_rm, (is_16bit) ? 16 : 8);
        break;
      }
        // sbb AL imm8
      case 0x1C: {
        const uint8_t rhs = mem8(CS, IP++);
        const uint16_t result = static_cast<uint16_t>(AL) -
                                static_cast<uint16_t>(rhs) -
                                static_cast<uint16_t>(CF());
        set_flags_sub(AL, rhs, result, 8);
        AL = result & 0xFF;
        break;
      }
        // sbb eAX imm16/32
      case 0x1D: {
        const uint16_t rhs = mem16(CS, IP);
        IP += 2;

        const uint32_t result = static_cast<uint32_t>(AX) -
                                static_cast<uint32_t>(rhs) -
                                static_cast<uint32_t>(CF());
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

        const uint32_t result = static_cast<uint32_t>(*reg16[opcode - 0x40]) +
                                static_cast<uint32_t>(1);
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

        const uint32_t result = static_cast<uint32_t>(*reg16[opcode - 0x48]) -
                                static_cast<uint32_t>(1);
        set_flags_sub(*reg16[opcode - 0x48], 1, result, 16);
        *reg16[opcode - 0x48] = result;

        SetCF(oldCF);
        break;
      }
        // JMP
        // JO e8
      case 0x70: {
        const int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
        if (OF()) {
          IP += offset;
        }
        break;
      }
        // JNO e8
      case 0x71: {
        const int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
        if (!OF()) {
          IP += offset;
        }
        break;
      }
        // JB e8
      case 0x72: {
        const int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
        if (CF()) {
          IP += offset;
        }
        break;
      }
        // JNB e8
      case 0x73: {
        const int8_t offset = static_cast<int8_t>(mem8(CS, IP++));
        if (!CF()) {
          IP += offset;
        }
        break;
      }
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
        const uint16_t result =
            static_cast<uint16_t>(AL) - static_cast<uint16_t>(rhs);
        set_flags_sub(AL, rhs, result, 8);
        break;
      }
        // cmp eAX imm16/32
      case 0x3D: {
        const uint16_t rhs = mem16(CS, IP);
        IP += 2;

        const uint32_t result =
            static_cast<uint32_t>(AX) - static_cast<uint32_t>(rhs);
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
        // TEST
        // test r/m8  r8      (0x84)
        // test r/m16 r16     (0x85)
      case 0x84:
      case 0x85: {
        const uint8_t mod_rm = mem8(CS, IP++);
        const bool is_16bit = (opcode == 0x85);
        test_rm_reg(mod_rm, (is_16bit) ? 16 : 8);

        break;
      }
        // XCHG
        // xchg r8   r/m8     (0x86)
        // xchg r16  r/m16    (0x87)
      case 0x86:
      case 0x87: {
        const uint8_t mod_rm = mem8(CS, IP++);
        const bool is_16bit = (opcode == 0x87);
        xchg_reg_rm(mod_rm, (is_16bit) ? 16 : 8);

        break;
      }
        // NOP
        // XCHG AX, AX
      case 0x90: {
        break;
      }
        // 0x90+r XCHG AX, r16
      case 0x91:
      case 0x92:
      case 0x93:
      case 0x94:
      case 0x95:
      case 0x96:
      case 0x97: {
        const uint16_t temp = *reg16[opcode - 0x90];
        uint16_t& rhs = *reg16[opcode - 0x90];
        rhs = AX;
        AX = temp;
        break;
      }
        // SHIFTS
        // D0 0   ROL r/m8    1
        // D0 1   ROR r/m8    1
        // D0 2   RCL r/m8    1
        // D0 3   RCR r/m8    1
        // D0 4   SHL r/m8    1
        // D0 5   SHR r/m8    1
        // D0 6   SAL r/m8    1
        // D0 7   SAR r/m8    1
        //
        // D1 0   ROL r/m16   1
        // D1 1   ROR r/m16   1
        // D1 2   RCL r/m16   1
        // D1 3   RCR r/m16   1
        // D1 4   SHL r/m16   1
        // D1 5   SHR r/m16   1
        // D1 6   SAL r/m16   1
        // D1 7   SAR r/m16   1
      case 0xD0:
      case 0xD1: {
        const uint8_t mod_rm = mem8(CS, IP++);
        const bool is_16bit = (opcode == 0xD1);
        instr_d0_d1_d2_d3_c0_c1(mod_rm, (is_16bit) ? 16 : 8, 1);

        break;
      }
        // D2 0   ROL r/m8    CL
        // D2 1   ROR r/m8    CL
        // D2 2   RCL r/m8    CL
        // D2 3   RCR r/m8    CL
        // D2 4   SHL r/m8    CL
        // D2 5   SHR r/m8    CL
        // D2 6   SAL r/m8    CL
        // D2 7   SAR r/m8    CL
        //
        // D3 0   ROL r/m16   CL
        // D3 1   ROR r/m16   CL
        // D3 2   RCL r/m16   CL
        // D3 3   RCR r/m16   CL
        // D3 4   SHL r/m16   CL
        // D3 5   SHR r/m16   CL
        // D3 6   SAL r/m16   CL
        // D3 7   SAR r/m16   CL
      case 0xD2:
      case 0xD3: {
        const uint8_t mod_rm = mem8(CS, IP++);
        const bool is_16bit = (opcode == 0xD3);
        instr_d0_d1_d2_d3_c0_c1(mod_rm, (is_16bit) ? 16 : 8, CL);

        break;
      }

        // C0 0   ROL r/m8    imm8
        // C0 1   ROR r/m8    imm8
        // C0 2   RCL r/m8    imm8
        // C0 3   RCR r/m8    imm8
        // C0 4   SHL r/m8    imm8
        // C0 5   SHR r/m8    imm8
        // C0 6   SAL r/m8    imm8
        // C0 7   SAR r/m8    imm8
        //
        // C1 0   ROL r/m16   imm8
        // C1 1   ROR r/m16   imm8
        // C1 2   RCL r/m16   imm8
        // C1 3   RCR r/m16   imm8
        // C1 4   SHL r/m16   imm8
        // C1 5   SHR r/m16   imm8
        // C1 6   SAL r/m16   imm8
        // C1 7   SAR r/m16   imm8
      case 0xC0:
      case 0xC1: {
        const uint8_t mod_rm = mem8(CS, IP++);
        const uint8_t times = mem8(CS, IP++);
        const bool is_16bit = (opcode == 0xC1);
        instr_d0_d1_d2_d3_c0_c1(mod_rm, (is_16bit) ? 16 : 8, times);

        break;
      }
        // PUSH 50+r
      case 0x50:
      case 0x51:
      case 0x52:
      case 0x53:
      case 0x55:
      case 0x56:
      case 0x57: {
        SP -= 2;
        mem16(SS, SP) = *reg16[opcode - 0x50];
        break;
      }
        // Special SP case
      case 0x54: {
        if (cpu_mode == CPU_MODE::CPU_8086 || cpu_mode == CPU_MODE::CPU_80186) {
          SP -= 2;
          mem16(SS, SP) = SP;
        } else {
          const uint16_t origSP = SP;
          SP -= 2;
          mem16(SS, SP) = origSP;
        }

        break;
      }
        // PUSH ES
      case 0x06: {
        SP -= 2;
        mem16(SS, SP) = ES;
        break;
      }
        // PUSH SS
      case 0x16: {
        SP -= 2;
        mem16(SS, SP) = SS;
        break;
      }
        // PUSH DS
      case 0x1E: {
        SP -= 2;
        mem16(SS, SP) = DS;
        break;
      }
        // PUSH imm16
      case 0x68: {
        const uint16_t val = mem16(CS, IP);
        IP += 2;

        SP -= 2;
        mem16(SS, SP) = val;
        break;
      }
        // PUSH imm8 (sign extend)
      case 0x6A: {
        const uint16_t val = sign_extend(mem8(CS, IP++));
        SP -= 2;
        mem16(SS, SP) = val;
        break;
      }
        // POP 58+r
      case 0x58:
      case 0x59:
      case 0x5A:
      case 0x5B:
      case 0x5C:
      case 0x5D:
      case 0x5E:
      case 0x5F: {
        *reg16[opcode - 0x58] = mem16(SS, SP);
        SP += 2;
        break;
      }
        // POP ES
      case 0x07: {
        ES = mem16(SS, SP);
        SP += 2;
        update_segment_register(ES);
        break;
      }
        // POP SS
      case 0x17: {
        SS = mem16(SS, SP);
        SP += 2;
        interrupt_delay = 2;
        update_segment_register(SS);
        break;
      }
        // POP DS
      case 0x1F: {
        DS = mem16(SS, SP);
        SP += 2;
        update_segment_register(DS);
        break;
      }
        // FE 0   INC r/m8
        // FE 1   DEC r/m8
      case 0xFE: {
        const uint8_t mod_rm = mem8(CS, IP++);
        instr_fe(mod_rm);
        break;
      }
        // FF 0   INC    r/m16
        // FF 1   DEC    r/m16
        // FF 2   CALL   r/m16
        // FF 3   CALLF  r/m16
        // FF 4   JUMP   r/m16
        // FF 5   JUMPF  r/m16
        // FF 6   PUSH   r/m16
      case 0xFF: {
        const uint8_t mod_rm = mem8(CS, IP++);
        instr_ff(mod_rm);
        break;
      }
        // OR
        // OR  r/m8  r8
        // OR  r/m16 r16
      case 0x08:
      case 0x09: {
        const uint8_t mod_rm = mem8(CS, IP++);

        const bool is_16bit = (opcode == 0x09);
        or_rm_reg(mod_rm, (is_16bit) ? 16 : 8);
        break;
      }
        // OR  r8  r/m8
        // OR  r16 r/m16
      case 0x0A:
      case 0x0B: {
        const uint8_t mod_rm = mem8(CS, IP++);

        const bool is_16bit = (opcode == 0x0B);
        or_reg_rm(mod_rm, (is_16bit) ? 16 : 8);
        break;
      }
        // OR AL imm8
      case 0x0C: {
        const uint8_t rhs = mem8(CS, IP++);
        const uint16_t result =
            static_cast<uint16_t>(AL) | static_cast<uint16_t>(rhs);
        set_flags_logical(result, 8);
        AL = result & 0xFF;
        break;
      }
        // OR eAX imm16/32
      case 0x0D: {
        const uint16_t rhs = mem16(CS, IP);
        IP += 2;

        const uint32_t result =
            static_cast<uint32_t>(AX) | static_cast<uint32_t>(rhs);
        set_flags_logical(result, 16);
        AX = result & 0xFFFF;
        break;
      }
        // AND
        // AND  r/m8  r8
        // AND  r/m16 r16
      case 0x20:
      case 0x21: {
        const uint8_t mod_rm = mem8(CS, IP++);

        const bool is_16bit = (opcode == 0x21);
        and_rm_reg(mod_rm, (is_16bit) ? 16 : 8);
        break;
      }
        // AND  r8  r/m8
        // AND  r16 r/m16
      case 0x22:
      case 0x23: {
        const uint8_t mod_rm = mem8(CS, IP++);

        const bool is_16bit = (opcode == 0x23);
        and_reg_rm(mod_rm, (is_16bit) ? 16 : 8);
        break;
      }
        // AND AL imm8
      case 0x24: {
        const uint8_t rhs = mem8(CS, IP++);
        const uint16_t result =
            static_cast<uint16_t>(AL) & static_cast<uint16_t>(rhs);
        set_flags_logical(result, 8);
        AL = result & 0xFF;
        break;
      }
        // AND eAX imm16/32
      case 0x25: {
        const uint16_t rhs = mem16(CS, IP);
        IP += 2;

        const uint32_t result =
            static_cast<uint32_t>(AX) & static_cast<uint32_t>(rhs);
        set_flags_logical(result, 16);
        AX = result & 0xFFFF;
        break;
      }
        // XOR
        // XOR  r/m8  r8
        // XOR  r/m16 r16
      case 0x30:
      case 0x31: {
        const uint8_t mod_rm = mem8(CS, IP++);

        const bool is_16bit = (opcode == 0x31);
        xor_rm_reg(mod_rm, (is_16bit) ? 16 : 8);
        break;
      }
        // XOR  r8  r/m8
        // XOR  r16 r/m16
      case 0x32:
      case 0x33: {
        const uint8_t mod_rm = mem8(CS, IP++);

        const bool is_16bit = (opcode == 0x33);
        xor_reg_rm(mod_rm, (is_16bit) ? 16 : 8);
        break;
      }
        // XOR AL imm8
      case 0x34: {
        const uint8_t rhs = mem8(CS, IP++);
        const uint16_t result =
            static_cast<uint16_t>(AL) ^ static_cast<uint16_t>(rhs);
        set_flags_logical(result, 8);
        AL = result & 0xFF;
        break;
      }
        // XOR eAX imm16/32
      case 0x35: {
        const uint16_t rhs = mem16(CS, IP);
        IP += 2;

        const uint32_t result =
            static_cast<uint32_t>(AX) ^ static_cast<uint32_t>(rhs);
        set_flags_logical(result, 16);
        AX = result & 0xFFFF;
        break;
      }
        // HLT
      case 0xF4: {
        /*
          Will be revised later
        */
        throw ProgramExitedException{0};
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
        // CLI
      case 0xFA: {
        SetIF(0);
        break;
      }
        // STI
      case 0xFB: {
        SetIF(1);
        interrupt_delay = 2;
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
