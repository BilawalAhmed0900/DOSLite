//
// Created by Bilawal Ahmed on 17/Jun/2025.
//

#ifndef PROGRAMEXITEDEXCEPTION_H
#define PROGRAMEXITEDEXCEPTION_H
#include <exception>

class ProgramExitedException : public std::exception {
 public:
  explicit ProgramExitedException(int code);
  virtual ~ProgramExitedException();

 public:
  int code;
};

#endif  // PROGRAMEXITEDEXCEPTION_H
