//
// Created by Bilawal Ahmed on 18/May/2025.
//

#ifndef CPU8068_H
#define CPU8068_H

#include <cstdint>
#include <vector>

#include "CPUMode.h"

class LoadToCPU;

#pragma pack(push, 1)
class CPU8068 {
 public:
  CPU8068(CPU_MODE cpu_mode);
  void reset_registers();
  void execute();

  uint8_t& mem8(uint16_t CS, uint16_t IP);
  uint16_t& mem16(uint16_t CS, uint16_t IP);
  uint16_t sign_extend(uint8_t val);
  bool is_AF(uint16_t lhs, uint16_t rhs, uint32_t result);
  uint32_t ROL(uint32_t val, uint8_t width, uint8_t count,
               uint8_t& last_bit_rotated);
  uint32_t ROR(uint32_t val, uint8_t width, uint8_t count,
               uint8_t& last_bit_rotated);
  uint32_t RCL(uint32_t val, uint8_t width, uint8_t count,
               uint8_t& last_bit_rotated);
  uint32_t RCR(uint32_t val, uint8_t width, uint8_t count,
               uint8_t& last_bit_rotated);
  uint32_t SHL(uint32_t val, uint8_t width, uint8_t count,
               uint8_t& last_bit_rotated);
  uint32_t SHR(uint32_t val, uint8_t width, uint8_t count,
               uint8_t& last_bit_rotated);
  uint32_t SAR(uint32_t val, uint8_t width, uint8_t count,
               uint8_t& last_bit_rotated);

  // For now, not implementing the interrupt table
  void interrupt(uint8_t num);
  void dos_interrupt();

  void adjust_flags(uint32_t result, uint8_t width);

  void set_flags_add(uint16_t lhs, uint16_t rhs, uint32_t result,
                     uint8_t width);
  void set_flags_sub(uint16_t lhs, uint16_t rhs, uint32_t result,
                     uint8_t width);
  void set_flags_logical(uint32_t result, uint8_t width);

  void add_rm_reg(uint8_t mod_rm, uint8_t width);
  void adc_rm_reg(uint8_t mod_rm, uint8_t width);
  void sub_rm_reg(uint8_t mod_rm, uint8_t width);
  void sbb_rm_reg(uint8_t mod_rm, uint8_t width);
  void or_rm_reg(uint8_t mod_rm, uint8_t width);
  void and_rm_reg(uint8_t mod_rm, uint8_t width);
  void xor_rm_reg(uint8_t mod_rm, uint8_t width);

  void add_reg_rm(uint8_t mod_rm, uint8_t width);
  void adc_reg_rm(uint8_t mod_rm, uint8_t width);
  void sub_reg_rm(uint8_t mod_rm, uint8_t width);
  void sbb_reg_rm(uint8_t mod_rm, uint8_t width);
  void or_reg_rm(uint8_t mod_rm, uint8_t width);
  void and_reg_rm(uint8_t mod_rm, uint8_t width);
  void xor_reg_rm(uint8_t mod_rm, uint8_t width);

  void lea_reg_rm(uint8_t mod_rm);

  void pop_rm(uint8_t mod_rm);

  void test_rm_reg(uint8_t mod_rm, uint8_t width);

  void xchg_reg_rm(uint8_t mod_rm, uint8_t width);

  void mov_rm_reg(uint8_t mod_rm, uint8_t width);
  void cmp_rm_reg(uint8_t mod_rm, uint8_t width);
  void mov_rm_imm(uint8_t mod_rm, uint8_t width);
  void mov_reg_rm(uint8_t mod_rm, uint8_t width);
  void cmp_reg_rm(uint8_t mod_rm, uint8_t width);
  void mov_rm_sreg(uint8_t mod_rm, uint8_t width);
  void mov_sreg_rm(uint8_t mod_rm, uint8_t width);

  // string operations
  void mov_es_di_ds_si(uint8_t width);
  void lods_ds_si(uint8_t width);
  void stos_es_di(uint8_t width);
  void cmps_es_di_ds_si(uint8_t width);
  void scas_es_di(uint8_t width);

  void instr_80_81_82(uint8_t mod_rm, uint8_t width);
  void instr_83(uint8_t mod_rm);
  void instr_d0_d1_d2_d3_c0_c1(uint8_t mod_rm, uint8_t width, uint8_t count);
  void instr_fe(uint8_t mod_rm);
  void instr_ff(uint8_t mod_rm);

  void les_lds(uint8_t mod_rm, bool is_lds);

  bool get_address_mode_rm(uint8_t mode, uint8_t r_m, uint16_t& segment,
                           uint16_t& address);

  void update_segment_register(uint16_t reg);

  void DAA();
  void DAS();
  void AAA();
  void AAS();
  void AAM(uint8_t base);
  void AAD(uint8_t base);

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

  static constexpr uint16_t CF_MASK = 1 << 0;
  static constexpr uint16_t PF_MASK = 1 << 2;
  static constexpr uint16_t AF_MASK = 1 << 4;
  static constexpr uint16_t ZF_MASK = 1 << 6;
  static constexpr uint16_t SF_MASK = 1 << 7;
  static constexpr uint16_t IF_MASK = 1 << 9;
  static constexpr uint16_t DF_MASK = 1 << 10;
  static constexpr uint16_t OF_MASK = 1 << 11;
  uint16_t FLAGS;

  [[nodiscard]] uint8_t CF() const;
  void SetCF(uint8_t val);
  [[nodiscard]] uint8_t PF() const;
  void SetPF(uint8_t val);
  [[nodiscard]] uint8_t AF() const;
  void SetAF(uint8_t val);
  [[nodiscard]] uint8_t ZF() const;
  void SetZF(uint8_t val);
  [[nodiscard]] uint8_t SF() const;
  void SetSF(uint8_t val);
  [[nodiscard]] uint8_t DF() const;
  void SetDF(uint8_t val);
  [[nodiscard]] uint8_t OF() const;
  void SetOF(uint8_t val);
  void SetIF(uint8_t val);

  /*
   *  All instruction are in sequence following this register sequence
   */
  constexpr static size_t REGISTER_COUNT = 8;
  uint8_t* reg8[REGISTER_COUNT] = {&AL, &CL, &DL, &BL, &AH, &CH, &DH, &BH};
  uint16_t* reg16[REGISTER_COUNT] = {&AX, &CX, &DX, &BX, &SP, &BP, &SI, &DI};

  constexpr static size_t MEMORY_SIZE = 1 * 1024 * 1024;
  constexpr static size_t SEGMENT_MULTIPLIER = 16; // << 4
  constexpr static size_t SEGMENT_SIZE = 64 * 1024;
  std::vector<uint8_t> memory;
  CPU_MODE cpu_mode;

  /*
    Dumb implementation for 'Interrupt boundary delay'

    2 will be set when we pop SS
    1 on the next instruction
    0 after that
  */
  mutable uint8_t interrupt_delay;
};
#pragma pack(pop)

#endif  // CPU8068_H
