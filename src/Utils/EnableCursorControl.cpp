//
// Created by Bilawal Ahmed on 17/Jun/2025.
//

#include "EnableCursorControl.h"

#ifdef _WIN32
#include <Windows.h>
#include <consoleapi.h>
#include <handleapi.h>
#include <processenv.h>
#endif

EnableCursorControl::EnableCursorControl()
#ifdef _WIN32
    : oldMode(0)
#endif
{
#ifdef _WIN32
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE) return;

  DWORD dwMode = 0;
  if (!GetConsoleMode(hOut, &dwMode)) return;

  oldMode = dwMode;
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(hOut, dwMode);
#endif
}

EnableCursorControl::~EnableCursorControl() {
#ifdef _WIN32
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE) return;

  SetConsoleMode(hOut, oldMode);
#endif
}
