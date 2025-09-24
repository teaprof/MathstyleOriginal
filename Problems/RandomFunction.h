#ifndef RANDOMFUNCTION_H
#define RANDOMFUNCTION_H

#include <random>

#include "Base/arithmetic.h"

class TRandomFunction {
  public:
    const static int AllowTrigFunction = 1;
    const static int AllowFracFunction = 2;
    const static int AllowSum = 4;
    const static int AllowLnFunction = 8;
    const static int AllowExpFunction = 16;
    const static int AllowInverseTrigFunction = 32;
    static TNumeric RandomFunction(int Flags, int Level, std::mt19937& rng);
};

#endif  // RANDOMFUNCTION_H
