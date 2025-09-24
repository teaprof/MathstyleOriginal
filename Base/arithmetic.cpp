//---------------------------------------------------------------------------
#include "arithmetic.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <string>

#include "algebra_operations.h"
#include "mathomatic.h"
#include "symbols.h"

int GetOperatorPriority(Operation OpCode) {
    switch (OpCode) {
        case OperatorInline:
            return 10;
        case OperatorLess:
        case OperatorLessOrEqual:
        case OperatorGreater:
        case OperatorGreaterOrEqual:
        case OperatorBelongsTo:
        case OperatorEqual:
            return 0;
        case OperatorInterval:
        case OperatorIntervalSegment:
        case OperatorSegmentInterval:
        case OperatorSegment:
        case OperatorLog:
        case OperatorLn:
        case OperatorLg:
        case OperatorSin:
        case OperatorCos:
        case OperatorTg:
        case OperatorArcsin:
        case OperatorArccos:
        case OperatorArctg:
        case OperatorExp:
        case OperatorSh:
        case OperatorCh:
        case OperatorCtg:
        case OperatorSmallO:
        case OperatorBigO:
            return 6;
        case OperatorDeriv:
            return 0;
        case OperatorUnion:
        case OperatorSum:
            return 1;
        case OperatorPlusMinus:
        case OperatorMinus:
            return 2;
        case OperatorConst:
            return 2;  // если константа - положительное число, то скобки никогда не ставятся. Если со знаком минус - то скобки
                       // ставятся тогда, когда ставились бы скобки у выражения 0-a
        case OperatorIntersection:
        case OperatorIntegral:
        case OperatorProd:
            return 3;
        case OperatorFrac:
            return 3;
        case OperatorSqrt:
            return 4;
        case OperatorSubIndex:
        case OperatorSupIndex:
        case OperatorSign:
        case OperatorAbs:
        case OperatorPow:
            return 5;
        default:
            throw "TNumeric::GetOperatorPriority:: function is not realized for this type of OpCode";
    };
}

int CompareOperatorsPriority(Operation OpCode1, Operation OpCode2) {
    int prior1 = GetOperatorPriority(OpCode1);
    int prior2 = GetOperatorPriority(OpCode2);
    if (prior1 < prior2) return -1;
    if (prior1 == prior2) return 0;
    return 1;
};


TNumeric TNumeric::deepCopy() const {
    TNumeric res;
    res.strval = strval;
    res.operation = operation;
    res.role = role;
    for (size_t n = 0; n < operands.size(); n++) {
        res.operands.push_back(operands[n]->deepCopyPtr());
    }
    return res;
}
std::shared_ptr<TNumeric> TNumeric::deepCopyPtr() const {
    auto res = TNumeric::create();
    res->strval = strval;
    res->operation = operation;
    res->role = role;
    for (size_t n = 0; n < operands.size(); n++) {
        res->operands.push_back(operands[n]->deepCopyPtr());
    }
    return res;
}
TNumeric::TNumeric(double value) {
    if (value == 0) {
        strval = "0";
    } else {
        operation = OperatorConst;
        char p[256];
        sprintf(p, "%g", value);  /// \todo: in this case the value should be stored in the internal variable of type double
        strval = p;
    };
};
TNumeric::TNumeric(int value) {
    if (value == 0) {
        strval = "0";
    } else {
        operation = OperatorConst;
        char p[256];
        sprintf(p, "%d", value);
        strval = p;
    };
}

TNumeric::TNumeric(string value) {
    if (value == "-inf")
        strval = "-\\infty";
    else if (value == "inf")
        strval = "\\infty";
    else if (value == "+inf")
        strval = "+\\infty";
    else
        strval = value;
}

void TNumeric::OperandsPushback(const TNumeric& Val) {
    this->operands.push_back(std::make_shared<TNumeric>(Val));
}

void TNumeric::OperandsPushback(TNumeric&& Val) {
    this->operands.push_back(std::make_shared<TNumeric>(std::move(Val)));
}

void TNumeric::OperandsPushback(std::shared_ptr<TNumeric> val) {
    this->operands.push_back(val);
}

void TNumeric::OperandsClear() {
    this->operands.clear();
}

void TNumeric::MakeEqSet(const TNumeric& N1, const TNumeric& N2) {
    OperandsPushback(N1);
    OperandsPushback(N2);
    operation = OperatorEqSet;
};
void TNumeric::MakeEqSystem(const TNumeric& N1, const TNumeric& N2) {
    OperandsPushback(N1);
    OperandsPushback(N2);
    operation = OperatorEqSystem;
};

void TNumeric::CheckInitialized() const {
    if (operation == OperatorConst && strval.empty()) throw "TNumeric::CheckInitialized: object is not initialized yet";
}

bool TNumeric::hasRole(int role) const {
    if (role == this->role) {
        return true;
    }
    for (size_t i = 0; i < operands.size(); i++) {
        if (operands[i]->hasRole(role))
            return true;
    }
    return false;
}

std::shared_ptr<const TNumeric> TNumeric::GetByRole(int role) const {
    if (role == this->role) {
        return shared_from_this();  // todo: enable shared from this
    }
    for (size_t i = 0; i < operands.size(); i++) {
        if (operands[i]->hasRole(role)) {
            return operands[i]->GetByRole(role);
        }
    };
    return nullptr;
}

bool TNumeric::isInteger(int* Int) const {
    const TNumeric& Simplified = Simplify();
    if (Simplified.operation == OperatorConst) {
        double d;
        if (sscanf(Simplified.strval.c_str(), "%lf", &d) == 1) {
            if (d == static_cast<int>(d)) {
                if (Int) 
                    *Int = static_cast<int>(d);
                return true;
            };
        }
    }
    return false;
}
bool TNumeric::isInteger() const {
    return isInteger(nullptr);
}

bool TNumeric::IsVariable() const {
    if (operation == OperatorConst) {
        // first letter should be non-nomeric and non-minus
        if (strval.length() == 0) throw "TNumeric::IsVariable(): strval.length = 0";
        char p = strval[0];
        if (p >= '0' && p <= '9') return false;
        if (p == '-') return false;
        return true;
    };
    if (operation == OperatorSubIndex) {
        return operands[0]->IsVariable() == true;
    }
    return false;
}

bool TNumeric::IsEqual(const TNumeric& N) const {
    if (operation != N.operation) return false;
    if (operation == OperatorConst) return strval == N.strval;
    if (operands.size() != N.operands.size()) return false;
    for (size_t i = 0; i < operands.size(); i++)
        if (operands[i]->IsEqual(*N.operands[i]) == false) return false;
    return true;
}

