#include "RandomFunction.h"

TNumeric TRandomFunction::RandomFunction(int Flags, int Level, std::mt19937& rng) {
    if (Level == 0) {
        return TNumeric("x");  // Only const is allowed
    }
    vector<Operation> Operators;
    if (Flags & AllowTrigFunction) {
        Operators.push_back(OperatorSin);
        Operators.push_back(OperatorCos);
        Operators.push_back(OperatorTg);
        Operators.push_back(OperatorCtg);
    };
    if (Flags & AllowInverseTrigFunction) {
        Operators.push_back(OperatorArcsin);
        Operators.push_back(OperatorArccos);
        Operators.push_back(OperatorArctg);
    };
    if (Flags & AllowLnFunction) Operators.push_back(OperatorLn);
    if (Flags & AllowExpFunction) Operators.push_back(OperatorExp);
    if (Flags & AllowFracFunction) Operators.push_back(OperatorFrac);
    if (Flags & AllowSum) Operators.push_back(OperatorSum);
    if (Operators.size() == 0) {
        return TNumeric("x");  // Only const is allowed
    }
    size_t Index = rng() % Operators.size();
    Operation operation = Operators[Index];
    TNumeric Res;
    Res.operation = operation;
    switch (operation) {
        case OperatorSin:
        case OperatorCos:
        case OperatorTg:
        case OperatorCtg:
        case OperatorArcsin:
        case OperatorArccos:
        case OperatorArctg:
        case OperatorLn:
        case OperatorExp:
            Res.OperandsPushback(RandomFunction(Flags, Level - 1, rng));
            break;
        case OperatorFrac:
            Res.OperandsPushback(RandomFunction(Flags, Level - 1, rng));
            Res.OperandsPushback(RandomFunction(Flags, Level - 1, rng));
            break;
        case OperatorSum:
            Res.OperandsPushback(RandomFunction(Flags & ~AllowSum, Level, rng));  // we are excluding AllowSum to avoid
            Res.OperandsPushback(RandomFunction(
                Flags & ~AllowSum, Level, rng));  // infinite recursion (with probability = 0) and long expressions (prob > 0)
            break;
    }
    return Res;
}
