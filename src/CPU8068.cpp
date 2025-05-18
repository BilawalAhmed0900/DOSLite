//
// Created by Bilawal Ahmed on 18/May/2025.
//

#include "CPU8068.h"

CPU8068::CPU8068(): memory(MEMORY_SIZE, 0) {
    AX = BX = CX = DX = 0;
    SP = BP = SI = DI = 0;
    CS = DS = SS = ES = 0;
    IP = 0;
    FLAGS = 0;
}