bool TNumeric::DependsOn(const char* variable) const {
    if (operation == OperatorConst && strval == variable) return true;
    for (size_t i = 0; i < operands.size(); i++)
        if (operands[i]->DependsOn(variable)) return true;
    return false;
}
bool TNumeric::operator==(const TNumeric& N) const {
    /*    if(operation != N.operation) return false;
    if(operation == OperatorConst)
    {
        return strval == N.strval;
    };
    if(Operands.size() != N.Operands.size()) return false;
    for(size_t i = 0; i < Operands.size(); i++)
        if(Operands[i] != N.Operands[i]) return false;
    return true;*/
    if (operation == OperatorEqual) {
        if (operands.size() != N.operands.size()) return false;
        for (size_t i = 0; i < operands.size(); i++)
            if (operands[i] != N.operands[i]) return false;
        return true;
    } else {
        const TNumeric& Diff = *this - N;
        const TNumeric& simplified = Diff.Simplify();
        if (simplified.operation == OperatorConst && simplified.strval == "0") 
            return true;
        return false;
    };
}
bool TNumeric::operator!=(const TNumeric& N) const {
    return !operator==(N);
}
bool TNumeric::operator==(double Value) const {
    const TNumeric& Temp = Simplify();
    if (Temp.CanCalculate())
        return Temp.Calculate() == Value;
    else
        return false;
}
bool TNumeric::operator!=(double Value) const {
    return !operator==(Value);
}

TNumeric TNumeric::operator*(const TNumeric& N) const {
    N.CheckInitialized();
    /*    if(operation == OperatorProd)
    {
    //В этом случае просто добавляем в уже существующий список множителей ещё один множитель
    //2do: перенести преобразование (a*b)*c => a*b*c (два произведения в одно) в функцию Simplify
        TNumeric Result = *this;
        Result.OperandsPushback(N);
        return Result;
   } else {*/
    // структуру выражения менять не вправе, оставляем всё, как есть
    TNumeric Result;
    Result.operation = OperatorProd;
    Result.OperandsPushback(*this);
    Result.OperandsPushback(N);
    return Result;
    //};
};
TNumeric TNumeric::operator-() const {
    CheckInitialized();
    TNumeric Res;
    Res.operation = OperatorProd;
    Res.OperandsPushback(TNumeric(-1));
    Res.OperandsPushback(*this);
    return Res;
};
TNumeric TNumeric::operator+(const TNumeric& N) const {
    N.CheckInitialized();
    /*    if(operation == OperatorSum)
    {
        //В этом случае просто добавляем в уже существующий список множителей ещё один множитель
        //2do: перенести преобразование (a+b)+c => a+b+c (две суммы в одну) в функцию Simplify
        TNumeric Result = *this;
        Result.OperandsPushback(N);
        return Result;
    } else {*/
    TNumeric Result;
    Result.operation = OperatorSum;
    Result.OperandsPushback(*this);
    Result.OperandsPushback(N);
    return Result;
    //};
};

TNumeric TNumeric::operator-(const TNumeric& N) const {
    N.CheckInitialized();
    TNumeric Result;
    Result.operation = OperatorMinus;
    Result.OperandsPushback(*this);
    Result.OperandsPushback(N);
    return Result;
};

TNumeric TNumeric::operator/(const TNumeric& N) const {
    N.CheckInitialized();
    TNumeric Result;
    Result.operation = OperatorFrac;
    Result.OperandsPushback(*this);
    Result.OperandsPushback(N);
    return Result;
};
TNumeric TNumeric::operator^(const TNumeric& N) const {
    N.CheckInitialized();
    TNumeric Result;
    Result.operation = OperatorPow;
    Result.OperandsPushback(*this);
    Result.OperandsPushback(N);
    return Result;
};

TNumeric TNumeric::sqrt() const {
    CheckInitialized();
    TNumeric Result;
    Result.operation = OperatorSqrt;
    Result.OperandsPushback(*this);
    return Result;
};

TNumeric TNumeric::Derivative(const string VarName) const {
    switch (this->operation) {
        case OperatorLog: {
            TNumeric Nom = MakeLn(*operands[0]);
            TNumeric Denom = MakeLn(*operands[1]);
            return (Nom / Denom).Derivative(VarName);
        }
        case OperatorLn:
            return operands[0]->Derivative(VarName) / (*operands[0]);
        case OperatorLg:
            return operands[0]->Derivative(VarName) / (MakeLn(TNumeric(10)) * (*operands[0]));
        case OperatorSin:
            return MakeCos(*operands[0]) * operands[0]->Derivative(VarName);
        case OperatorCos:
            return -MakeSin(*operands[0]) * operands[0]->Derivative(VarName);
        case OperatorTg:
            return TNumeric(1) / MakePow(MakeCos(*operands[0]), TNumeric(2)) * operands[0]->Derivative(VarName);
        case OperatorArcsin:
            return TNumeric(1) / MakeSqrt(TNumeric(1) - MakePow(*operands[0], TNumeric(2))) * operands[0]->Derivative(VarName);
        case OperatorArccos:
            return -TNumeric(1) / MakeSqrt(TNumeric(1) - MakePow(*operands[0], TNumeric(2))) * operands[0]->Derivative(VarName);
        case OperatorArctg:
            return TNumeric(1) / (TNumeric(1) + MakePow(*operands[0], TNumeric(2))) * operands[0]->Derivative(VarName);
        case OperatorCtg:
            return -TNumeric(1) / MakePow(MakeSin(*operands[0]), TNumeric(2)) * operands[0]->Derivative(VarName);
        case OperatorAbs:
            return MakeSign(*operands[0]) * operands[0]->Derivative(VarName);
        case OperatorSign:
            return TNumeric("0");
        case OperatorSum: {
            TNumeric Res("0");
            Res.operation = OperatorSum;
            for (size_t i = 0; i < operands.size(); i++) {
                Res.OperandsPushback(operands[i]->Derivative(VarName));
            }
            return Res;
        }
        case OperatorPlusMinus: {
            TNumeric Res("0");
            Res.operation = OperatorPlusMinus;
            for (size_t i = 0; i < operands.size(); i++) {
                Res.OperandsPushback(operands[i]->Derivative(VarName));
            }
            return Res;
        }
        case OperatorMinus: {
            TNumeric Res("0");
            Res.operation = OperatorMinus;
            for (size_t i = 0; i < operands.size(); i++) {
                Res.OperandsPushback(operands[i]->Derivative(VarName));
            }
            return Res;
        }
        case OperatorConst:
            if (strval != VarName)
                return TNumeric("0");
            else
                return TNumeric("1");
        case OperatorProd: {
            TNumeric Res("0");
            Res.operation = OperatorSum;
            for (size_t i = 0; i < operands.size(); i++) {
                TNumeric Term;
                for (size_t j = 0; j < operands.size(); j++) {
                    TNumeric Multiplier;
                    if (i == j)
                        Multiplier = operands[j]->Derivative(VarName);
                    else
                        Multiplier = *operands[j];
                    if (j == 0)
                        Term = Multiplier;
                    else
                        Term = Term * Multiplier;
                }
                Res.OperandsPushback(Term);
            }
            return Res;
        }
        case OperatorFrac:
            return (operands[0]->Derivative(VarName) * (*operands[1]) - (*operands[0]) * operands[1]->Derivative(VarName)) /
                   MakePow(*operands[1], TNumeric(2));
        case OperatorSqrt:
            return TNumeric(1) / (TNumeric(2) * MakeSqrt(*operands[0])) * operands[0]->Derivative(VarName);
        case OperatorExp:
            return MakeExp(*operands[0]) * operands[0]->Derivative(VarName);
        case OperatorSh:
            return MakeCh(*operands[0]) * operands[0]->Derivative(VarName);
        case OperatorCh:
            return MakeSh(*operands[0]) * operands[0]->Derivative(VarName);
        case OperatorPow:
            if (operands[1]->DependsOn(VarName.c_str()) == false) {
                return (*operands[1]) * ((*operands[0]) ^ ((*operands[1]) - TNumeric(1))) * operands[0]->Derivative(VarName);
            };
            return MakeExp((*operands[1]) * MakeLn(*operands[0])).Derivative(VarName);
        case OperatorIntegral:
        case OperatorDeriv:
        default:
            throw "TNumeric::Derivative:: function is not realized for this type of OpCode";
    };
}

