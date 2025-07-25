﻿# CMakeList.txt : CMake project for x8086, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("x8086")

# Add source to this project's executable.
add_executable (x8086
        src/x8086.cpp
        src/CPU/CPU8068.cpp
        src/CPU/CPU8068.h
        src/CPU/CPUMode.h
        src/CPU/funcs/mov.cpp
        src/CPU/funcs/cmp.cpp
        src/CPU/funcs/flags.cpp
        src/CPU/funcs/arithematics.cpp
        src/CPU/funcs/utils.cpp
        src/CPU/funcs/multi_func_instrs.cpp
        src/CPU/funcs/add.cpp
        src/CPU/funcs/sub.cpp
        src/CPU/funcs/test.cpp
        src/CPU/funcs/xchg.cpp
        src/CPU/funcs/pop.cpp
        src/CPU/funcs/adc.cpp
        src/CPU/funcs/sbb.cpp
        src/CPU/funcs/or.cpp
        src/CPU/funcs/and.cpp
        src/CPU/funcs/xor.cpp
        src/CPU/funcs/lea.cpp
        src/CPU/funcs/string_operations.cpp
        src/CPU/funcs/les_lds.cpp
        src/ExecutableFiles/MZExe.cpp
        src/ExecutableFiles/MZExe.h
        src/Utils/logger.h
        src/ExecutableFiles/COM.cpp
        src/ExecutableFiles/COM.h
        src/Utils/LoadToCpu.cpp
        src/Utils/LoadToCpu.h
        src/Exceptions/ProgramExitedException.cpp
        src/Exceptions/ProgramExitedException.h
        src/Utils/EnableCursorControl.cpp
        src/Utils/EnableCursorControl.h)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET x8086 PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
