//
// Created by Bilawal Ahmed on 17/Jun/2025.
//

#ifndef LOADTOCPU_H
#define LOADTOCPU_H

#include "../CPU/CPU8068.h"
#include "../ExecutableFiles/COM.h"

class LoadToCPU {
 public:
  static void load(CPU8068& cpu, const COM& com);
};

#endif  // LOADTOCPU_H