bool TNumeric::CanCalculate() const {
    try {
        if (isnan(Calculate())) return false;
        return true;
    } catch (...) {
        return false;
    }
}

double TNumeric::Calculate() const {
    size_t i;
    double A;
    vector<double> Vals;
    Vals.assign(operands.size(), 0);
    for (size_t i = 0; i < operands.size(); i++) {
        Vals[i] = operands[i]->Calculate();
        if (isnan(Vals[i])) return NaN;
    };
    switch (operation) {
        case OperatorConst:
            if (strval.length() != 0) {
                if (strval == "-\\infty") return -INFINITY;
                if (strval == "\\infty" || strval == "+\\infty") return INFINITY;
                if (strval == "\\pi") return M_PI;
                try {
                    double d;
                    if (sscanf(strval.c_str(), "%lf", &d) == 0) return NaN;
                    A = d;
                } catch (...) {
                    return NaN;
                };
                return A;
            } else
                return NaN;
            break;
        case OperatorEqual:
            return Vals[1];
            break;
        case OperatorSum:
            A = 0;
            for (i = 0; i < operands.size(); i++)
                A += Vals[i];
            return A;
            break;
        case OperatorMinus:
            A = Vals[0];
            for (i = 1; i < operands.size(); i++)
                A -= Vals[i];
            return A;
            break;
        case OperatorProd:
            A = 1;
            for (i = 0; i < operands.size(); i++)
                A *= Vals[i];
            return A;
            break;
        case OperatorFrac:
            if (Vals[1] == 0) {
                if (Vals[0] >= 0)
                    return Infinity;
                else
                    return -Infinity;
            }
            return Vals[0] / Vals[1];
            break;
        case OperatorSqrt:
            if (Vals[0] >= 0)
                return sqrtl(Vals[0]);
            else
                return NaN;
            break;
        case OperatorPow:
            if (Vals[1] == 0) return 1;
            if (Vals[0] > 0)
                return exp(Vals[1] * log(Vals[0]));
            else {
                if (ceil(Vals[1]) == Vals[1])  // показатель целый
                {
                    // BInt нужно только для определения знака
                    int BInt = ceil(Vals[1]);
                    if (BInt < 0) BInt = -BInt;
                    int sign;
                    if (BInt % 2 == 0)
                        sign = 1;
                    else
                        sign = -1;
                    return exp(Vals[1] * log(-Vals[0])) * sign;
                }
                return NaN;
            };
            break;
        case OperatorExp:
            if (Vals[0] == 0) return 1;
            return exp(Vals[0]);
            break;
        case OperatorSh:
            if (Vals[0] == 0) return 0;
            return sinh(Vals[0]);
            break;
        case OperatorCh:
            if (Vals[0] == 0) return 1;
            return cosh(Vals[0]);
            break;
        case OperatorLog:
            if (Vals[0] <= 0 || Vals[1] <= 0) return NaN;
            return log(Vals[0]) / log(Vals[1]);
        case OperatorLn:
            if (Vals[0] <= 0) return NaN;
            return log(Vals[0]);
        case OperatorLg:
            if (Vals[0] <= 0) return NaN;
            return log(Vals[0]) / log(10.0L);
        case OperatorSin:
            return sin(Vals[0]);
        case OperatorCos:
            return cos(Vals[0]);
        case OperatorTg:
            if (cos(Vals[0]) == 0) {
                if (sin(Vals[0]) > 0)
                    return +Infinity;
                else
                    return -Infinity;
            }
            return sin(Vals[0]) / cos(Vals[0]);
        case OperatorCtg:
            if (sin(Vals[0]) == 0) {
                if (cos(Vals[0]) > 0)
                    return +Infinity;
                else
                    return -Infinity;
            }
            return cos(Vals[0]) / sin(Vals[0]);
        case OperatorArcsin:
            if (fabs(Vals[0]) > 1)
                return NaN;
            else
                return asin(Vals[0]);
        case OperatorArccos:
            if (fabs(Vals[0]) > 1)
                return NaN;
            else
                return acos(Vals[0]);
        case OperatorArctg:
            return atan(Vals[0]);
        case OperatorSign:
            if (Vals[0] > 0)
                return 1;
            else if (Vals[0] < 0)
                return -1;
            else
                return 0;
        case OperatorAbs:
            return fabs(Vals[0]);
        case OperatorLess:
        case OperatorLessOrEqual:
        case OperatorGreater:
        case OperatorGreaterOrEqual:
        case OperatorEqSet:
        case OperatorEqSystem:
        case OperatorInterval:
        case OperatorIntervalSegment:
        case OperatorSegmentInterval:
        case OperatorSegment:
        case OperatorInline:
        case OperatorUnion:
        case OperatorIntersection:
        case OperatorSubIndex:
        case OperatorSupIndex:
        case OperatorDeriv:
        case OperatorPlusMinus:
        case OperatorIntegral:
        default:
            throw "TNumeric::Calculate(): Can't calculate the EqSet, EqSystem or lines operation etc";
            break;
    };
    return NaN;
};

TNumeric TNumeric::Substitute(const string& var, const TNumeric& Val) const {
    if (operation == OperatorConst) {
        if (strval == var)
            return Val;
        else
            return *this;
    };
    TNumeric Res;
    Res.operation = operation;
    for (size_t i = 0; i < operands.size(); i++)
        Res.OperandsPushback(operands[i]->Substitute(var, Val));
    return Res;
}

