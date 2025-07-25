#pragma once

#include <cstdint>

#pragma pack(push, 1)
class CPU386 {
 public:
  CPU386();
  void Reset();

 private:
  union {
    uint32_t EIP;
    struct {
      uint16_t IP;
      uint16_t ____IP_UNUSED;
    };
  };

  union {
    uint32_t EAX;
    struct {
      union {
        uint16_t AX;
        struct {
          uint8_t AL;
          uint8_t AH;
        };
      };
      uint16_t ____AX_UNUSED;
    };
  };

  union {
    uint32_t ECX;
    struct {
      union {
        uint16_t CX;
        struct {
          uint8_t CL;
          uint8_t CH;
        };
      };
      uint16_t ____CX_UNUSED;
    };
  };

  union {
    uint32_t EDX;
    struct {
      union {
        uint16_t DX;
        struct {
          uint8_t DL;
          uint8_t DH;
        };
      };
      uint16_t ____DX_UNUSED;
    };
  };

  union {
    uint32_t EBX;
    struct {
      union {
        uint16_t BX;
        struct {
          uint8_t BL;
          uint8_t BH;
        };
      };
      uint16_t ____BX_UNUSED;
    };
  };

  union {
    uint32_t ESP;
    struct {
      uint16_t SP;
      uint16_t ____SP_UNUSED;
    };
  };

  union {
    uint32_t EBP;
    struct {
      uint16_t BP;
      uint16_t ____BP_UNUSED;
    };
  };

  union {
    uint32_t ESI;
    struct {
      uint16_t SI;
      uint16_t ____SI_UNUSED;
    };
  };

  union {
    uint32_t EDI;
    struct {
      uint16_t DI;
      uint16_t ____DI_UNUSED;
    };
  };

  union {
    uint32_t EFLAGS;
    struct {
      union {
        uint16_t FLAGS;
        struct {
          uint16_t CF : 1;
          uint16_t FLAGS_R1 : 1;  // reserved, always 1
          uint16_t PF : 1;
          uint16_t FLAGS_R2 : 1;  // reserved, always 0
          uint16_t AF : 1;
          uint16_t FLAGS_R3 : 1;  // reserved, always 0
          uint16_t ZF : 1;
          uint16_t SF : 1;
          uint16_t TF : 1;
          uint16_t IF : 1;
          uint16_t DF : 1;
          uint16_t OF : 1;
          uint16_t IOPL : 2;
          uint16_t NT : 1;
          uint16_t FLAGS_R4 : 1;  // reserved, always 0
        };
      };

      union {
        uint16_t ____FLAGS_UNUSED;
        struct {
          uint16_t RF : 1;
          uint16_t VM : 1;
          uint16_t AC : 1;
          uint16_t VIF : 1;
          uint16_t VIP : 1;
          uint16_t ID : 1;
          uint16_t FLAGS_R5 : 10;  // reserved, always 0
        };
      };
    };
  };

  union {
    uint32_t CR0;
    struct {
      uint32_t PE : 1;
      uint32_t MP : 1;
      uint32_t EM : 1;
      uint32_t TS : 1;
      uint32_t ET : 1;
      uint32_t NE : 1;
      uint32_t CR0_R1 : 10;
      uint32_t WP : 1;
      uint32_t CR0_R2 : 1;
      uint32_t AM : 1;
      uint32_t CR0_R3 : 10;
      uint32_t NW : 1;
      uint32_t CD : 1;
      uint32_t PG : 1;
    };
  };

  struct {
    uint32_t base;
    uint16_t limit;
  } gdtr;

  static constexpr size_t REG_COUNT = 8;
  uint32_t *reg32[REG_COUNT] = {&EAX, &ECX, &EDX, &EBX, &ESP, &EBP, &ESI, &EDI};
  uint16_t *reg16[REG_COUNT] = {&AX, &CX, &DX, &BX, &SP, &BP, &SI, &DI};
  uint8_t *reg8[REG_COUNT] = {&AL, &CL, &DL, &BL, &AH, &CH, &DH, &BH};

  uint16_t CS, SS, DS, ES;
  uint16_t FS, GS;
};
#pragma pack(pop)
