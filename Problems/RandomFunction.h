#ifndef RANDOMFUNCTION_H
#define RANDOMFUNCTION_H

#include "Base/arithmetic.h"
#include "tproblem.h" //only for TRandom

class TRandomFunction
{
public:
    TRandom *Rng;
    TRandomFunction(TRandom *Rng);
    const static int AllowTrigFunction = 1;
    const static int AllowFracFunction = 2;
    const static int AllowSum = 4;
    const static int AllowLnFunction = 8;
    const static int AllowExpFunction = 16;
    const static int AllowInverseTrigFunction = 32;
    TNumeric RandomFunction(int Flags, int Level = 0);
};

#endif // RANDOMFUNCTION_H