string TNumeric::CodeBasic() const {
    string Str;
    size_t i;
    switch (operation) {
        case OperatorConst:
            if (strval == "-\\infty")
                Str = "(-inf)";
            else if (strval == "\\infty" || strval == "+\\infty")
                Str = "(inf)";
            else
                Str = strval;
            break;
        case OperatorEqual:
            Str = operands[0]->CodeBasic() + "=" + operands[1]->CodeBasic();
            break;
        case OperatorSum:
            Str = '(';
            for (i = 0; i < operands.size(); i++) {
                Str += operands[i]->CodeBasic();
                if (i + 1 < operands.size()) Str += '+';
            };
            Str += ')';
            return Str;
            break;
        case OperatorMinus:
            Str = string("(") + operands[0]->CodeBasic() + "-";
            for (i = 1; i < operands.size(); i++) {
                Str += operands[i]->CodeBasic();
                if (i + 1 < operands.size()) Str += '-';
            };
            Str += ')';
            return Str;
            break;
        case OperatorProd:
            Str = '(';
            for (i = 0; i < operands.size(); i++) {
                Str += operands[i]->CodeBasic();
                if (i + 1 < operands.size()) Str += '*';
            };
            Str += ')';
            return Str;
            break;
        case OperatorFrac:
            Str = string("(") + operands[0]->CodeBasic() + ")/(" + operands[1]->CodeBasic() + ")";
            break;
        case OperatorSqrt:
            Str = string("((") + operands[0]->CodeBasic() + ")^(1/2))";
            break;
        case OperatorPow:
            Str = string("((") + operands[0]->CodeBasic() + ")^(" + operands[1]->CodeBasic() + "))";
            break;
        case OperatorSubIndex:
            Str = operands[0]->CodeBasic() + "_" + operands[1]->CodeBasic();
            break;
        case OperatorIntegral:
            Str = string("integral(") + operands[0]->CodeBasic() + ") d" + operands[1]->CodeBasic();
            break;
        case OperatorExp:
            Str = string("exp(") + operands[0]->CodeBasic() + ")";
            break;
        case OperatorSh:
            Str = string("sh(") + operands[0]->CodeBasic() + ")";
            break;
        case OperatorCh:
            Str = string("ch(") + operands[0]->CodeBasic() + ")";
            break;
        case OperatorCos:
            Str = string("cos(") + operands[0]->CodeBasic() + ")";
            break;
        case OperatorSin:
            Str = string("sin(") + operands[0]->CodeBasic() + ")";
            break;
        case OperatorTg:
            Str = string("tg(") + operands[0]->CodeBasic() + ")";
            break;
        case OperatorArccos:
            Str = string("acos(") + operands[0]->CodeBasic() + ")";
            break;
        case OperatorArcsin:
            Str = string("asin(") + operands[0]->CodeBasic() + ")";
            break;
        case OperatorArctg:
            Str = string("atg(") + operands[0]->CodeBasic() + ")";
            break;
        case OperatorCtg:
            Str = string("ctg(") + operands[0]->CodeBasic() + ")";
            break;
        case OperatorLg:
            Str = string("lg(") + operands[0]->CodeBasic() + ")";
            break;
        case OperatorLn:
            Str = string("ln(") + operands[0]->CodeBasic() + ")";
            break;
        case OperatorLog:
            Str = string("log(") + operands[0]->CodeBasic() + ", " + operands[1]->CodeBasic() + ")";
            break;
        default:
            Str = string("f(");
            for (size_t i = 0; i < operands.size(); i++)
                if (i + 1 < operands.size())
                    Str = Str + operands[0]->CodeBasic() + ", ";
                else
                    Str = Str + operands[0]->CodeBasic() + ")";
            // throw "TNumeric::CodeBasic(): function not fully realized yet";
    };
    return Str;
};
//==============================================================================
string DeleteExternalBrackets(string Str) {
    // Удаляем внешние скобки
    char* p = (char*)Str.c_str();
    int BracketLevel = 0;
    bool External = true;  // true, если найденная пара скобок StartIndex EndIndex является внешними скобками - за пределами их
                           // нет арифеметических операций
    size_t StartIndex, EndIndex;  // начало и конец результирующей подстроки, не включая последений символ
    // то есть Result = [StartIndex, EndIndex)
    StartIndex = 0;
    EndIndex = Str.length();
    while (*p) {
        if (*p == '(') {
            if (BracketLevel == 0) StartIndex = p - Str.c_str() + 1;
            BracketLevel++;
        };
        if (*p == ')') {
            if (BracketLevel == 1) EndIndex = p - Str.c_str();
            BracketLevel--;
        };
        if (*p != '(' && *p != ')' && *p != ' ')
        // найдено или цифра, или буква или арифметическое действие
        {
            if (BracketLevel == 0)
                // найдено за пределами самых внешних скобок
                External = false;  // значит, эти скобки уже не внешние, их удалять не будем
        }
        p++;
    };
    if (StartIndex == 1 && External)
        return Str.substr(StartIndex, EndIndex - StartIndex);
    else
        return Str;
};
//==============================================================================
Operation DecodeFunctionName(const std::string_view& str) {
    if (str == "exp") return OperatorExp;
    if (str == "sh") return OperatorSh;
    if (str == "ch") return OperatorCh;
    if (str == "log") return OperatorLog;
    if (str == "ln") return OperatorLn;
    if (str == "lg") return OperatorLg;
    if (str == "sin") return OperatorSin;
    if (str == "cos") return OperatorCos;
    if (str == "tg") return OperatorTg;
    if (str == "ctg") return OperatorCtg;
    if (str == "asin") return OperatorSin;
    if (str == "acos") return OperatorArccos;
    if (str == "atg") return OperatorArctg;
    if (str == "cos") return OperatorCos;
    std::ostringstream errstr;
    errstr << "DecodeFunctionName: Unknown function name: " << string(str);
    throw errstr.str().c_str();
}

bool CheckFunctionTemplate(const char* str, TNumeric* Res)
// string must be like function_name(arg1, arg2, ..., argn)
// returns
{
    const char* leftbracket = 0;
    const char* rightbracket = 0;
    const char* p = str;
    if (*p >= '0' && *p <= '9') return false;
    while (*p && *p != '(') {
        p++;
        if (*p == 0) break;
        if (*p == '+' || *p == '-' || *p == '/' || *p == '*' || *p == '^') return false;
    };
    if (*p == 0) return false;
    leftbracket = p;
    int bracketlevel = 0;
    while (*p) {
        if (*p == '(') bracketlevel++;
        if (*p == ')') bracketlevel--;
        if (*p == '(' && bracketlevel == 1) leftbracket = p;
        if (*p == ')' && bracketlevel == 0) {
            rightbracket = p;
            if (*(p + 1) != 0)  // the last character in a string should be ')'
                return false;
        };
        p++;
    };
    if (bracketlevel != 0) throw "CheckFunctionTemplate: unbalanced brackets";
    string FunctionName(str);
    FunctionName.erase(leftbracket - str);  // начиная от скобки '(' все удаляем
    // Выделяем аргументы
    bracketlevel = 0;
    const char* q = leftbracket + 1;
    Res->operation = DecodeFunctionName(FunctionName.c_str());
    string Str(str);
    for (p = leftbracket + 1; p < rightbracket; p++) {
        if (*p == '(') bracketlevel++;
        if (*p == ')') bracketlevel--;
        if (*p == ',' && bracketlevel == 0) {
            TNumeric Op;
            string S = Str.substr(q - str, p - q);
            Op.Assign(S.c_str());
            q = p + 1;
            Res->OperandsPushback(Op);
        }
    };
    TNumeric Op;
    string S = Str.substr(q - str, p - q);
    Op.Assign(S.c_str());
    Res->OperandsPushback(Op);
    return true;
}

void TNumeric::Assign(const char* str) {
    char* p;
    size_t len = strlen(str);
    p = new char[len + 1];
    strcpy(p, str);
    Assign(p);
    delete[] p;
};

