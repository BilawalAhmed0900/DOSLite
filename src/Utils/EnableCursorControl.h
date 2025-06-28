//
// Created by Bilawal Ahmed on 17/Jun/2025.
//

#ifndef ENABLECURSORCONTROL_H
#define ENABLECURSORCONTROL_H

#ifdef _WIN32
#include <Windows.h>
#endif

class EnableCursorControl {
 public:
  EnableCursorControl();
  ~EnableCursorControl();

 private:
#ifdef _WIN32
  DWORD oldMode;
#endif
};

#endif  // ENABLECURSORCONTROL_H