void TNumeric::Assign(char* str) {
    string Str = str;
    Str = DeleteExternalBrackets(Str);
    while (1) {
        size_t pos = Str.find("**");
        if (pos == string::npos) break;
        Str.replace(pos, 2, "^");
    };
    while (1) {
        size_t pos = Str.find(" ");
        if (pos == string::npos) break;
        Str.replace(pos, 1, "");
    };
    TNumeric Res;
    if (CheckFunctionTemplate(Str.c_str(), &Res)) { /// \todo: Res is empty here
        *this = Res;
        assert(false);
        return;
    }

    int OpCode = -1;
    int OpPriority = -1;
    int BracketLevel = -1;
    int OpPos = -1;

    char* p;
    int i = 0;
    int CurBracketLevel = 0;
    p = (char*)Str.c_str();
    while (*p) {
        int CurOpCode = -1;
        if (*p == '(') CurBracketLevel++;
        if (*p == ')') CurBracketLevel--;

        if (*p == '+') CurOpCode = OperatorSum;
        if (*p == '-')
            if (i > 0 && *(p - 1) != '(')  // чтобы исключить случай унарного минуса, когда "-3" преобразуется в "(0)-(3)"="0-3"
                CurOpCode = OperatorMinus;
        if (*p == '/') CurOpCode = OperatorFrac;
        if (*p == '^') CurOpCode = OperatorPow;
        if (*p == '*') CurOpCode = OperatorProd;

        // случай 2^2+2
        int CurOpPrior;
        if (CurOpCode != -1) 
            CurOpPrior = GetOperatorPriority(static_cast<Operation>(CurOpCode));

        if (CurOpCode != -1)
            if (OpCode == -1 ||
                (CurBracketLevel < BracketLevel ||
                 (CurBracketLevel == BracketLevel && (CurOpPrior < OpPriority || (CurOpPrior == OpPriority && i > OpPos)))))
            // Если выполнено хотя бы одно условие
            // 1. OpCode == -1 - это первая найденная операция в выражении
            // 2. CurBracketLevel < BracketLevel - эта операция находится во внешних скобках по отношению к найденной
            // 3. CurBracketLevel == BracketLevel - в тех же скобках, но при этом должно выполнять одно из условий
            // 3a. CurOpPrior < OpPriority - операция имеет меньший приоритет, чем найденная
            // 3b. CurOpPrior = OpPriority - операция имеет тот же приоритет, но при этом нас интересует самая правая операция (i
            // > OpPos)
            //     (Последнее условие, чтобы правильно обрабатывать a-b-c = (a-b) -c != a - (b - c)
            {
                BracketLevel = CurBracketLevel;
                OpPriority = CurOpPrior;
                OpCode = CurOpCode;
                OpPos = i;
            };
        p++;
        i++;
    };
    if (OpCode == -1) {
        if (Str.empty()) {
            TNumeric Res("0");
            *this = Res;
        } else {
            TNumeric Res(Str);
            *this = Res;
        };
    } else {
        string Str1, Str2;
        Str1 = Str.substr(0, OpPos);
        Str2 = Str.substr(OpPos + 1);
        /*#ifdef __DEBUG__
       cout<<Str1.c_str()<<"  "<<Str[OpPos]<<":"<<OpPriority<<"   "<<Str2.c_str()<<endl;
#endif*/
        TNumeric N1, N2, Res;
        N1.Assign((char*)Str1.c_str());
        N2.Assign((char*)Str2.c_str());

        switch (OpCode) {
            case OperatorSum:
                Res = N1 + N2;
                break;
            case OperatorMinus:
                Res = N1 - N2;
                break;
            case OperatorProd:
                Res = N1 * N2;
                break;
            case OperatorPow:
                Res = N1 ^ N2;
                break;
            case OperatorFrac:
                Res = N1 / N2;
                break;
            default:
                break;
        };
        *this = Res;
    };
    SimplifyPresentation();
}
TNumeric TNumeric::SimplifyPresentation() const{
    TNumeric res(*this);
    for (size_t i = 0; i < operands.size(); i++)
        res.operands[i] = TNumeric::create(std::move(operands[i]->SimplifyPresentation()));
    switch (operation) {
        case OperatorConst:
            break;
        case OperatorEqual:
            break;
        case OperatorSum:  //(a+b)+c = a+b+c
        {
            size_t i = 0;
            while (i < res.operands.size()) {
                if (res.operands[i]->operation == OperatorSum) {
                    for (size_t j = 0; j < res.operands[i]->operands.size(); j++) {
                        res.operands.insert(res.operands.begin() + i, res.operands[i]->operands[j]);
                        i++;
                    };
                    res.operands.erase(res.operands.begin() + i);
                } else
                    i++;
            };
        } break;
        case OperatorProd:  // a*(b*c) = a*b*c
        {
            size_t i = 0;
            while (i < res.operands.size()) {
                if (res.operands[i]->operation == OperatorProd) {
                    for (size_t j = 0; j < res.operands[i]->operands.size(); j++) {
                        res.operands.insert(res.operands.begin() + i, res.operands[i]->operands[j]);
                        i++;
                    };
                    res.operands.erase(res.operands.begin() + i);
                } else
                    i++;
            };
        } break;
        case OperatorFrac:
            break;
        case OperatorSqrt:
            break;
        case OperatorPow:
            // 1. упрощаем, если показатель степени равен 1/2
            const TNumeric& N = res.operands[1]->deepCopy();
            if (N.operation == OperatorFrac)
                if (N.operands[0]->strval == "1" && N.operands[1]->strval == "2") {
                    // std::shared_ptr<TNumeric> N2 = myoperands[0];
                    res.operands.erase(res.operands.end());  // удаляем второй (он же последний) элемент
                    res.operation = OperatorSqrt;
                };
            break;
    }
    return res;
}

TNumeric TNumeric::EliminateUnimplementedFunctions(size_t& StartID, map<string, TNumeric>& vars_to_functions) const {
    TNumeric res(*this);
    bool NeedToEliminate = true;
    switch (res.operation) {
        case OperatorConst:
            // проводим переобзначение "\var" -> "an"
            if (!(strval.length() > 0 && strval[0] == '\\'))
                NeedToEliminate = false;
            break;
        // ниже список функций, реализованных в mathomatic, которые можно не упрощать
        case OperatorSum:
        case OperatorMinus:
        case OperatorProd:
        case OperatorFrac:
        case OperatorSqrt:
        case OperatorPow:
        case OperatorEqual:
            NeedToEliminate = false;
            break;
    }
    if (NeedToEliminate) {
        // проверяем, вдруг эта функция уже есть и как-то обозначена
        auto It = vars_to_functions.begin();        
        while (It != vars_to_functions.end()) {
            if (It->second.IsEqual(*this))
            // нашли, используем уже существующее обозначение
            {
                res.operation = OperatorConst;
                res.strval = It->first;
                break;
            }
            It++;
        }
        if (It == vars_to_functions.end()) {
            // не нашли, придумываем новое обозначение
            std::pair<string, TNumeric> NewItem;
            NewItem.second = *this;
            for (size_t i = 0; i < NewItem.second.operands.size(); i++)
                NewItem.second.operands[i] = std::make_shared<TNumeric>(NewItem.second.operands[i]->Simplify());

            char Buf[128];
            sprintf(Buf, "a%d", (int)StartID++);
            res.operation = OperatorConst;            
            res.strval = Buf;
            NewItem.first = Buf;
            vars_to_functions.insert(NewItem);
        }
    }
    // упрощаем аргументы
    for (size_t i = 0; i < operands.size(); i++)
        res.operands[i] = TNumeric::create(std::move(res.operands[i]->EliminateUnimplementedFunctions(StartID, vars_to_functions)));
    return res;
}

TNumeric TNumeric::RestoreUnimplementedFunctions(const std::map<string, TNumeric>& V) const {
    TNumeric res(*this);
    switch (operation) {
        case OperatorConst: {
            std::map<string, TNumeric>::const_iterator it = V.find(strval);
            if (it == V.end())
                break;
            else {
                res = it->second;
            }
        }; 
        break;
        default: {
            for (size_t i = 0; i < operands.size(); i++)
                res.operands[i] = TNumeric::create(std::move(res.operands[i]->RestoreUnimplementedFunctions(V)));
        };
    }
    return res;
}

bool TNumeric::asRational(int& Numerator, int& Denominator) const {
    if (CanCalculate() == false) return false;
    if (operation == OperatorConst) {
        if (isInteger(&Numerator)) {
            Denominator = 1;
            return true;
        }
    };
    if (operation == OperatorFrac) {
        if (operands[0]->isInteger(&Numerator) && operands[1]->isInteger(&Denominator)) {
            return true;
        }
    }
    return false;
}

TNumeric TNumeric::SimplifyTrig() const {
    bool OperatorOk = false;
    switch (operation) {
        case OperatorSin:
        case OperatorCos:
        case OperatorTg:
        case OperatorCtg:
            OperatorOk = true;
    };
    if (!OperatorOk) 
        return *this;

    TNumeric Res(*this);
    Res.operands[0] = TNumeric::create(Res.operands[0]->Simplify());
    // double Val = Operands[0]->Calculate();
    // if(Val == 0)
    if (*Res.operands[0] == TNumeric(0)) {
        switch (operation) {
            case OperatorSin:
                return TNumeric("0");
            case OperatorCos:
                return TNumeric("1");
            case OperatorTg:
                return TNumeric("0");
            case OperatorCtg:
                return MakeCtg(TNumeric("0"));
        };
    } else if (*Res.operands[0] == NumPi) {
        switch (operation) {
            case OperatorSin:
                return TNumeric("0");
            case OperatorCos:
                return TNumeric("-1");
            case OperatorTg:
                return TNumeric("0");
            case OperatorCtg:
                return MakeCtg(NumPi);
        };
    } else if (*Res.operands[0] == NumPi2) {
        switch (operation) {
            case OperatorSin:
                return TNumeric("1");
            case OperatorCos:
                return TNumeric("0");
            case OperatorTg:
                return MakeTg(NumPi2);
            case OperatorCtg:
                return TNumeric("0");
        };
    } else if (*Res.operands[0] == NumPi3) {
        // todo
    } else if (*Res.operands[0] == Num2Pi3) {
        // todo
    };
    // todo Нужно  сделать попытку привести угол к отрезку [0, pi/2], а потом с учетом знака вычислить ответ
    return Res;
}

TNumeric TNumeric::SimplifyInverseTrig() const {
    bool OperatorOk = false;
    switch (operation) {
        case OperatorArcsin:
        case OperatorArccos:
        case OperatorArctg:
            OperatorOk = true;
    };
    if (!OperatorOk) 
        return *this;

    TNumeric Res(*this);
    Res.operands[0] = std::make_shared<TNumeric>(Res.operands[0]->Simplify());
    // double Val = Operands[0]->Calculate();

    if (*Res.operands[0] == TNumeric(0)) {
        switch (operation) {
            case OperatorArcsin:
                return TNumeric("0");
            case OperatorArccos:
                return NumPi2;
            case OperatorArctg:
                return TNumeric("0");
        };
    } else if (*Res.operands[0] == TNumeric(1)) {
        switch (operation) {
            case OperatorArcsin:
                return NumPi2;
            case OperatorArccos:
                return TNumeric("0");
            case OperatorArctg:
                return NumPi4;
        };
    } else if (*Res.operands[0] == TNumeric(-1)) {
        switch (operation) {
            case OperatorArcsin:
                return -NumPi2;
            case OperatorArccos:
                return NumPi;
            case OperatorArctg:
                return -NumPi4;
        };
    } else if (*Res.operands[0] == MakeFrac(TNumeric(1), TNumeric(2))) {
        switch (operation) {
            case OperatorArcsin:
                return NumPi6;
            case OperatorArccos:
                return NumPi3;
            case OperatorArctg:
                return MakeArctg(MakeFrac(TNumeric("1"), TNumeric("2")));
        };
    } else if (*Res.operands[0] == -MakeFrac(TNumeric(1), TNumeric(2))) {
        switch (operation) {
            case OperatorArcsin:
                return -NumPi6;
            case OperatorArccos:
                return Num2Pi3;
            case OperatorArctg:
                return -MakeArctg(MakeFrac(TNumeric("1"), TNumeric("2")));
        };
    } else if (*Res.operands[0] == MakeFrac(MakeSqrt(TNumeric("3")), TNumeric(2))) {
        switch (operation) {
            case OperatorArcsin:
                return NumPi3;
            case OperatorArccos:
                return NumPi6;
            case OperatorArctg:
                return MakeArctg(MakeFrac(MakeSqrt(TNumeric("3")), TNumeric(2)));
        };
    } else if (*Res.operands[0] == -MakeFrac(MakeSqrt(TNumeric("3")), TNumeric(2))) {
        switch (operation) {
            case OperatorArcsin:
                return -NumPi3;
            case OperatorArccos:
                return Num5Pi6;
            case OperatorArctg:
                return -MakeArctg(MakeFrac(MakeSqrt(TNumeric("3")), TNumeric(2)));
        };
    } else if (*Res.operands[0] == MakeFrac(TNumeric(1), MakeSqrt(TNumeric(2)))) {
        switch (operation) {
            case OperatorArcsin:
                return NumPi4;
            case OperatorArccos:
                return NumPi4;
            case OperatorArctg:
                return MakeArctg(MakeFrac(TNumeric("1"), MakeSqrt(TNumeric("2"))));
        };
    } else if (*Res.operands[0] == MakeFrac(TNumeric(1), MakeSqrt(TNumeric(2)))) {
        switch (operation) {
            case OperatorArcsin:
                return -NumPi4;
            case OperatorArccos:
                return Num3Pi4;
            case OperatorArctg:
                return -MakeArctg(MakeFrac(TNumeric("1"), MakeSqrt(TNumeric("2"))));
        };
    };
    return Res;
}

TNumeric TNumeric::SimplifyLog() const {
    if (operation != OperatorLog) return *this;
    TNumeric A = operands[0]->Simplify();
    TNumeric B = operands[1]->Simplify();
    if (A.CanCalculate() && B.CanCalculate()) {
        TNumeric Res;
        double a = A.Calculate();
        double b = B.Calculate();
        if (a < 0 || b < 0 || b == 1) {
            return MakeLog(A, B);
        };
        if (a == b) return TNumeric(1);
        if (a == 1) return TNumeric(0);
        int AN, AD, BN, BD;
        Res.operation = OperatorLog;
        int N, D;  // коэффициент перед логарифмом
        if (A.asRational(AN, AD)) {
            int AN1, AD1;
            if (AN * AD < 0) throw "TNumeric::SimplifyLog: AN*AD<0";
            if (AN < 0) AN = -AN;
            if (AD < 0) AD = -AD;
            CheckCommonPower(AN, AD, N, AN1, AD1);
            if (N > 1) A = (TNumeric(AN1) / TNumeric(AD1)).Simplify();
        } else
            N = 1;
        if (B.asRational(BN, BD)) {
            int BN1, BD1;
            if (BN * BD < 0) throw "TNumeric::SimplifyLog: AN*AD<0";
            if (BN < 0) BN = -BN;
            if (BD < 0) BD = -BD;
            CheckCommonPower(BN, BD, D, BN1, BD1);
            if (D > 1) B = (TNumeric(BN1) / TNumeric(BD1)).Simplify();
        } else
            D = 1;
        Res.OperandsPushback(A);
        Res.OperandsPushback(B);
        if (N == 1 && D != 1) Res = MakeFrac(Res, TNumeric(D));
        if (N != 1 && D == 1) Res = MakeProd(TNumeric(N), std::move(Res));
        if (N != 1 && D != 1) Res = MakeProd(MakeFrac(TNumeric(N), TNumeric(D)), std::move(Res));
        return Res;
    };
    return *this;
}

TNumeric TNumeric::SimplifyFunctions() const {
    TNumeric res;
    switch (operation) {
        case OperatorLog:
            res = SimplifyLog();
            break;
        /*case OperatorLn:
            case OperatorLg:*/
        case OperatorSin:
        case OperatorCos:
        case OperatorTg:
        case OperatorCtg:
            res = SimplifyTrig();
            break;
        case OperatorArcsin:
        case OperatorArccos:
        case OperatorArctg:
            res = SimplifyInverseTrig();
            break;
            /*        default:
                for(size_t i = 0; i < Operands.size(); i++)
                    Operands[i]->SimplifyFunctions();
                break;*/
        case OperatorLn:
        case OperatorLg:
            if (*operands[0] == TNumeric(1)) 
                res = TNumeric(0);
            break;
    }
    for (size_t i = 0; i < operands.size(); i++)
        res.operands[i] = TNumeric::create(std::move(operands[i]->Simplify()));
    return res;
}

TNumeric TNumeric::MathoCmd(const string& Cmd) const {
    // ходим по всему дереву и упрощаем все функции

    // 1.Устраняем все нереализованные в mathomatic функции
    size_t StartID = 0;
    map<string, TNumeric> func_map;
    auto Temp = this->EliminateUnimplementedFunctions(StartID, func_map);
    // 2. вызываем mathomatic
    std::string SourceCode = Temp.CodeBasic();
#ifdef __DEBUG__
    cout << SourceCode << "->";
    cout.flush();
#endif
    static const std::string VarName = "vartosimplify";
    SourceCode = VarName + " = " + SourceCode;
    std::string ResCode;
    ResCode = math_process(SourceCode.c_str());    
    ResCode = math_process(Cmd.c_str());
    math_clear_all();
    // удаляем "vartosimplify = "
    size_t pos = ResCode.find(VarName + " = ");
    if (pos == string::npos) 
        throw "TNumeric::Simplify: Unknown answer format from math library. Can't simplify";
    ResCode = ResCode.substr(pos + VarName.length() + 3);
#ifdef __DEBUG__
    cout << ResCode << endl;
#endif
    auto res = std::make_shared<TNumeric>(ResCode);
    // 3. восстанавливаем функции
    res->RestoreUnimplementedFunctions(func_map);
    return Temp;
}

TNumeric TNumeric::Simplify() const {
    /// \todo: if *this is not changed, do not create new object and return this
    TNumeric Res(*this);
    if (operation == OperatorConst) 
        return Res;
    Res = Res.SimplifyFunctions();
    if (operation == OperatorEqual) {
        const TNumeric& A = operands[0]->MathoCmd("simplify");
        const TNumeric& B = operands[1]->MathoCmd("simplify");
        return MakeEquality(A,B);
    };
    Res = Res.MathoCmd("simplify");
    return Res;
}

void TNumeric::LoadFromFile(ifstream& f) {
    assert(false);
}

void TNumeric::SaveToFile(ofstream& f) {
    assert(false);
}

TNumeric TNumeric::Unfactor() const {
    return MathoCmd("unfactor");
}

TNumeric MakeUnion(const TNumeric& N1, const TNumeric& N2) {
    TNumeric Res;
    Res.operation = OperatorUnion;
    Res.OperandsPushback(N1);
    Res.OperandsPushback(N2);
    return Res;
}
TNumeric MakeSubscript(const TNumeric& N1, const TNumeric& N2) {
    TNumeric Res;
    Res.operation = OperatorSubIndex;
    Res.OperandsPushback(N1);
    Res.OperandsPushback(N2);
    return Res;
}

TNumeric MakeSuperscript(const TNumeric& N1, const TNumeric& N2) {
    TNumeric Res;
    Res.operation = OperatorSupIndex;
    Res.OperandsPushback(N1);
    Res.OperandsPushback(N2);
    return Res;
}

TNumeric MakeFrac(const TNumeric& N1, const TNumeric& N2) {
    return N1 / N2;
}
//****************************************************************************
TNumeric GetPolynom(size_t Power, size_t StartID)
// конструирует многочлен с нулевыми коэффициентами. role устанавливаются от StartID до StartID+Power
{
    TNumeric Res;
    for (size_t i = 0; i <= Power; i++) {
        TNumeric a(0);
        a.role = i + StartID;
        // a.EditableFlags = ConstAllowed;

        /*        char Buf[10]; //может быть полезно, присваивает role значению коэфициента
        sprintf(Buf, "%d", a.role);
        a.strval = Buf;*/
        if (i == 0) {
            Res = a;
        } else {
            TNumeric Temp;
            if (i == 1)
                Temp = a * TNumeric("x");
            else
                Temp = a * (TNumeric("x") ^ TNumeric(static_cast<int>(i)));
            Res = Temp + Res;
        }
    }
    return Res;
}
//****************************************************************************

TNumeric MakeFunction(const Operation operation, const TNumeric& N) {
    TNumeric Res;
    Res.operation = operation;
    Res.OperandsPushback(N);
    return Res;
}
TNumeric MakeFunction(const Operation operation, const TNumeric& N1, const TNumeric& N2) {
    TNumeric Res;
    Res.operation = operation;
    Res.OperandsPushback(N1);
    Res.OperandsPushback(N2);
    return Res;
}
TNumeric MakeFunction(const Operation operation, TNumeric&& N1, TNumeric&& N2) {
    TNumeric Res;
    Res.operation = operation;
    Res.OperandsPushback(std::move(N1));
    Res.OperandsPushback(std::move(N2));
    return Res;
}
TNumeric MakeProd(TNumeric&& N1, TNumeric&& N2) {
    return MakeFunction(OperatorProd, std::move(N1), std::move(N2));
}

TNumeric MakeLn(const TNumeric& N) {
    return MakeFunction(OperatorLn, N);
};

TNumeric MakeLog(const TNumeric& N, const TNumeric& Base) {
    return MakeFunction(OperatorLog, N, Base);
};

TNumeric MakeCos(const TNumeric& N) {
    return MakeFunction(OperatorCos, N);
}

TNumeric MakeSin(const TNumeric& N) {
    return MakeFunction(OperatorSin, N);
}

TNumeric MakeTg(const TNumeric& N) {
    return MakeFunction(OperatorTg, N);
}

TNumeric MakeEquality(const TNumeric& N1, const TNumeric& N2) {
    return MakeFunction(OperatorEqual, N1, N2);
};

TNumeric MakeBelongsTo(const TNumeric& N1, const TNumeric& N2) {
    return MakeFunction(OperatorBelongsTo, N1, N2);
};

TNumeric MakeInterval(const TNumeric& X1, const TNumeric& X2, bool includeleft, bool includeright) {
    TNumeric Res;
    if (includeleft && includeright) Res.operation = OperatorSegment;
    if (!includeleft && includeright) Res.operation = OperatorIntervalSegment;
    if (includeleft && !includeright) Res.operation = OperatorSegmentInterval;
    if (!includeleft && !includeright) Res.operation = OperatorInterval;
    Res.OperandsPushback(X1);
    Res.OperandsPushback(X2);
    return Res;
}
TNumeric MakeInline(const TNumeric& N1, const TNumeric& N2) {
    return MakeFunction(OperatorInline, N1, N2);
}

TNumeric MakeInline(const TNumeric& N1, const TNumeric& N2, const TNumeric& N3) {
    TNumeric Res;
    Res.operation = OperatorInline;
    Res.OperandsPushback(N1);
    Res.OperandsPushback(N2);
    Res.OperandsPushback(N3);
    return Res;
}

TNumeric MakeArccos(const TNumeric& N) {
    return MakeFunction(OperatorArccos, N);
}

TNumeric MakeArcsin(const TNumeric& N) {
    return MakeFunction(OperatorArcsin, N);
}

TNumeric MakeArctg(const TNumeric& N) {
    return MakeFunction(OperatorArctg, N);
}

TNumeric MakePow(const TNumeric& N1, const TNumeric& N2) {
    return MakeFunction(OperatorPow, N1, N2);
}

TNumeric MakeExp(const TNumeric& N) {
    return MakeFunction(OperatorExp, N);
}
TNumeric MakeSh(const TNumeric& N) {
    return MakeFunction(OperatorSh, N);
}
TNumeric MakeCh(const TNumeric& N) {
    return MakeFunction(OperatorCh, N);
}

TNumeric MakeSqrt(const TNumeric& N) {
    return MakeFunction(OperatorSqrt, N);
}

TNumeric MakePlusMinus(const TNumeric& N) {
    return MakeFunction(OperatorPlusMinus, N);
}

TNumeric MakeCtg(const TNumeric& N) {
    return MakeFunction(OperatorCtg, N);
}

TNumeric MakeAbs(const TNumeric& N) {
    return MakeFunction(OperatorAbs, N);
}

TNumeric MakeSign(const TNumeric& N) {
    return MakeFunction(OperatorSign, N);
}

TNumeric MakeBigO(const TNumeric& N) {
    return MakeFunction(OperatorBigO, N);
}
TNumeric MakeSmallO(const TNumeric& N) {
    return MakeFunction(OperatorSmallO, N);
}

TNumeric MakeSystemOfEquations(const TNumeric& N1, const TNumeric& N2) {
    TNumeric Res;
    Res.operation = OperatorEqSystem;
    Res.OperandsPushback(N1);
    Res.OperandsPushback(N2);
    return Res;
}
TNumeric MakeSetOfEquations(const TNumeric& N1, const TNumeric& N2) {
    TNumeric Res;
    Res.operation = OperatorEqSet;
    Res.OperandsPushback(N1);
    Res.OperandsPushback(N2);
    return Res;
}

TNumeric MakeIntegral(const TNumeric& N, const TNumeric& dx) {
    TNumeric Res;
    Res.operation = OperatorIntegral;
    Res.OperandsPushback(N);
    Res.OperandsPushback(dx);
    return Res;
}

TNumeric MakeIntegral(const TNumeric& N, const string& dx) {
    TNumeric Res;
    Res.operation = OperatorIntegral;
    Res.OperandsPushback(N);
    Res.OperandsPushback(TNumeric(dx));
    return Res;
}

std::shared_ptr<const TNumeric> FindParent(std::shared_ptr<const TNumeric> Root, std::shared_ptr<const TNumeric> Child) {
    // Обходим рекурсивно Root, чтобы найти родителя WhatToDelete
    if (Root == Child) 
        return nullptr;  // can't be parent for itself
    for (size_t i = 0; i < Root->operands.size(); i++)
        if (Root->operands[i] == Child) {
            return Root;
        };
    for (size_t i = 0; i < Root->operands.size(); i++) {
        std::shared_ptr<const TNumeric> Temp = FindParent(Root->operands[i], Child);
        if (Temp)
            return Temp;
    };
    return nullptr;
}

bool CanErase(std::shared_ptr<const TNumeric> Root, std::shared_ptr<const TNumeric> WhatToDelete) {
    if (FindParent(Root, WhatToDelete) != nullptr)
        return true;
    else
        return false;
}

/// \todo: this function can be implemented using TNumeric::replaceById
/*std::shared_ptr<const TNumeric> EraseNumeric(std::shared_ptr<TNumeric> Root, std::shared_ptr<TNumeric> WhatToDelete) {
    auto res = std::make_shared<TNumeric>(*Root);
    std::shared_ptr<const TNumeric> Parent = FindParent(Root, WhatToDelete);
    assert(Parent != nullptr);
    // if(Parent == nullptr)
    //     return;

    switch (Parent->operation) {
        case OperatorSum:
        case OperatorMinus:
        case OperatorProd:
        case OperatorFrac:
            for (size_t i = 0; i < Parent->operands.size(); i++)
                if (Parent->operands[i] == WhatToDelete) {
                    Parent->operands.erase(Parent->operands.begin() + i);
                    break;
                }
            if (Parent->operands.size() == 1) {
                int op = Parent->operation;
                assert(op == OperatorSum || op == OperatorMinus || op == OperatorProd);
                TNumeric Temp = *Parent->operands[0];
                *Parent = Temp;
            }
            break;
    }
    return res;
}*/

TNumeric NumPi = TNumeric("\\pi");
TNumeric NumPi2 = TNumeric(MakeFrac(TNumeric("\\pi"), TNumeric(2)));
TNumeric NumPi4 = TNumeric(MakeFrac(TNumeric("\\pi"), TNumeric(4)));
TNumeric NumPi3 = TNumeric(MakeFrac(TNumeric("\\pi"), TNumeric(3)));
TNumeric Num2Pi3 = TNumeric(MakeFrac(TNumeric(2) * TNumeric("\\pi"), TNumeric(2)));
TNumeric NumPi6 = TNumeric(MakeFrac(TNumeric("\\pi"), TNumeric(6)));
TNumeric Num3Pi4 = TNumeric(MakeFrac(TNumeric(3) * TNumeric("\\pi"), TNumeric(4)));
TNumeric Num5Pi6 = TNumeric(MakeFrac(TNumeric(5) * TNumeric("\\pi"), TNumeric(6)));
