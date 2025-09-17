//---------------------------------------------------------------------------
#include <string>
#include <fstream>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <sstream>

#include "symbols.h"
#include "arithmetic.h"
#include "mathomatic.h"
#include "algebra_operations.h"

//#define __DEBUG__

void TNumeric::CreateClear()
{
    EditableFlags = NoEditable;
    CursorPos = -1;
    DrawMouse = true;
    Operator = OperatorConst;
    OperandsClear();
    K.clear();
    Active = 0;
    Selected = 0;
    ID = -1;
    MouseX = MouseY = 0;
    OwnSizeActual = false;
    OwnWidth = 0;
    OwnHeight = 0;
    OwnDepth = 0;
}

TNumeric::TNumeric()
{
    CreateClear();
};
TNumeric::~TNumeric()
{
};
TNumeric::TNumeric(double d)
{
    CreateClear();

    if(d == 0)
    {
        K = "0";
    } else {
        Operator = OperatorConst;
        char p[256];
        sprintf(p, "%g", d);
        K = p;
    };
};
TNumeric::TNumeric(int d)
{
    CreateClear();

    if(d == 0)
    {
        K = "0";
    } else {
        Operator = OperatorConst;
        char p[256];
        sprintf(p, "%d", d);
        K = p;
    };
}
TNumeric::TNumeric(size_t d)
{
    CreateClear();

    if(d == 0)
    {
        K = "0";
    } else {
        Operator = OperatorConst;
        char p[256];
        sprintf(p, "%d", (int)d);
        K = p;
    };
}

TNumeric::TNumeric(string d)
{
    CreateClear();

    if(d=="-inf") K="-\\infty"; else
    if(d=="inf") K="\\infty"; else
    if(d=="+inf") K="+\\infty"; else
    K = d;
};
TNumeric::TNumeric(const TNumeric& N)
{
	Assign(N);
};

void TNumeric::LoadFromFile(ifstream &f)
{
    CursorPos = -1;
    Active = 0;
    Selected = 0;

    f.read((char*)&EditableFlags, sizeof(EditableFlags));
    f.read((char*)&DrawMouse, sizeof(DrawMouse));
    f.read((char*)&Operator, sizeof(Operator));
    f.read((char*)&ID, sizeof(ID));
unsigned __int16 KLen;
    f.read((char*)&KLen, sizeof(KLen));
    char *Buf = new char[KLen + 1];
    f.read(Buf, KLen+1); //последний в запись идёт нуль
    if(Buf[KLen] != 0)
        throw "Bad file format";
    K = Buf;

unsigned __int16 OpCount;
    f.read((char*)&OpCount, sizeof(OpCount));
    if(OpCount > 100)
        throw "Bad file format";
    OperandsClear();
    for(size_t i = 0; i < OpCount; i++)
    {
        TNumeric N;
        N.LoadFromFile(f);
        OperandsPushback(N);
    }
}

void TNumeric::WriteToFile(ofstream &f)
{
    f.write((char*)&EditableFlags, sizeof(EditableFlags));
    f.write((char*)&DrawMouse, sizeof(DrawMouse));
    f.write((char*)&Operator, sizeof(Operator));
    f.write((char*)&ID, sizeof(ID));
unsigned __int16 KLen = K.length();
    f.write((char*)&KLen, sizeof(KLen));
    f.write((char*) K.c_str(), K.length()+1); //последний в запись идёт нуль
unsigned __int16 OpCount = Operands.size();
    f.write((char*)&OpCount, sizeof(OpCount));
    for(size_t i = 0; i < Operands.size(); i++)
        Operands[i].WriteToFile(f);
}

bool TNumeric::IsSizeActual() const
{
    if(OwnSizeActual == false) return false;
    for(size_t i = 0; i < Operands.size(); i++)
        if(Operands[i].IsSizeActual() == false)
            return false;
    return true;

}

void TNumeric::OperandsPushback(const TNumeric &Val)
{
    OwnSizeActual = false;
    this->Operands.push_back(Val);
}

void TNumeric::OperandsClear()
{
    OwnSizeActual = false;
    this->Operands.clear();
}

void TNumeric::MakeEqSet(const TNumeric& N1, const TNumeric& N2)
{
   CreateClear();

   OperandsPushback(N1);
   OperandsPushback(N2);
   Operator = OperatorEqSet;
};
void TNumeric::MakeEqSystem(const TNumeric& N1, const TNumeric& N2)
{
    CreateClear();

    OperandsPushback(N1);
    OperandsPushback(N2);
    Operator = OperatorEqSystem;
};

void TNumeric::CheckInitialized() const
{
    if(Operator == OperatorConst && K.empty())
        throw "TNumeric::CheckInitialized: object is not initialized yet";
}

void TNumeric::Assign(const TNumeric& N)
{
   EditableFlags = N.EditableFlags;
   K = N.K;
   OperandsClear();
   if(N.Operands.size() > 0)
   {
       Operands.assign(N.Operands.size(), TNumeric(0));
       for(size_t i = 0; i < N.Operands.size(); i++)
           Operands[i] = N.Operands[i];
   };
   Operator = N.Operator;

   MouseX = N.MouseX;
   MouseY = N.MouseY;
   DrawMouse = N.DrawMouse;
   Selected = N.Selected;
   Active = N.Active;
   CursorPos = N.CursorPos;
   ID = N.ID;
   OwnSizeActual = N.OwnSizeActual;
   OwnWidth = N.OwnWidth;
   OwnHeight = N.OwnHeight;
   OwnDepth = N.OwnDepth;
   TRectangleElement::operator=(N);
};

TNumeric* TNumeric::GetByID(int ID)
{
   if(ID == this->ID) return this;
   for(size_t i = 0; i < Operands.size(); i++)
   {
      TNumeric *P = Operands[i].GetByID(ID);
      if(P != NULL) return P;
   };
   return NULL;
};

void TNumeric::ClearID()
{
    this->ID = -1;
    for(size_t i = 0; i < Operands.size(); i++)
        Operands[i].ClearID();
}


bool TNumeric::IsInteger(int* Int) const
{
TNumeric Simplified = Simplify();
    if(Simplified.Operator == OperatorConst)
    {
        double d;
        if(sscanf(Simplified.K.c_str(), "%lf", &d) == 1)
        {
            if(d == ceil(d))
            {
                if(Int)*Int = (int)d;
                return true;
            };
        }
    }
    return false;
}

bool TNumeric::IsVariable() const
{
    if(Operator == OperatorConst)
    {
        //first letter should be non-nomeric and non-minus
        if(K.length() == 0) throw "TNumeric::IsVariable(): K.length = 0";
        char p = K[0];
        if(p>='0' && p<='9') return false;
        if(p == '-') return false;
        return true;
    };
    if(Operator == OperatorSubIndex)
    {
        return Operands[0].IsVariable() == true;
    }
    return false;
}

bool TNumeric::IsEqual(const TNumeric &N) const
{
    if(Operator != N.Operator) return false;
    if(Operator == OperatorConst) return K == N.K;
    if(Operands.size() != N.Operands.size())return false;
    for(size_t i = 0; i < Operands.size(); i++)
        if(Operands[i].IsEqual(N.Operands[i]) == false) return false;
    return true;
}

bool TNumeric::DependsOn(const char* variable) const
{
    if(Operator == OperatorConst && K == variable) return true;
    for(size_t i = 0; i < Operands.size(); i++)
        if(Operands[i].DependsOn(variable))return true;
    return false;
}


bool TNumeric::operator==(const TNumeric& N) const
{
/*    if(Operator != N.Operator) return false;
    if(Operator == OperatorConst)
    {
        return K == N.K;
    };
    if(Operands.size() != N.Operands.size()) return false;
    for(size_t i = 0; i < Operands.size(); i++)
        if(Operands[i] != N.Operands[i]) return false;
    return true;*/
    if(Operator == OperatorEqual)
    {
        if(Operands.size() != N.Operands.size()) return false;
        for(size_t i = 0; i < Operands.size(); i++)
            if(Operands[i] != N.Operands[i]) return false;
        return true;
    }else{
        TNumeric Diff = *this - N;
        Diff = Diff.Simplify();
        if(Diff.Operator == OperatorConst && Diff.K == "0") return true;
        return false;
    };
}


void TNumeric::operator=(const TNumeric& N)
{
    Assign(N);
};
TNumeric TNumeric::operator*(const TNumeric& N) const
{
    N.CheckInitialized();
/*    if(Operator == OperatorProd)
    {
    //В этом случае просто добавляем в уже существующий список множителей ещё один множитель
    //2do: перенести преобразование (a*b)*c => a*b*c (два произведения в одно) в функцию Simplify
        TNumeric Result = *this;
        Result.OperandsPushback(N);
        return Result;
   } else {*/
//структуру выражения менять не вправе, оставляем всё, как есть
        TNumeric Result;
        Result.Operator = OperatorProd;
        Result.OperandsPushback(*this);
        Result.OperandsPushback(N);
        return Result;
    //};
};
TNumeric TNumeric::operator-() const
{
    CheckInitialized();
TNumeric Res;
   Res.Operator = OperatorProd;
   Res.OperandsPushback(TNumeric(-1));
   Res.OperandsPushback(*this);
   return Res;
};
TNumeric TNumeric::operator+(const TNumeric& N) const
{
    N.CheckInitialized();
/*    if(Operator == OperatorSum)
    {
        //В этом случае просто добавляем в уже существующий список множителей ещё один множитель
        //2do: перенести преобразование (a+b)+c => a+b+c (две суммы в одну) в функцию Simplify
        TNumeric Result = *this;
        Result.OperandsPushback(N);
        return Result;
    } else {*/
        TNumeric Result;
        Result.Operator = OperatorSum;
        Result.OperandsPushback(*this);
        Result.OperandsPushback(N);
        return Result;
   //};
};

TNumeric TNumeric::operator-(const TNumeric& N) const
{
    N.CheckInitialized();
        TNumeric Result;
   Result.Operator = OperatorMinus;
   Result.OperandsPushback(*this);
        Result.OperandsPushback(N);
        return Result;
};

TNumeric TNumeric::operator/(const TNumeric& N) const
{
    N.CheckInitialized();
TNumeric Result;
	Result.Operator = OperatorFrac;
   Result.OperandsPushback(*this);
   Result.OperandsPushback(N);
   return Result;
};
TNumeric TNumeric::operator^(const TNumeric& N) const
{
    N.CheckInitialized();
TNumeric Result;
	Result.Operator = OperatorPow;
   Result.OperandsPushback(*this);
   Result.OperandsPushback(N);
   return Result;
};

TNumeric TNumeric::sqrt() const
{
    CheckInitialized();
TNumeric Result;
	Result.Operator = OperatorSqrt;
   Result.OperandsPushback(*this);
   return Result;
};


TNumeric TNumeric::Derivative(const string VarName) const
{
    switch(this->Operator)
    {
       case OperatorLog:
        {
           TNumeric Nom = MakeLn(Operands[0]);
           TNumeric Denom = MakeLn(Operands[1]);
           return (Nom/Denom).Derivative(VarName);
        }
       case OperatorLn: return Operands[0].Derivative(VarName)/Operands[0];
       case OperatorLg: return Operands[0].Derivative(VarName)/(MakeLn(TNumeric(10))*Operands[0]);
       case OperatorSin: return MakeCos(Operands[0])*Operands[0].Derivative(VarName);
       case OperatorCos: return -MakeSin(Operands[0])*Operands[0].Derivative(VarName);
       case OperatorTg: return TNumeric(1)/MakePow(MakeCos(Operands[0]), TNumeric(2)) * Operands[0].Derivative(VarName);
       case OperatorArcsin: return TNumeric(1)/MakeSqrt(TNumeric(1) - MakePow(Operands[0], TNumeric(2)))*Operands[0].Derivative(VarName);
       case OperatorArccos: return -TNumeric(1)/MakeSqrt(TNumeric(1) - MakePow(Operands[0], TNumeric(2)))*Operands[0].Derivative(VarName);
       case OperatorArctg: return TNumeric(1)/(TNumeric(1)+ MakePow(Operands[0], TNumeric(2)))*Operands[0].Derivative(VarName);
       case OperatorCtg: return -TNumeric(1)/MakePow(MakeSin(Operands[0]), TNumeric(2)) * Operands[0].Derivative(VarName);
       case OperatorAbs: return MakeSign(Operands[0])*Operands[0].Derivative(VarName);
       case OperatorSign: return TNumeric("0");
       case OperatorSum:
        {
            TNumeric Res("0");
            Res.Operator = OperatorSum;
            for(size_t i = 0; i < Operands.size(); i++)
            {
                Res.OperandsPushback(Operands[i].Derivative(VarName));
            }
            return Res;
        }
       case OperatorPlusMinus:
        {
            TNumeric Res("0");
            Res.Operator = OperatorPlusMinus;
            for(size_t i = 0; i < Operands.size(); i++)
            {
                Res.OperandsPushback(Operands[i].Derivative(VarName));
            }
            return Res;
        }
       case OperatorMinus:
        {
            TNumeric Res("0");
            Res.Operator = OperatorMinus;
            for(size_t i = 0; i < Operands.size(); i++)
            {
                Res.OperandsPushback(Operands[i].Derivative(VarName));
            }
            return Res;
        }
       case OperatorConst: if(K != VarName) return TNumeric("0");
        else return TNumeric("1");
       case OperatorProd:
       {
            TNumeric Res("0");
            Res.Operator = OperatorSum;
            for(size_t i = 0; i < Operands.size(); i++)
            {
                TNumeric Term;
                for(size_t j = 0; j < Operands.size(); j++)
                {
                    TNumeric Multiplier;
                    if(i == j) Multiplier = Operands[j].Derivative(VarName);
                    else Multiplier = Operands[j];
                    if(j == 0) Term = Multiplier;
                    else Term = Term*Multiplier;

                }
                Res.OperandsPushback(Term);
            }
            return Res;
       }
       case OperatorFrac: return (Operands[0].Derivative(VarName)*Operands[1] - Operands[0]*Operands[1].Derivative(VarName))/MakePow(Operands[1], TNumeric(2));
       case OperatorSqrt: return TNumeric(1)/(TNumeric(2)*MakeSqrt(Operands[0]))*Operands[0].Derivative(VarName);
       case OperatorExp: return MakeExp(Operands[0])*Operands[0].Derivative(VarName);
       case OperatorSh: return MakeCh(Operands[0])*Operands[0].Derivative(VarName);
       case OperatorCh: return MakeSh(Operands[0])*Operands[0].Derivative(VarName);
       case OperatorPow:
        if(Operands[1].DependsOn(VarName.c_str()) == false)
        {
            return Operands[1]*(Operands[0]^(Operands[1]-TNumeric(1)))*Operands[0].Derivative(VarName);
        };
        return MakeExp(Operands[1]*MakeLn(Operands[0])).Derivative(VarName);
       case OperatorIntegral:
       case OperatorDeriv:       
       default: throw "TNumeric::Derivative:: function is not realized for this type of OpCode";
    };
}


int TNumeric::GetOperatorPriority(int OpCode) const
{
   switch(OpCode)
   {   
      case OperatorInline: return 10;
      case OperatorLess:
      case OperatorLessOrEqual:
      case OperatorGreater:
      case OperatorGreaterOrEqual:
      case OperatorBelongsTo:
      case OperatorEqual: return 0;
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
      case OperatorDeriv: return 0;
      case OperatorUnion:
      case OperatorSum: return 1;
      case OperatorPlusMinus:
      case OperatorMinus: return 2;
      case OperatorConst: return 2; //если константа - положительное число, то скобки никогда не ставятся. Если со знаком минус - то скобки ставятся тогда, когда ставились бы скобки у выражения 0-a
      case OperatorIntersection:
      case OperatorIntegral:
      case OperatorProd: return 3;
      case OperatorFrac: return 3;
      case OperatorSqrt: return 4;
      case OperatorSubIndex:
      case OperatorSupIndex:
      case OperatorSign:
      case OperatorAbs:
      case OperatorPow: return 5;
      default: throw "TNumeric::GetOperatorPriority:: function is not realized for this type of OpCode";
   };
};
int TNumeric::CompareOperatorsPriority(int OpCode1, int OpCode2) const
{
int p1 = GetOperatorPriority(OpCode1);
int p2 = GetOperatorPriority(OpCode2);
   if(p1<p2) return -1;
   if(p1 == p2) return 0;
   return 1;
};

bool TNumeric::CanCalculate() const
{
    try
    {
        if(isnan(Calculate())) return false;
        return true;
    } catch(...)
    {
        return false;
    }
}

double TNumeric::Calculate() const
{
size_t i;
double A;
vector<double> Vals;
    Vals.assign(Operands.size(), 0);
    for(size_t i = 0; i < Operands.size(); i++)
    {
        Vals[i] = Operands[i].Calculate();
        if(isnan(Vals[i]))return NaN;
    };
    switch(Operator)
        {
        case OperatorConst:
            if(K.length() != 0)
            {
                if(K == "-\\infty") return -INFINITY;
                if(K == "\\infty" || K == "+\\infty") return INFINITY;
                if(K == "\\pi") return M_PI;
                try {
                    double d;
                    if(sscanf(K.c_str(), "%lf", &d) == 0) return NaN;
                    A = d;
                }catch(...)
                {
                   return NaN;
                };
                return A;
             } else return NaN;
        break;
        case OperatorEqual: return Vals[1];
        break;
        case OperatorSum:
             A = 0;
             for(i = 0; i < Operands.size(); i++)
                A += Vals[i];
             return A;
        break;
        case OperatorMinus:
           A = Vals[0];
            for(i = 1; i < Operands.size(); i++)
               A -= Vals[i];
            return A;
        break;
        case OperatorProd:
            A = 1;
            for(i = 0; i < Operands.size(); i++)
               A*=Vals[i];
            return A;
        break;
        case OperatorFrac:
           if(Vals[1] == 0)
            {
               if(Vals[0]>=0) return Infinity;
                else return -Infinity;
            } return Vals[0]/Vals[1];
        break;
        case OperatorSqrt:
            if(Vals[0]>=0) return sqrtl(Vals[0]);
            else return NaN;
        break;
        case OperatorPow:
            if(Vals[1] == 0) return 1;
            if(Vals[0]>0) return exp(Vals[1]*log(Vals[0]));
            else {
                if(ceil(Vals[1]) == Vals[1]) //показатель целый
                {
                    //BInt нужно только для определения знака
                    int BInt = ceil(Vals[1]);
                    if(BInt < 0) BInt = -BInt;
                    int sign;
                    if(BInt % 2 == 0) sign = 1;
                    else sign = -1;
                    return exp(Vals[1]*log(-Vals[0]))*sign;
                } return NaN;
            };
        break;
        case OperatorExp:
            if(Vals[0] == 0) return 1;
            return exp(Vals[0]);
        break;
        case OperatorSh:
            if(Vals[0] == 0) return 0;
            return sinh(Vals[0]);
        break;
        case OperatorCh:
            if(Vals[0] == 0) return 1;
            return cosh(Vals[0]);
        break;
        case OperatorLog:
            if(Vals[0] <= 0 || Vals[1] <= 0) return NaN;
            return log(Vals[0])/log(Vals[1]);
        case OperatorLn:
            if(Vals[0] <= 0) return NaN;
            return log(Vals[0]);
        case OperatorLg:
            if(Vals[0] <= 0) return NaN;
            return log(Vals[0])/log(10.0L);
        case OperatorSin:
            return sin(Vals[0]);
        case OperatorCos:
            return cos(Vals[0]);
        case OperatorTg:
            if(cos(Vals[0]) == 0)
            {
                if(sin(Vals[0])>0) return +Infinity;
                else return -Infinity;
            }
            return sin(Vals[0])/cos(Vals[0]);
        case OperatorCtg:
            if(sin(Vals[0]) == 0)
            {
                if(cos(Vals[0])>0) return +Infinity;
                else return -Infinity;
            }
           return cos(Vals[0])/sin(Vals[0]);
        case OperatorArcsin:
            if(fabs(Vals[0]) > 1) return NaN;
            else return asin(Vals[0]);
        case OperatorArccos:
            if(fabs(Vals[0]) > 1) return NaN;
            else return acos(Vals[0]);
        case OperatorArctg:
            return atan(Vals[0]);
        case OperatorSign:
            if(Vals[0] > 0) return 1;
            else if(Vals[0] < 0) return -1;
            else return 0;
        case OperatorAbs: return fabs(Vals[0]);
        case OperatorLess:
        case OperatorLessOrEqual:
        case OperatorGreater:
        case OperatorGreaterOrEqual:
        case OperatorEqSet:
        case OperatorEqSystem:
        case OperatorLines:
        case OperatorInterval:
        case OperatorIntervalSegment:
        case OperatorSegmentInterval:
        case OperatorSegment:
        case OperatorInline:
        case OperatorTab:
        case OperatorUnion:
        case OperatorIntersection:
        case OperatorSubIndex:
        case OperatorSupIndex:
        case OperatorDeriv:
        case OperatorPlusMinus:
        case OperatorIntegral:
        default:
        throw "TNumeric::Calculate(): Can't calculate the EqSet, EqSystem or lines operator etc";
      break;
   };
   return NaN;
};

TNumeric TNumeric::Substitute(const string &var, const TNumeric& Val) const
{
    if(Operator == OperatorConst)
    {
        if(K == var) return Val;
        else return *this;
    };
TNumeric Res;
    Res.Operator = Operator;
    for(size_t i = 0; i < Operands.size(); i++)
        Res.OperandsPushback(Operands[i].Substitute(var, Val));
    return Res;
}

void TNumeric::SetEditableFlags(int Flags)
{
    this->EditableFlags = Flags;
    for(size_t i = 0; i < Operands.size(); i++)
        Operands[i].SetEditableFlags(Flags);
}

void TNumeric::PrettyGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    if(IsSizeActual() && NeedBrackets == this->SizeWithBrackets)
    {
        Width = this->OwnWidth;
        Height = this->OwnHeight;
        Depth = this->OwnDepth;
    };
string AStr;
    Width = 0;
    Height = 0;
    Depth = 0;
    switch(Operator)
    {
        case OperatorConst:
        if(K.length() != 0)
        {
            if(K.find("_") != string::npos)
            {
                size_t pos = K.find("_");
                TNumeric N = MakeSubscript(TNumeric(K.substr(0, pos)), TNumeric(K.substr(pos+1)));
                N.PrettyGetTextRectangle(Canvas, Width, Height, Depth, false, Simplify);
            } else {
                AStr = Recognize(K);
                //Если требуются скобки, то ставим их. Но если число положительное - то скобки никогда не ставятся
                //if(NeedBrackets)
                //{
                    //проверяем отрицательность числа
                    double f;
                    if(sscanf(AStr.c_str(), "%lf", &f) > 0) //todo: OPTIMIZE HERE
                    {
                        //отрицательное число всегда в скобках
                        if(f < 0) AStr = string("(")+AStr+")";
                    }
               //}
               Width =Canvas->TextWidth(AStr);
               Height = Canvas->TightTextHeight(AStr);
               Depth = Canvas->TightTextDepth(AStr);
            };
         } else {
            AStr = "0";
            Width = Canvas->TextWidth(AStr);
            Height = Canvas->TightTextHeight(AStr);
            Depth = Canvas->TightTextDepth(AStr);
         };
        break;
        case OperatorInline:  InlineGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify); break;
        case OperatorInterval:
        case OperatorIntervalSegment:
        case OperatorSegmentInterval:
        case OperatorSegment: IntervalGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
        case OperatorLess:
        case OperatorLessOrEqual:
        case OperatorGreater:
        case OperatorGreaterOrEqual:
        case OperatorEqual: EqualGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
        case OperatorSum: SumGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
        case OperatorMinus: MinusGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
        case OperatorBelongsTo: BelongsToGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
        case OperatorProd: ProdGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
        case OperatorFrac: FracGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
        case OperatorSqrt: SqrtGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
        case OperatorSupIndex:
        case OperatorPow: PowGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
        case OperatorSubIndex: SubIndexGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
        case OperatorEqSet: EqSetGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
        case OperatorEqSystem: EqSystemGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
        case OperatorTab: LinesGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, 40, Simplify);
        break;
        case OperatorLines: LinesGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, 0, Simplify);
        break;
        case OperatorUnion: UnionGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
        case OperatorIntersection: IntersectionGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
        case OperatorDeriv: DerivGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
        case OperatorPlusMinus: PlusMinusGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
        case OperatorIntegral: IntegralGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
        case OperatorAbs: AbsGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
        case OperatorLog:
        case OperatorLn:
        case OperatorLg:
        case OperatorSin:
        case OperatorCos:
        case OperatorTg:
        case OperatorCtg:
        case OperatorArcsin:
        case OperatorArccos:
        case OperatorArctg:
        case OperatorSh:
        case OperatorCh:
        case OperatorExp:
        case OperatorSign:
        case OperatorSmallO:
        case OperatorBigO:
            FunctionGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
   };
   OwnSizeActual = true;
   this->OwnWidth = Width;
   this->OwnHeight = Height;
   this->OwnDepth = Depth;
   this->SizeWithBrackets = NeedBrackets;
};

void TNumeric::DrawAtBaseLeft(TPaintCanvas* Canvas, int X, int Y, int MaxWidth) const
{
    Q_UNUSED(MaxWidth);
#ifdef __DEBUG__
    cout<<CodeBasic()<<endl;
#endif
    //MaxWidth не используется
    Canvas->Font = Canvas->FormulaFont;
    DrawAtBaseLeft(Canvas, X, Y, false);
}

int TNumeric::PrettyDrawAtBaseLeft(TPaintCanvas* Canvas, int X, int Y, bool NeedBrackets, bool Simplify) const
//Y = baseline
{
int W, H, D;
    PrettyGetTextRectangle(Canvas, W, H, D, NeedBrackets, Simplify);
#ifdef __DEBUG__
    Canvas->Rectangle(X, Y-H, X+W, Y+D, Qt::gray);
#endif

    for(size_t i = 0; i < Operands.size(); i++)
    {
        Operands[i].DrawMouse = DrawMouse;
        Operands[i].MouseX = MouseX;
        Operands[i].MouseY = MouseY;
        Operands[i].Active = 0;
        Operands[i].Selected = Selected;
    };

   Active = 0;

   if(EditableFlags != NoEditable)
       Canvas->Pen.setColor(Canvas->EditableColor);
   else
       Canvas->Pen.setColor(Canvas->FormulaColor);

    switch(Operator)
    {
        case OperatorConst:
            if(K.length() != 0)
            {
                if(K.find("_") != string::npos)
                {
                    size_t pos = K.find("_");
                    TNumeric N = MakeSubscript(TNumeric(K.substr(0, pos)), TNumeric(K.substr(pos+1)));
                    return N.PrettyDrawAtBaseLeft(Canvas, X, Y, false, Simplify);
                } else {
                    string AStr = Recognize(K);
                    //Если требуются скобки, то ставим их. Но если число положительное - то скобки никогда не ставятся
/*                    if(NeedBrackets)
                    {*/
                        //проверяем отрицательность числа                        
                        double f;
                        if(sscanf(AStr.c_str(), "%lf", &f) > 0)
                        {
                            //отрицательное число всегда в скобках
                            if(f < 0) AStr = string("(")+AStr+")";
                        }
//                   }
                     Canvas->TextOutA(X, Y, AStr);
                };
            } else {
                Canvas->Rectangle(X+1, Y-H+2, X+W-1, Y+D-2);
            };
        break;
        case OperatorInline:  InlineDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify); break;
        case OperatorInterval:
        case OperatorIntervalSegment:
        case OperatorSegmentInterval:
        case OperatorSegment: IntervalDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify); break;
        case OperatorLess:
        case OperatorLessOrEqual:
        case OperatorGreater:
        case OperatorGreaterOrEqual:
        case OperatorEqual:  EqualDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
        case OperatorSum:  SumDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
        case OperatorMinus:  MinusDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
        case OperatorBelongsTo:  BelongsToDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
        case OperatorProd: ProdDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
        case OperatorFrac: FracDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
        case OperatorSqrt: SqrtDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
        case OperatorSupIndex:
        case OperatorPow: PowDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
        case OperatorSubIndex: SubIndexDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
        case OperatorEqSet: EqSetDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
        case OperatorEqSystem: EqSystemDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
        case OperatorTab: LinesDraw(Canvas, X, Y, W, H, D, NeedBrackets, 40, Simplify);
        break;
        case OperatorLines: LinesDraw(Canvas, X, Y, W, H, D, NeedBrackets, 0, Simplify);
        break;
        case OperatorUnion: UnionDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
        case OperatorIntersection: IntersectionDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
        case OperatorDeriv: DerivDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
        case OperatorPlusMinus: PlusMinusDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
        case OperatorIntegral: IntegralDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
        case OperatorAbs: AbsDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
        case OperatorLog:
        case OperatorLn:
        case OperatorLg:
        case OperatorSin:
        case OperatorCos:
        case OperatorTg:
        case OperatorCtg:
        case OperatorArcsin:
        case OperatorArccos:
        case OperatorArctg:
        case OperatorExp:
        case OperatorSh:
        case OperatorCh:
        case OperatorSign:
        case OperatorSmallO:
        case OperatorBigO:
            FunctionDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
   };

   if(DrawMouse)
   {
        //определяем, лежит ли элемент this под мышкой
        if(Active==0 && MouseX > X && MouseX < X+W && MouseY < Y + D && MouseY > Y - H)
            Active = (TNumeric*) this;
        //если кто-то из Operands лежит под мышкой, то это важнее - транслируем Active на верх к корню дерева
        for(size_t i = 0; i < Operands.size(); i++)
            if(Operands[i].Active)
                Active = Operands[i].Active;
        //если этот элемент является Active, то рисуем рамку
        if(Active == this)
            Canvas->Rectangle(X - 1, Y + D + 1, X + W + 1, Y - H - 1);
   };

   if(Selected == this)
   {
     Canvas->Rectangle(X - 1, Y + D + 1, X + W + 1, Y - H - 1, Qt::blue);
   };
#ifdef __DEBUG__
   cout<<CodeBasic()<<endl;
#endif
   return 0;
};

void TNumeric::DrawBracket(char Bracket, TPaintCanvas* Canvas, int X, int Y, int Height, int Depth) const
{
string Str;
    Str += Bracket;
QFont Temp = Canvas->Font;
    Canvas->Font.setWeight(QFont::Light);
    int dBaseLine = Canvas->SetTextHPlusD(Str, Height, Depth);
    //int RealH = Canvas->TextHeight(Str);
    //int RealD = Canvas->TextDepth(Str);
    //int W = Canvas->TextWidth(Str);

    Canvas->TextOutA(X, Y-dBaseLine, Str);
 //   Canvas->TextOutA(X, Y-RealD, Str);
    Canvas->Font = Temp;
};
int TNumeric::GetBracketWidth(char Bracket, TPaintCanvas* Canvas, int Height, int Depth) const
{
    Q_UNUSED(Depth);
    Q_UNUSED(Height);
string Str;
    Str += Bracket;
QFont Temp = Canvas->Font;
    Canvas->Font.setWeight(QFont::Light);
//    int dBaseLine = Canvas->SetTextHPlusD(Str, Height, Depth);
int    res = Canvas->TextWidth(Str);
    Canvas->Font = Temp;
    return res;
};

void TNumeric::DrawBracket(string Bracket, TPaintCanvas* Canvas, int X, int Y, int Height, int Depth) const
{
string Str;
    Str += Bracket;
QFont Temp = Canvas->Font;
    Canvas->Font.setWeight(QFont::Light);
    int dBaseLine = Canvas->SetTextHPlusD(Bracket, Height, Depth);
//    Canvas->TextOutRect(X, Y-Height, X+W, Y+Depth+5, Str);
    Canvas->TextOutA(X, Y - dBaseLine, Str);
#ifdef __DEBUG__
    int W, RealH, RealD;
    Canvas->TightTextWHD(Str, &W, &RealH, &RealD);
    //Canvas->Rectangle(X, Y-Height - dBaseLine, X+W, Y+Depth, Qt::gray);
    //Canvas->Rectangle(X, Y-RealH - dBaseLine, X+W, Y+RealD - dBaseLine, Qt::red);
#endif
    Canvas->Font = Temp;
};
int TNumeric::GetBracketWidth(string Bracket, TPaintCanvas* Canvas, int Height, int Depth) const
{
string Str;
    Str += Bracket;
QFont Temp = Canvas->Font;
    Canvas->Font.setWeight(QFont::Light);
    Canvas->SetTextHPlusD(Bracket, Height, Depth);
int    res = Canvas->TextWidth(Str);
//int    res = Canvas->TightTextWidth(Str);
    Canvas->Font = Temp;
    return res;
};


void TNumeric::FracGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
int W, H, D;
    int MidLine = Canvas->TextHeight("1")/2;
    Width = Height = Depth = 0;
    Operands[0].PrettyGetTextRectangle(Canvas, W, H, D, false, Simplify);
    Height = H + D + 2 + MidLine;
    if(Width < W)
    Width = W;
    Operands[1].PrettyGetTextRectangle(Canvas, W, H, D, false, Simplify);
    Depth = H + D + 2 - MidLine;
    if(Depth < 0) Depth = 0;
    if(Width < W)
        Width = W;
    if(WithBrackets)
    {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
};
void TNumeric::FracDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
int X1, Y1, D1;
int WidthOfFrac;
   FracGetTextRectangle(Canvas, X1, Y1, D1, false, Simplify);
   WidthOfFrac = X1;
   if(NeedBrackets)
   {
       DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
       DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X + Width - GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth), Y, Height, Depth);
       X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
   };

   int MidLine = Y - Canvas->TextHeight("1")/2;

   Operands[0].PrettyGetTextRectangle(Canvas, X1, Y1, D1, false, Simplify);
   Operands[0].PrettyDrawAtBaseLeft(Canvas, X - X1/2 + WidthOfFrac/2, MidLine - D1 - 2, false, Simplify);

   Operands[1].PrettyGetTextRectangle(Canvas, X1, Y1, D1, false, Simplify);
   Operands[1].PrettyDrawAtBaseLeft(Canvas, X - X1/2 + WidthOfFrac/2, MidLine + Y1 + 2, false, Simplify);

   Canvas->Line(X, MidLine, X+WidthOfFrac, MidLine, Qt::black);
};


//==============================================================================

void TNumeric::FunctionGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
string Name;
    switch(Operator)
    {
        case OperatorLog: Name = "log"; break;
        case OperatorLn: Name = "ln"; break;
        case OperatorLg: Name = "lg"; break;
        case OperatorSin: Name = "sin"; break;
        case OperatorCos: Name = "cos"; break;
        case OperatorTg: Name = "tg"; break;
        case OperatorCtg: Name = "ctg"; break;
        case OperatorArcsin: Name = "arcsin"; break;
        case OperatorArccos: Name = "arccos"; break;
        case OperatorArctg: Name = "arctg"; break;
        case OperatorExp: Name = "exp"; break;
        case OperatorSh: Name = "sh"; break;
        case OperatorCh: Name = "ch"; break;
        case OperatorSign: Name = "sign"; break;
        case OperatorSmallO: Name = "o"; break;
        case OperatorBigO: Name = "O"; break;
        default: throw "TNumeric::FunctionGetTextRectangle: Unknown Function";
    };
    QFont FTemp = Canvas->Font;
    Canvas->Font.setBold(true);
    Width += Canvas->TextWidth(Name) + Canvas->TextWidth(" ");
    Height = Canvas->TextHeight(Name);
    Depth = Canvas->TextDepth(Name);
    Canvas->Font = FTemp;
    int W1, H1, D1;
    int W2, H2, D2;
    if(Operator == OperatorLog)
    {
        QFont Font;
        Font = DecreaseFont(Canvas);
        Operands[1].PrettyGetTextRectangle(Canvas, W2, H2, D2, true, Simplify);

        Canvas->Font = Font;
        Width = Width + W2;
        //int dDepth = H2 + D2 - Height/2;
        int dDepth = H2 + D2;
        if(dDepth > Depth)  Depth = dDepth;
    }   
    Operands[0].PrettyGetTextRectangle(Canvas, W1, H1, D1, true, Simplify);
    Width += W1;
    if(Height < H1) Height = H1;
    if(Depth < D1) Depth = D1;
    if(WithBrackets)
    {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
}

void TNumeric::FunctionDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(Width);
string Name;
    switch(Operator)
    {
        case OperatorLog: Name = "log"; break;
        case OperatorLn: Name = "ln"; break;
        case OperatorLg: Name = "lg"; break;
        case OperatorSin: Name = "sin"; break;
        case OperatorCos: Name = "cos"; break;
        case OperatorTg: Name = "tg"; break;
        case OperatorCtg: Name = "ctg"; break;
        case OperatorArcsin: Name = "arcsin"; break;
        case OperatorArccos: Name = "arccos"; break;
        case OperatorArctg: Name = "arctg"; break;
        case OperatorExp: Name = "exp"; break;
        case OperatorSh: Name = "sh"; break;
        case OperatorCh: Name = "ch"; break;
        case OperatorSign: Name = "sign"; break;
        case OperatorBigO: Name = "O"; break;
        case OperatorSmallO: Name = "o"; break;
        default: throw "TNumeric::FunctionGetTextRectangle: Unknown Function";
    };
#ifdef __DEBUG__
    cout<<CodeBasic()<<endl;
#endif

    if(NeedBrackets)
    {
         DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
         X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
    };
    QFont FTemp = Canvas->Font;
    Canvas->Font.setBold(true);
    Canvas->TextOutA(X, Y, Name);
    X += Canvas->TextWidth(Name) + Canvas->TextWidth(" ")/2;
    Canvas->Font = FTemp;
    int W1, H1, D1;
    int W2, H2, D2;

    if(Operator == OperatorLog)
    {
        QFont Font = DecreaseFont(Canvas);

        Operands[1].PrettyGetTextRectangle(Canvas, W1, H1, D1, true, Simplify);
//        Operands[1].PrettyDrawAtBaseLeft(Canvas, X, Y - Height/2 + H1, true, Simplify);
        Operands[1].PrettyDrawAtBaseLeft(Canvas, X, Y + H1, true, Simplify);
        Canvas->Font = Font;

        X += W1;

    };
    Operands[0].PrettyGetTextRectangle(Canvas, W2, H2, D2, true, Simplify);
    Operands[0].PrettyDrawAtBaseLeft(Canvas, X, Y, true, Simplify);
    X+= W2;

    if(NeedBrackets)
         DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X, Y, Height, Depth);
}

//==============================================================================
void TNumeric::StrGetTextRectangle(const char* Str, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
int W, H, D;
    for(size_t i = 0; i < Operands.size(); i++)
    {
        bool NeedBrackets = false;
        if(CompareOperatorsPriority(Operator, Operands[i].Operator)>0) NeedBrackets = true;
        Operands[i].PrettyGetTextRectangle(Canvas, W, H, D, NeedBrackets, Simplify);
        Width += W;
        if(H > Height)
            Height = H;
        if(D > Depth)
            Depth = D;
    };
    Width += (Operands.size()-1)*(Canvas->TextWidth(Str) + Canvas->TextWidth(" "));
    if(WithBrackets)
    {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
};
void TNumeric::StrDraw(const char* Str, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
   Q_UNUSED(Width);
   if(NeedBrackets)
   {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
   };
   for(size_t i = 0; i < Operands.size(); i++)
   {
   	int W1, H1, D1;
      bool NeedBrackets = false;
      if(CompareOperatorsPriority(Operator, Operands[i].Operator)>0) NeedBrackets = true;
      Operands[i].PrettyGetTextRectangle(Canvas, W1, H1, D1, NeedBrackets, Simplify);
      Operands[i].PrettyDrawAtBaseLeft(Canvas, X, Y, NeedBrackets, Simplify);
      X = X + W1;
      if(i+1 < Operands.size())
      {
//          int PlusHeight = Canvas->TextHeight(Str);
          Canvas->TextOutA(X, Y, Str);
          X = X + Canvas->TextWidth(Str);
      };
   };
   if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X, Y, Height, Depth);
};
//==============================================================================
//==============================================================================
void TNumeric::IntervalGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    Q_UNUSED(WithBrackets);
int W, H, D;
    for(size_t i = 0; i < Operands.size(); i++)
    {
        Operands[i].PrettyGetTextRectangle(Canvas, W, H, D, false, Simplify);
        Width += W;
        if(H > Height)
            Height = H;
        if(D > Depth)
            Depth = D;
    };
    Width += (Operands.size()-1)*Canvas->TextWidth(",");
string LeftBracket = UniCode2UTF8String(MyLeftParenthesis),RightBracket = UniCode2UTF8String(MyRightParenthesis);
    switch(Operator)
    {
        case OperatorInterval: LeftBracket=UniCode2UTF8String(MyLeftParenthesis); RightBracket=UniCode2UTF8String(MyRightParenthesis); break;
        case OperatorSegmentInterval: LeftBracket='['; RightBracket=UniCode2UTF8String(MyRightParenthesis); break;
        case OperatorIntervalSegment: LeftBracket=UniCode2UTF8String(MyLeftParenthesis); RightBracket=']'; break;
        case OperatorSegment: LeftBracket='['; RightBracket=']'; break;
    };

    Width = Width + GetBracketWidth(LeftBracket, Canvas, Height, Depth) + GetBracketWidth(RightBracket, Canvas, Height, Depth);
};
void TNumeric::IntervalDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(NeedBrackets);
    Q_UNUSED(Width);
    string LeftBracket = UniCode2UTF8String(MyLeftParenthesis), RightBracket = UniCode2UTF8String(MyRightParenthesis);
    switch(Operator)
    {
        case OperatorInterval: LeftBracket=UniCode2UTF8String(MyLeftParenthesis); RightBracket=UniCode2UTF8String(MyRightParenthesis); break;
        case OperatorSegmentInterval: LeftBracket='['; RightBracket=UniCode2UTF8String(MyRightParenthesis); break;
        case OperatorIntervalSegment: LeftBracket=UniCode2UTF8String(MyLeftParenthesis); RightBracket=']'; break;
        case OperatorSegment: LeftBracket='['; RightBracket=']'; break;
   };
   DrawBracket(LeftBracket, Canvas, X, Y, Height, Depth);
   X = X + GetBracketWidth(LeftBracket, Canvas, Height, Depth);
   for(size_t i = 0; i < Operands.size(); i++)
   {
        int W1, H1, D1;
        Operands[i].PrettyGetTextRectangle(Canvas, W1, H1, D1, false, Simplify);
        Operands[i].PrettyDrawAtBaseLeft(Canvas, X, Y, false, Simplify);
        X = X + W1;
        if(i+1 < Operands.size())
        {
          Canvas->TextOutA(X, Y, ",");
          X = X + Canvas->TextWidth(",");
        };
   };
   DrawBracket(RightBracket, Canvas, X, Y, Height, Depth);
};
//==============================================================================
//==============================================================================
void TNumeric::InlineGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
int W, H, D;
    for(size_t i = 0; i < Operands.size(); i++)
    {
        Operands[i].PrettyGetTextRectangle(Canvas, W, H, D, false, Simplify);
        Width += W;
        if(H > Height)
            Height = H;
        if(D > Depth)
            Depth = D;
    };
    if(WithBrackets)
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
};
void TNumeric::InlineDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(Width);
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
    };
    for(size_t i = 0; i < Operands.size(); i++)
    {
        int W1, H1, D1;
        Operands[i].PrettyGetTextRectangle(Canvas, W1, H1, D1, false, Simplify);
        Operands[i].PrettyDrawAtBaseLeft(Canvas, X, Y, false, Simplify);
        X = X + W1;
    };
    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X, Y, Height, Depth);
};
//==============================================================================
//==============================================================================
void TNumeric::SumGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
TNumeric Temp(*this); //работаем с Temp, так как this = const
int W, H, D;
string StrPlus = UniCode2UTF8String(MyPlusSign);
string StrMinus = UniCode2UTF8String(MyMinusSign);
vector<size_t> Exclude; //слагаемые, равные нулю, их лучше всего исключить
#ifdef __DEBUG__
    cout<<CodeBasic()<<endl;
#endif
    if(Simplify)
    {
        for(size_t i = 0; i < Temp.Operands.size(); i++)
        {
            if(Temp.Operands[i].Operator == OperatorConst && Temp.Operands[i].K == "0")Exclude.push_back(i);
            if(Temp.Operands[i].Operator == OperatorFrac)
            {
                if(Temp.Operands[i].Operands[0].Operator==OperatorConst && Temp.Operands[i].Operands[0].K == "0")Exclude.push_back(i);
            }
            if(Temp.Operands[i].Operator == OperatorProd)
            {
                for(size_t j = 0; j < Temp.Operands[i].Operands.size(); j++)
                    if(Temp.Operands[i].Operands[j].Operator==OperatorConst && Temp.Operands[i].Operands[j].K == "0")
                    {
                        Exclude.push_back(i);
                        break;
                    };
            }
        }
        if(Exclude.size() == Temp.Operands.size() && Temp.Operands.size() > 0)
        //все слагаемые - нули, оставляем только одно из них
        {
            Exclude.erase(Exclude.begin());
        };
    };
size_t Drawn = 0; //количество уже отрисованных слагаемых
    for(size_t i = 0; i < Temp.Operands.size(); i++)
    {
        if(find(Exclude.begin(), Exclude.end(), i) != Exclude.end())continue;
        bool NeedBrackets = false;
        if(CompareOperatorsPriority(Operator, Temp.Operands[i].Operator)>0) NeedBrackets = true;
        vector<size_t> TakenMinuses; //Операнды, у которых минусы уже учтены в знаке перед произведением или дробью
        size_t MinusesCount = 0;

        if((Simplify) & (Drawn > 0))
            //Пытаемся вынести минус, эта операция не явлеятся thread-safe
            //у первого (Drawn = 0) слагаемого минус не выносится
        {
            if(Temp.Operands[i].Operator == OperatorProd || Temp.Operands[i].Operator == OperatorFrac)
            {
                for(size_t j = 0; j < Temp.Operands[i].Operands.size(); j++)
                {
                    if(Temp.Operands[i].Operands[j].Operator == OperatorConst)
                    {
                        if(Temp.Operands[i].Operands[j].K.length() > 0 && Temp.Operands[i].Operands[j].K[0]=='-')
                        {
                            TakenMinuses.push_back(j);
                            Temp.Operands[i].Operands[j].K.erase(Temp.Operands[i].Operands[j].K.begin());
                            MinusesCount++;
                        }
                    }
                }
            }
            if(Temp.Operands[i].Operator == OperatorConst)
                if(Temp.Operands[i].K.length() > 0 && Temp.Operands[i].K[0]=='-')
                {
                    MinusesCount++;
                    Temp.Operands[i].K.erase(Temp.Operands[i].K.begin());
                }
        }
        //рисуем знак
        if(MinusesCount % 2 == 0)
        {
            //плюс, перед первым членом не ставится
            if(Drawn>0) Width += Canvas->TextWidth(StrPlus);
        }
        else
        {
            //минус
            Width += Canvas->TextWidth(StrMinus);
        }

        if(CompareOperatorsPriority(Operator, Temp.Operands[i].Operator)>0) NeedBrackets = true;
        if(Simplify)
            //если надо было упростить, то рисуем упрощенные слагаемые
        {
            Temp.Operands[i].PrettyGetTextRectangle(Canvas, W, H, D, NeedBrackets, Simplify);
        } else {
            //ничего не упрощали, тогда рисуем исходные слагаемые. При этом поле Active у этих слагаемых меняется и вытаскивается функцией PrettyDrawAtBaseLeft в корень дерева
            Operands[i].PrettyGetTextRectangle(Canvas, W, H, D, NeedBrackets, Simplify);
        }

        if(Simplify)
        {
        //возвращаем минус обратно
            if(Temp.Operands[i].Operator == OperatorProd || Temp.Operands[i].Operator == OperatorFrac)
            {
                for(size_t j = 0; j < TakenMinuses.size(); j++)
                    Temp.Operands[i].Operands[TakenMinuses[j]].K = string("-")+Temp.Operands[i].Operands[TakenMinuses[j]].K;
            }
            if(Temp.Operands[i].Operator == OperatorConst && MinusesCount % 2 == 1)
            {
                Temp.Operands[i].K = string("-") + Temp.Operands[i].K;
            }
        };
        Width += W;
        if(H > Height)
            Height = H;
        if(D > Depth)
            Depth = D;
        Drawn++;
    };
    if(WithBrackets)
    {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
#ifdef __DEBUG__
    cout<<CodeBasic()<<endl;
#endif
};
void TNumeric::SumDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(Width);
#ifdef __DEBUG__
    cout<<CodeBasic()<<endl;
#endif
    TNumeric Temp(*this); //работаем с Temp, так как this = const
   if(NeedBrackets)
   {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
   };
vector<size_t> Exclude; //слагаемые, равные нулю, их лучше всего исключить
    if(Simplify)
    {
        for(size_t i = 0; i < Temp.Operands.size(); i++)
        {
            if(Temp.Operands[i].Operator == OperatorConst && Temp.Operands[i].K == "0")Exclude.push_back(i);
            if(Temp.Operands[i].Operator == OperatorFrac)
            {
                if(Temp.Operands[i].Operands[0].Operator==OperatorConst && Temp.Operands[i].Operands[0].K == "0")Exclude.push_back(i);
            }
            if(Temp.Operands[i].Operator == OperatorProd)
            {
                for(size_t j = 0; j < Temp.Operands[i].Operands.size(); j++)
                    if(Temp.Operands[i].Operands[j].Operator==OperatorConst && Temp.Operands[i].Operands[j].K == "0")
                    {
                        Exclude.push_back(i);
                        break;
                    };
            }
        }
        if(Exclude.size() == Temp.Operands.size() && Temp.Operands.size() > 0)
        //все слагаемые - нули, оставляем только одно из них
        {
            Exclude.erase(Exclude.begin());
        };
    };

string StrPlus = UniCode2UTF8String(MyPlusSign);
string StrMinus = UniCode2UTF8String(MyMinusSign);
size_t Drawn = 0; //количество уже отрисованных слагаемых

   for(size_t i = 0; i < Temp.Operands.size(); i++)
   {
       if(find(Exclude.begin(), Exclude.end(), i) != Exclude.end())continue;
       int W1, H1, D1;
       bool NeedBrackets = false;
       if(CompareOperatorsPriority(Operator, Temp.Operands[i].Operator)>0) NeedBrackets = true;
       vector<size_t> TakenMinuses; //Операнды, у которых минусы уже учтены в знаке перед произведением или дробью
       size_t MinusesCount = 0 ;

       if(Simplify && Drawn > 0)
       //Пытаемся вынести минус, эта операция не явлеятся thread-safe
       //у первого (Drawn = 0) слагаемого минус не выносится
       {
           if(Temp.Operands[i].Operator == OperatorProd || Temp.Operands[i].Operator == OperatorFrac)
           {
               for(size_t j = 0; j < Temp.Operands[i].Operands.size(); j++)
               {
                   if(Temp.Operands[i].Operands[j].Operator == OperatorConst)
                   {
                       if(Temp.Operands[i].Operands[j].K.length() > 0 && Temp.Operands[i].Operands[j].K[0]=='-')
                       {
                           TakenMinuses.push_back(j);
                           Temp.Operands[i].Operands[j].K.erase(Temp.Operands[i].Operands[j].K.begin());
                           MinusesCount++;
                       }
                   }
               }
           }
           if(Temp.Operands[i].Operator == OperatorConst)
               if(Temp.Operands[i].K.length() > 0 && Temp.Operands[i].K[0]=='-')
               {
                   MinusesCount++;
                   Temp.Operands[i].K.erase(Temp.Operands[i].K.begin());
               }
       }
       //рисуем знак
       if(MinusesCount % 2 == 0)
       {
           //плюс, перед первым членом не ставится
           if(Drawn>0)
           {
               Canvas->TextOutA(X, Y, StrPlus);
               X = X + Canvas->TextWidth(StrPlus);
           }
       }
       else
       {
           //минус, рисуется всегда
           Canvas->TextOutA(X, Y, StrMinus);
           X = X + Canvas->TextWidth(StrMinus);
       }

       if(CompareOperatorsPriority(Operator, Temp.Operands[i].Operator)>0) NeedBrackets = true;
       if(Simplify)
           //если надо было упростить, то рисуем упрощенные слагаемые
       {
           Temp.Operands[i].PrettyGetTextRectangle(Canvas, W1, H1, D1, NeedBrackets, Simplify);
           Temp.Operands[i].PrettyDrawAtBaseLeft(Canvas, X, Y, NeedBrackets, Simplify);
       } else {
           //ничего не упрощали, тогда рисуем исходные слагаемые. При этом поле Active у этих слагаемых меняется и вытаскивается функцией PrettyDrawAtBaseLeft в корень дерева
           Operands[i].PrettyGetTextRectangle(Canvas, W1, H1, D1, NeedBrackets, Simplify);
           Operands[i].PrettyDrawAtBaseLeft(Canvas, X, Y, NeedBrackets, Simplify);
       }

       if(Simplify)
       {
       //возвращаем минус обратно
           if(Temp.Operands[i].Operator == OperatorProd || Temp.Operands[i].Operator == OperatorFrac)
           {
               for(size_t j = 0; j < TakenMinuses.size(); j++)
                   Temp.Operands[i].Operands[TakenMinuses[j]].K = string("-")+Temp.Operands[i].Operands[TakenMinuses[j]].K;
           }
           if(Temp.Operands[i].Operator == OperatorConst && MinusesCount % 2 == 1)
           {
               Temp.Operands[i].K = string("-") + Temp.Operands[i].K;
           }
       };

       X = X + W1;
       Drawn++;
   };
   if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X, Y, Height, Depth);
};
//==============================================================================
//==============================================================================
void TNumeric::MinusGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    TNumeric Temp(*this); //работаем с Temp, так как this = const
#ifdef __DEBUG__
    cout<<CodeBasic()<<endl;
#endif
int W, H, D;
string StrPlus = UniCode2UTF8String(MyPlusSign);
string StrMinus = UniCode2UTF8String(MyMinusSign);
    for(size_t i = 0; i < Temp.Operands.size(); i++)
    {
        bool NeedBrackets = false;
        if(CompareOperatorsPriority(Operator, Temp.Operands[i].Operator)>0) NeedBrackets = true;
        vector<size_t> TakenMinuses; //Операнды, у которых минусы уже учтены в знаке перед произведением или дробью
        size_t MinusesCount = 0;

        if(Simplify)
        //Пытаемся вынести минус, эта операция не явлеятся thread-safe
        {
            if(Temp.Operands[i].Operator == OperatorProd || Temp.Operands[i].Operator == OperatorFrac)
            {
                for(size_t j = 0; j < Temp.Operands[i].Operands.size(); j++)
                {
                    if(Temp.Operands[i].Operands[j].Operator == OperatorConst)
                    {
                        if(Temp.Operands[i].Operands[j].K.length() > 0 && Temp.Operands[i].Operands[j].K[0]=='-')
                        {
                            TakenMinuses.push_back(j);
                            Temp.Operands[i].Operands[j].K.erase(Temp.Operands[i].Operands[j].K.begin());
                            MinusesCount++;
                        }
                    }
                }
            }
            if(Temp.Operands[i].Operator == OperatorConst)
                if(Temp.Operands[i].K.length() > 0 && Temp.Operands[i].K[0]=='-')
                {
                    MinusesCount++;
                    Temp.Operands[i].K.erase(Temp.Operands[i].K.begin());
                }
        }
        //рисуем знак
        if(MinusesCount % 2 == 1)
        {
            //минус, и ещё один минус перед вторым членом и далее
            if(i == 0) Width += Canvas->TextWidth(StrMinus);
            else Width += Canvas->TextWidth(StrPlus);
        } else {
            //плюс, перед первым членом знак не нужен
            //начиная со второго - знак минус
            if(i>0) Width += Canvas->TextWidth(StrMinus);
        }

        if(Simplify)
            //если надо было упростить, то рисуем упрощенные слагаемые
        {
            Temp.Operands[i].PrettyGetTextRectangle(Canvas, W, H, D, NeedBrackets, Simplify);
        } else {
            //ничего не упрощали, тогда рисуем исходные слагаемые. При этом поле Active у этих слагаемых меняется и вытаскивается функцией PrettyDrawAtBaseLeft в корень дерева
            Operands[i].PrettyGetTextRectangle(Canvas, W, H, D, NeedBrackets, Simplify);
        }        


        if(Simplify)
        {
        //возвращаем минус обратно
            if(Temp.Operands[i].Operator == OperatorProd || Temp.Operands[i].Operator == OperatorFrac)
            {
                for(size_t j = 0; j < TakenMinuses.size(); j++)
                    Temp.Operands[i].Operands[TakenMinuses[j]].K = string("-")+Temp.Operands[i].Operands[TakenMinuses[j]].K;
            }
            if(Temp.Operands[i].Operator == OperatorConst && (MinusesCount % 2 == 1))
            {
                Temp.Operands[i].K = string("-") + Temp.Operands[i].K;
            }
        };
        Width += W;
        if(H > Height)
            Height = H;
        if(D > Depth)
            Depth = D;
    };
    if(WithBrackets)
    {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
};
void TNumeric::MinusDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(Width);
    TNumeric Temp(*this); //работаем с Temp, так как this = const
#ifdef __DEBUG__
    cout<<CodeBasic()<<endl;
#endif
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
    };
string StrPlus = UniCode2UTF8String(MyPlusSign);
string StrMinus = UniCode2UTF8String(MyMinusSign);
   for(size_t i = 0; i < Temp.Operands.size(); i++)
   {
       int W1, H1, D1;
       bool NeedBrackets = false;
       if(CompareOperatorsPriority(Operator, Temp.Operands[i].Operator)>0) NeedBrackets = true;
       vector<size_t> TakenMinuses; //Операнды, у которых минусы уже учтены в знаке перед произведением или дробью
       size_t MinusesCount = 0 ;

       if(Simplify)
       //Пытаемся вынести минус, эта операция не явлеятся thread-safe
           //todo: скопировать из SumDraw
       {
           if(Temp.Operands[i].Operator == OperatorProd || Temp.Operands[i].Operator == OperatorFrac)
           {
               for(size_t j = 0; j < Temp.Operands[i].Operands.size(); j++)
               {
                   if(Temp.Operands[i].Operands[j].Operator == OperatorConst)
                   {
                       if(Temp.Operands[i].Operands[j].K.length() > 0 && Temp.Operands[i].Operands[j].K[0]=='-')
                       {
                           TakenMinuses.push_back(j);
                           Temp.Operands[i].Operands[j].K.erase(Temp.Operands[i].Operands[j].K.begin());
                           MinusesCount++;
                       }
                   }
               }
           }
           if(Temp.Operands[i].Operator == OperatorConst)
               if(Temp.Operands[i].K.length() > 0 && Temp.Operands[i].K[0]=='-')
               {
                   MinusesCount++;
                   Temp.Operands[i].K.erase(Temp.Operands[i].K.begin());
               }
       }
       //рисуем знак
       if(MinusesCount % 2 == 1)
       {
           //минус, и ещё один минус перед вторым членом и далее
           if(i == 0)
           {
               Canvas->TextOutA(X, Y, StrMinus);
               X = X + Canvas->TextWidth(StrMinus);
           } else  {
               Canvas->TextOutA(X, Y, StrPlus);
               X = X + Canvas->TextWidth(StrPlus);
           }
       }
       else
       {
           //плюс, перед первым членом знак не нужен
           //начиная со второго - знак минус
           if(i>0)
           {
               Canvas->TextOutA(X, Y, StrMinus);
               X = X + Canvas->TextWidth(StrMinus);
           }
       }

       if(Simplify)
           //если надо было упростить, то рисуем упрощенные слагаемые
       {
           Temp.Operands[i].PrettyGetTextRectangle(Canvas, W1, H1, D1, NeedBrackets, Simplify);
           Temp.Operands[i].PrettyDrawAtBaseLeft(Canvas, X, Y, NeedBrackets, Simplify);
       } else {
           //ничего не упрощали, тогда рисуем исходные слагаемые. При этом поле Active у этих слагаемых меняется и вытаскивается функцией PrettyDrawAtBaseLeft в корень дерева
           Operands[i].PrettyGetTextRectangle(Canvas, W1, H1, D1, NeedBrackets, Simplify);
           Operands[i].PrettyDrawAtBaseLeft(Canvas, X, Y, NeedBrackets, Simplify);
       }

       if(Simplify)
       {
       //возвращаем минус обратно
           if(Temp.Operands[i].Operator == OperatorProd || Temp.Operands[i].Operator == OperatorFrac)
           {
               for(size_t j = 0; j < TakenMinuses.size(); j++)
                   Temp.Operands[i].Operands[TakenMinuses[j]].K = string("-")+Temp.Operands[i].Operands[TakenMinuses[j]].K;
           }
           if(Temp.Operands[i].Operator == OperatorConst && (MinusesCount % 2== 1))
           {
               Temp.Operands[i].K = string("-") + Temp.Operands[i].K;
           }
       };
       X = X + W1;
   };
   if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X, Y, Height, Depth);
};
//==============================================================================
//==============================================================================
void TNumeric::UnionGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
char Sign[10];
    UniCode2UTF8(Sign, UnicodeUnion);
    StrGetTextRectangle(Sign, Canvas, Width, Height, Depth, WithBrackets, Simplify);
}

void TNumeric::UnionDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
char Sign[10];
    UniCode2UTF8(Sign, UnicodeUnion);
    StrDraw(Sign, Canvas, X, Y, Width, Height, Depth, NeedBrackets, Simplify);
}

//==============================================================================
//==============================================================================
void TNumeric::PlusMinusGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
char Sign[10];
    UniCode2UTF8(Sign, UnicodePlusMinus);
    StrGetTextRectangle(Sign, Canvas, Width, Height, Depth, WithBrackets, Simplify);
}
void TNumeric::PlusMinusDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
char Sign[10];
    UniCode2UTF8(Sign, UnicodePlusMinus);
    StrDraw(Sign, Canvas, X, Y, Width, Height, Depth, NeedBrackets, Simplify);
}


//==============================================================================
//==============================================================================

void TNumeric::IntersectionGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
char Sign[10];
    UniCode2UTF8(Sign, UnicodeIntersection);
    StrGetTextRectangle(Sign, Canvas, Width, Height, Depth, WithBrackets, Simplify);
}

void TNumeric::IntersectionDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
char Sign[10];
    UniCode2UTF8(Sign, UnicodeIntersection);
    StrDraw(Sign, Canvas, X, Y, Width, Height, Depth, NeedBrackets, Simplify);
}

//==============================================================================
//==============================================================================
void TNumeric::ProdGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
int W, H, D;
vector<size_t> ExcludeOperands;
    if(Simplify)
    {
        for(size_t j = 0; j < Operands.size(); j++)
            if(Operands[j].Operator == OperatorConst && Operands[j].K == "1") ExcludeOperands.push_back(j);
        if(ExcludeOperands.size() == Operands.size() && ExcludeOperands.size() > 0) ExcludeOperands.erase(ExcludeOperands.begin());
    }
char Str[10];
    UniCode2UTF8(Str, UnicodeMiddleDot);
    for(size_t i = 0; i < Operands.size(); i++)
    {
        if(Simplify && find(ExcludeOperands.begin(), ExcludeOperands.end(), i) != ExcludeOperands.end()) continue;
        bool NeedBrackets = false;
        if(CompareOperatorsPriority(Operator, Operands[i].Operator)>0) NeedBrackets = true;
        Operands[i].PrettyGetTextRectangle(Canvas, W, H, D, NeedBrackets, Simplify);
        Width += W;
        if(H > Height)
            Height = H;
        if(D > Depth)
            Depth = D;
        if(i+1 < Operands.size() && !(Simplify && find(ExcludeOperands.begin(), ExcludeOperands.end(), i+1) != ExcludeOperands.end()))
            Width += Canvas->TextWidth(Str);
    };
    if(WithBrackets)
    {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
};
void TNumeric::ProdDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(Width);

   if(NeedBrackets)
   {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
   };
vector<size_t> ExcludeOperands;
    if(Simplify)
    {
        for(size_t j = 0; j < Operands.size(); j++)
            if(Operands[j].Operator == OperatorConst && Operands[j].K == "1") ExcludeOperands.push_back(j);
        if(ExcludeOperands.size() == Operands.size() && ExcludeOperands.size() > 0) ExcludeOperands.erase(ExcludeOperands.begin());
    }
char Str[10];
   UniCode2UTF8(Str, UnicodeMiddleDot);
   for(size_t i = 0; i < Operands.size(); i++)
   {
       if(Simplify && find(ExcludeOperands.begin(), ExcludeOperands.end(), i) != ExcludeOperands.end())
           continue;
       int W1, H1, D1;
       bool NeedBrackets = false;
       if(CompareOperatorsPriority(Operator, Operands[i].Operator)>0) NeedBrackets = true;
       Operands[i].PrettyGetTextRectangle(Canvas, W1, H1, D1, NeedBrackets, Simplify);
       Operands[i].PrettyDrawAtBaseLeft(Canvas, X, Y, NeedBrackets, Simplify);
       X = X + W1;
       if(i+1 < Operands.size() && !(Simplify && find(ExcludeOperands.begin(), ExcludeOperands.end(), i+1) != ExcludeOperands.end()))
       {
           //Canvas->TextOutA(X, Y, "*");
#ifdef __DEBUG__
           int W, H, D;
           W = Canvas->TextWidth(Str);
           H = Canvas->TextHeight(Str);
           D = Canvas->TextDepth(Str);
           Canvas->Line(X, Y, X+W, Y, QPen(Qt::red));
           Canvas->Rectangle(X, Y-H, X+W, Y+D, Qt::red);
#endif
           Canvas->TextOutA(X, Y, Str);
           X = X + Canvas->TextWidth(Str);
       };
   };
   if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X, Y, Height, Depth);
};
//==============================================================================
//==============================================================================
void TNumeric::BelongsToGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
char Sign[10];
    UniCode2UTF8(Sign, UnicodeElementOf);
    StrGetTextRectangle(Sign, Canvas, Width, Height, Depth, WithBrackets, Simplify);
};
void TNumeric::BelongsToDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
char Sign[10];
    UniCode2UTF8(Sign, 0x2208);
    StrDraw(Sign, Canvas, X, Y, Width, Height, Depth, NeedBrackets, Simplify);
};
//==============================================================================
void TNumeric::EqualGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
string str;
        switch(Operator)
        {
            case OperatorLess: str = UniCode2UTF8String(MyLessThanSign); break;
            case OperatorLessOrEqual:  str = UniCode2UTF8String(MyLessOrEqualSign); break;
            case OperatorGreater: str = UniCode2UTF8String(MyGreaterThanSign); break;
            case OperatorGreaterOrEqual: str = UniCode2UTF8String(MyGreaterOrEqualSign); break;
            case OperatorEqual:   str = UniCode2UTF8String(MyEqualSign); break;
            default: str = "?";
        }        
        StrGetTextRectangle(str.c_str(), Canvas, Width, Height, Depth, WithBrackets, Simplify);
};
void TNumeric::EqualDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
string str;
        switch(Operator)
        {
            case OperatorLess: str = UniCode2UTF8String(MyLessThanSign); break;
            case OperatorLessOrEqual:  str = UniCode2UTF8String(MyLessOrEqualSign); break;
            case OperatorGreater: str = UniCode2UTF8String(MyGreaterThanSign); break;
            case OperatorGreaterOrEqual: str = UniCode2UTF8String(MyGreaterOrEqualSign); break;
            case OperatorEqual:   str = UniCode2UTF8String(MyEqualSign); break;
            default: str = "?";
        }
        StrDraw(str.c_str(), Canvas, X, Y, Width, Height, Depth, NeedBrackets, Simplify);
};
//==============================================================================
//==============================================================================
void TNumeric::PowGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
int W1, H1, D1;
int W2, H2, D2;
    Width = Height = Depth = 0;
    Operands[0].PrettyGetTextRectangle(Canvas, W1, H1, D1, true, Simplify);
    Height = H1;
    Depth = D1;
    Width = W1;
QFont Font = DecreaseFont(Canvas);
    Operands[1].PrettyGetTextRectangle(Canvas, W2, H2, D2, true, Simplify);
    Canvas->Font = Font;
    if(Height < H1/2 + H2 + D2)
        Height = H1/2 + H2 + D2;
    if(Depth < D2 - H1/2)
        Depth = D2 - H1/2;
    Width += W2;
    if(WithBrackets)
    {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
};
void TNumeric::PowDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(Width);
int W1, H1, D1;
int W2, H2, D2;
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
    };
    Operands[0].PrettyGetTextRectangle(Canvas, W1, H1, D1, true, Simplify);
    Operands[0].PrettyDrawAtBaseLeft(Canvas, X, Y, true, Simplify);
    X += W1;
    QFont Font = DecreaseFont(Canvas);
    Operands[1].PrettyGetTextRectangle(Canvas, W2, H2, D2, true, Simplify);
    Operands[1].PrettyDrawAtBaseLeft(Canvas, X, Y - H1/2 - D2, true, Simplify);
    Canvas->Font = Font;
    X+=W2;

    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X , Y, Height, Depth);
};
//==============================================================================
//==============================================================================
void TNumeric::AbsGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    Q_UNUSED(WithBrackets);
int W1, H1, D1;
    Width = Height = Depth = 0;
    Operands[0].PrettyGetTextRectangle(Canvas, W1, H1, D1, false, Simplify);
    Height = H1;
    Depth = D1;
    Width = W1;
    Width = Width + 2*GetBracketWidth("|", Canvas, Height, Depth);
};
void TNumeric::AbsDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(NeedBrackets);
    Q_UNUSED(Width);
int W1, H1, D1;
    DrawBracket("|", Canvas, X, Y, Height, Depth);
    X = X + GetBracketWidth("|", Canvas, Height, Depth);

    Operands[0].PrettyGetTextRectangle(Canvas, W1, H1, D1, false, Simplify);
    Operands[0].PrettyDrawAtBaseLeft(Canvas, X, Y, false, Simplify);
    X += W1;

    DrawBracket("|", Canvas, X , Y, Height, Depth);
};
//==============================================================================
//==============================================================================
void TNumeric::DerivGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
int W1, H1, D1;
int W2, H2, D2;
    Width = Height = Depth = 0;
    Operands[0].PrettyGetTextRectangle(Canvas, W1, H1, D1, true, Simplify);
    Height = H1;
    Depth = D1;
    Width = W1;

    H2 = Canvas->TextHeight("'");
    D2 = Canvas->TextDepth("'");
    W2 = Canvas->TextWidth("'");

    Width += W2;
    if(WithBrackets)
    {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
};
void TNumeric::DerivDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(Width);
int W1, H1, D1;
int W2, H2, D2;
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
    };
    Operands[0].PrettyGetTextRectangle(Canvas, W1, H1, D1, true, Simplify);
    Operands[0].PrettyDrawAtBaseLeft(Canvas, X, Y, true, Simplify);
    X+=W1;

    H2 = Canvas->TextHeight("'");
    D2 = Canvas->TextDepth("'");
    W2 = Canvas->TextWidth("'");
    Canvas->TextOutA(X, Y - H1 + H2, "'");
    X += W2;


    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X , Y, Height, Depth);
};
//==============================================================================
//==============================================================================
void TNumeric::SubIndexGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
int W, H, D;
    Width = Height = Depth = 0;
    Operands[0].PrettyGetTextRectangle(Canvas, W, H, D, true, Simplify);
    Height = H;
    Depth = D;
    Width = W;
    QFont Font = DecreaseFont(Canvas);
    Operands[1].PrettyGetTextRectangle(Canvas, W, H, D, true, Simplify);    
    Canvas->Font = Font;
    if(H + D > 2*Depth)
        Depth = (H + D)/2 + 1;

    /*int IndexBaseLine = - Height/2 + H;
    if(Depth > IndexBaseLine)
        IndexBaseLine = Depth - H/2;
    if(IndexBaseLine + D  > Depth)
        Depth = IndexBaseLine + D; */
    Width += W;
    if(WithBrackets)
    {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
};
void TNumeric::SubIndexDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(Width);
int W1, H1, D1;
int W2, H2, D2;
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
    };
    Operands[0].PrettyGetTextRectangle(Canvas, W1, H1, D1, true, Simplify);
    Operands[0].PrettyDrawAtBaseLeft(Canvas, X, Y, true, Simplify);
    X += W1;

    QFont Font = DecreaseFont(Canvas);
    Operands[1].PrettyGetTextRectangle(Canvas, W2, H2, D2, true, Simplify);
/*    int IndexBaseLine = - H1/2 + H2;
    if(D1 > IndexBaseLine)
        IndexBaseLine = D1 - H2/2;
    Operands[1].PrettyDrawAtBaseLeft(Canvas, X, Y + IndexBaseLine, true, Simplify);*/
    Operands[1].PrettyDrawAtBaseLeft(Canvas, X, Y + (H2 - D2)/2, true, Simplify);
    X+=W2;
    Canvas->Font = Font;

    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X , Y, Height, Depth);
};
//==============================================================================
//==============================================================================
void TNumeric::SqrtGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
int W, H, D;
    Operands[0].PrettyGetTextRectangle(Canvas, W, H, D, false, Simplify);
    Width = W;
    Height = H;
    Width = Width + 10;
    Height = Height + 10;
    Depth = D;
    if(WithBrackets)
    {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) +GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
};
void TNumeric::SqrtDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(Width);
int W, H, D;
   if(NeedBrackets)
   {
       DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
       X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
   };
   Operands[0].PrettyGetTextRectangle(Canvas, W, H, D, false, Simplify);
   Operands[0].PrettyDrawAtBaseLeft(Canvas, X+10, Y, false, Simplify);
   QColor Color = Canvas->Pen.color();
   Canvas->Line(X, Y, X+2, Y, Color, 1);
   Canvas->Line(X+2, Y, X+5, Y + Depth, Color, 2);
   Canvas->Line(X+5, Y + Depth, X+9, Y - Height + 2, Color, 1);
   Canvas->Line(X+9, Y - Height + 2, X + 10 + W, Y - Height + 2, Color, 1);
   Canvas->Line(X + 10 + W, Y - Height + 2, X + 10 +W, Y - Height + 5, Color, 1);

   if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X+10+W, Y, Height, Depth);
};
//==============================================================================
void  TNumeric::LinesGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, int LeftMargin, bool Simplify) const
//одна или несколько строк, объединенных оператором совокупности или пересечения
{
   Width = 0;
   Height = 0;
   Depth = 0;

   int InterLineSpacing = Canvas->TextHeight("A")/3;
   for(size_t i = 0; i < Operands.size(); i++)
   {
      int W, H, D;
      Operands[i].PrettyGetTextRectangle(Canvas, W, H, D, false, Simplify);
      if(W > Width) Width = W;
      Height += D+H;
      if(i+1 < Operands.size())
          Height += InterLineSpacing;
   };
   Depth = Height/2;
   Height = Height - Depth;
   if(WithBrackets)
   {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
   };

   Width += LeftMargin;
};
void TNumeric::LinesDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, int LeftMargin, bool Simplify) const
{
    X += LeftMargin;
    if(NeedBrackets)
    {
       DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
           X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
   };
   int InterLineSpacing = Canvas->TextHeight("A")/3;
   for(size_t i = 0; i < Operands.size(); i++)
   {
      int W, H, D;
      Operands[i].PrettyGetTextRectangle(Canvas, W, H, D, false, Simplify);
      Operands[i].PrettyDrawAtBaseLeft(Canvas, X, Y-Height+H, false, Simplify);
      Y += H+D;
      if(i+1 < Operands.size())
          Y += InterLineSpacing;
   };
   if(NeedBrackets)
       DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X + Width - GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth), Y, Height, Depth);
};

void TNumeric::EqSystemGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
   LinesGetTextRectangle(Canvas, Width, Height, Depth, WithBrackets, 0, Simplify);
   Width += GetBracketWidth(UniCode2UTF8String(MyLeftCurlyBracket), Canvas, Height*1, Depth);
};
void TNumeric::EqSystemDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
string bracket = UniCode2UTF8String(MyLeftCurlyBracket);
    DrawBracket(bracket, Canvas, X, Y, Height, Depth);
int BW = GetBracketWidth(bracket, Canvas, Height, Depth);
    X+=BW;
    LinesDraw(Canvas, X, Y, Width, Height, Depth, NeedBrackets, 0, Simplify);
}
void TNumeric::EqSetGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{    
   LinesGetTextRectangle(Canvas, Width, Height, Depth, WithBrackets, 0, Simplify);
   Width += GetBracketWidth(UniCode2UTF8String(MyLeftCurlyBracket), Canvas, Height, Depth);
};
void TNumeric::EqSetDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
string bracket = UniCode2UTF8String(MyLeftCurlyBracket);
    DrawBracket(bracket, Canvas, X, Y, Height, Depth);
int BW = GetBracketWidth(bracket, Canvas, Height, Depth);
    X+=BW;
    LinesDraw(Canvas, X, Y, Width, Height, Depth, NeedBrackets, 0, Simplify);
}
void TNumeric::IntegralGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
int W1, H1, D1;
int W2, H2, D2;
int dW, dH, dD;
    bool Brackets = false;
    if(GetOperatorPriority(Operands[0].Operator) < GetOperatorPriority(OperatorIntegral)) Brackets = true;    

    Operands[0].PrettyGetTextRectangle(Canvas, W1, H1, D1, Brackets, Simplify);
    Operands[1].PrettyGetTextRectangle(Canvas, W2, H2, D2, false, Simplify);
    TNumeric d("d");
    d.PrettyGetTextRectangle(Canvas, dW, dH, dD, false, false);
    Width = W1 + W2 + dW;
    Height = max(max(H1, H2), dH);
    Depth = max(max(D1, D2), dD);
    Height += Height/2;
    Depth += Depth/2;
    Width += GetBracketWidth(UniCode2UTF8String(UnicodeIntegral), Canvas, Height, Depth);

    if(WithBrackets)
    {
         Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };

}

void TNumeric::IntegralDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
int W1, H1, D1;
int W2, H2, D2;
int dW, dH, dD;
TNumeric d("d");    
    if(NeedBrackets)
    {
       DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
       X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
    };

    bool Brackets = false;
    if(GetOperatorPriority(Operands[0].Operator) < GetOperatorPriority(OperatorIntegral)) Brackets = true;
    Operands[0].PrettyGetTextRectangle(Canvas, W1, H1, D1, Brackets, Simplify);
    Operands[1].PrettyGetTextRectangle(Canvas, W2, H2, D2, false, Simplify);
    d.PrettyGetTextRectangle(Canvas, dW, dH, dD, false, false);


    DrawBracket(UniCode2UTF8String(UnicodeIntegral), Canvas, X, Y, Height, Depth);
    X += GetBracketWidth(UniCode2UTF8String(UnicodeIntegral), Canvas, Height, Depth);

    Operands[0].PrettyDrawAtBaseLeft(Canvas, X, Y, Brackets, Simplify);
    X += W1;

    d.PrettyDrawAtBaseLeft(Canvas, X, Y, false, false);
    X += dW;

    Operands[1].PrettyDrawAtBaseLeft(Canvas, X, Y, false, Simplify);
    X += W2;

    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X + Width - GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth), Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
}

//===========================================================================
string TNumeric::CodeBasic() const
{
string Str;
size_t i;
    switch(Operator)
    {
        case OperatorConst:
            if(K == "-\\infty") Str="(-inf)"; else
            if(K == "\\infty" || K == "+\\infty") Str="(inf)"; else
            Str = K;
        break;
        case OperatorEqual:  Str = Operands[0].CodeBasic()+"="+Operands[1].CodeBasic();
        break;
        case OperatorSum:
            Str = '(';
            for(i = 0; i < Operands.size(); i++)
            {
                Str += Operands[i].CodeBasic();
                if(i+1 < Operands.size())
                Str += '+';
            };
            Str += ')';
            return Str;
            break;
        case OperatorMinus:
            Str = string("(")+Operands[0].CodeBasic()+"-";
            for(i = 1; i < Operands.size(); i++)
            {
                Str += Operands[i].CodeBasic();
                if(i+1 < Operands.size())
                    Str += '-';
            };
            Str += ')';
            return Str;
            break;
        case OperatorProd:
            Str = '(';
            for(i = 0; i < Operands.size(); i++)
            {
                Str += Operands[i].CodeBasic();
                if(i+1 < Operands.size())
                Str += '*';
            };
            Str += ')';
            return Str;
            break;
        case OperatorFrac: Str = string("(")+Operands[0].CodeBasic() + ")/(" + Operands[1].CodeBasic() + ")";
        break;
        case OperatorSqrt: Str = string("((")+Operands[0].CodeBasic() + ")^(1/2))";
        break;
        case OperatorPow: Str = string("((")+Operands[0].CodeBasic() + ")^(" + Operands[1].CodeBasic() + "))";
        break;
        case OperatorSubIndex: Str = Operands[0].CodeBasic()+"_"+Operands[1].CodeBasic(); break;
        case OperatorIntegral: Str = string("integral(") + Operands[0].CodeBasic() +") d" + Operands[1].CodeBasic(); break;
        case OperatorExp: Str = string("exp(") + Operands[0].CodeBasic() +")"; break;
        case OperatorSh: Str = string("sh(") + Operands[0].CodeBasic() +")"; break;
        case OperatorCh: Str = string("ch(") + Operands[0].CodeBasic() +")"; break;
        case OperatorCos: Str = string("cos(") + Operands[0].CodeBasic() +")"; break;
        case OperatorSin: Str = string("sin(") + Operands[0].CodeBasic() +")"; break;
        case OperatorTg: Str = string("tg(") + Operands[0].CodeBasic() +")"; break;
        case OperatorArccos: Str = string("acos(") + Operands[0].CodeBasic() +")"; break;
        case OperatorArcsin: Str = string("asin(") + Operands[0].CodeBasic() +")"; break;
        case OperatorArctg: Str = string("atg(") + Operands[0].CodeBasic() +")"; break;
        case OperatorCtg: Str = string("ctg(") + Operands[0].CodeBasic() +")"; break;
        case OperatorLg: Str = string("lg(") + Operands[0].CodeBasic() +")"; break;
        case OperatorLn: Str = string("ln(") + Operands[0].CodeBasic() +")"; break;
        case OperatorLog: Str = string("log(")+Operands[0].CodeBasic()+", "+Operands[1].CodeBasic()+")"; break;
        default: Str = string("f(");
            for(size_t i = 0; i < Operands.size(); i++)
                if(i+1<Operands.size()) Str = Str +Operands[0].CodeBasic()+", ";
                else Str = Str +Operands[0].CodeBasic()+")";
                //throw "TNumeric::CodeBasic(): function not fully realized yet";
    };
    return Str;
};
//==============================================================================
string DeleteExternalBrackets(string Str)
{
   //Удаляем внешние скобки
char* p = (char*)Str.c_str();
int BracketLevel = 0;
bool External = true; //true, если найденная пара скобок StartIndex EndIndex является внешними скобками - за пределами их нет арифеметических операций
size_t StartIndex, EndIndex; //начало и конец результирующей подстроки, не включая последений символ
//то есть Result = [StartIndex, EndIndex)
   StartIndex = 0;
   EndIndex = Str.length();
   while(*p)
   {
      if(*p == '(')
      {
         if(BracketLevel == 0)
            StartIndex = p - Str.c_str()+1;
         BracketLevel ++;
      };
      if(*p == ')')
      {
         if(BracketLevel == 1)
            EndIndex = p - Str.c_str();
         BracketLevel --;
      };
      if(*p != '(' && *p != ')' && *p != ' ')
          //найдено или цифра, или буква или арифметическое действие
      {
          if(BracketLevel == 0)
              //найдено за пределами самых внешних скобок
              External = false; //значит, эти скобки уже не внешние, их удалять не будем
      }
      p++;
   };
   if(StartIndex == 1 && External)
    return Str.substr(StartIndex, EndIndex-StartIndex);
   else return Str;
};
//==============================================================================
int DecodeFunctionName(const char* str)
{
    if(strcmp(str, "exp") == 0) return OperatorExp;
    if(strcmp(str, "sh") == 0) return OperatorSh;
    if(strcmp(str, "ch") == 0) return OperatorCh;
    if(strcmp(str, "log") == 0) return OperatorLog;
    if(strcmp(str, "ln") == 0) return OperatorLn;
    if(strcmp(str, "lg") == 0) return OperatorLg;
    if(strcmp(str, "sin") == 0) return OperatorSin;
    if(strcmp(str, "cos") == 0) return OperatorCos;
    if(strcmp(str, "tg") == 0) return OperatorTg;
    if(strcmp(str, "ctg") == 0) return OperatorCtg;
    if(strcmp(str, "asin") == 0) return OperatorSin;
    if(strcmp(str, "acos") == 0) return OperatorArccos;
    if(strcmp(str, "atg") == 0) return OperatorArctg;
    if(strcmp(str, "cos") == 0) return OperatorCos;
    ostringstream errstr;
    errstr<<"DecodeFunctionName: Unknown function name: "<<string(str);
    throw errstr.str().c_str();
}

bool CheckFunctionTemplate(const char* str, TNumeric* Res)
//string must be like function_name(arg1, arg2, ..., argn)
//returns
{
    const char *leftbracket = 0;
    const char *rightbracket = 0;
    const char* p = str;
    if(*p >= '0' && *p <= '9') return false;
    while(*p && *p != '(')
    {
        p++;
        if(*p == 0) break;
        if(*p == '+' || *p =='-' || *p=='/' || *p=='*' || *p=='^')
            return false;
    };
    if(*p == 0) return false;
    leftbracket = p;
    int bracketlevel = 0;
    while(*p)
    {
        if(*p == '(') bracketlevel++;
        if(*p == ')') bracketlevel--;
        if(*p == '(' && bracketlevel == 1) leftbracket = p;
        if(*p == ')' && bracketlevel == 0)
        {
            rightbracket = p;
            if(*(p+1) != 0) //the last character in a string should be ')'
                return false;
        };
        p++;
    };
    if(bracketlevel != 0)
        throw "CheckFunctionTemplate: unbalanced brackets";
    string FunctionName(str);
    FunctionName.erase(leftbracket - str); //начиная от скобки '(' все удаляем
    //Выделяем аргументы
    bracketlevel = 0;
    const char* q = leftbracket + 1;
    Res->Operator = DecodeFunctionName(FunctionName.c_str());
    string Str(str);
    for(p = leftbracket+1; p < rightbracket; p++)
    {
        if(*p == '(') bracketlevel++;
        if(*p == ')') bracketlevel--;
        if(*p == ',' && bracketlevel==0)
        {
            TNumeric Op;
            string S = Str.substr(q - str, p-q);
            Op.Assign(S.c_str());
            q = p + 1;
            Res->OperandsPushback(Op);
        }
    };
    TNumeric Op;
    string S = Str.substr(q - str, p-q);
    Op.Assign(S.c_str());
    q = p + 1;
    Res->OperandsPushback(Op);
    return true;
}

void TNumeric::Assign(const char* str)
{    
   char *p;
   p = new char[strlen(str)+1];
   strcpy(p, str);
   Assign(p);
   delete p;
};

void TNumeric::Assign(char* str)
{    
string Str = str;
   Str = DeleteExternalBrackets(Str);
   while(1)
   {
   	size_t pos = Str.find("**");
      if(pos == string::npos) break;
      Str.replace(pos, 2, "^");
   };
   while(1)
   {
   	size_t pos = Str.find(" ");
      if(pos == string::npos) break;
      Str.replace(pos, 1, "");
   };
   TNumeric Res;
   if(CheckFunctionTemplate(Str.c_str(), &Res))
   {
       this->Assign(Res);
       return;
   }

int OpCode = -1;
int OpPriority = -1;
int BracketLevel = -1;
int OpPos = -1;

char *p;
int i =0;
int CurBracketLevel = 0;
    p = (char*)Str.c_str();    
    while(*p)
    {
        int CurOpCode = -1;
        if(*p == '(') CurBracketLevel++;
        if(*p == ')') CurBracketLevel--;

        if(*p == '+') CurOpCode = OperatorSum;
        if(*p == '-')
            if(i>0 && *(p-1)!='(') //чтобы исключить случай унарного минуса, когда "-3" преобразуется в "(0)-(3)"="0-3"
                CurOpCode = OperatorMinus;
        if(*p == '/') CurOpCode = OperatorFrac;
        if(*p == '^') CurOpCode = OperatorPow;
        if(*p == '*') CurOpCode = OperatorProd;

        //случай 2^2+2
        int CurOpPrior;
        if(CurOpCode != -1) CurOpPrior = GetOperatorPriority(CurOpCode);

        if(CurOpCode != -1)
            if(OpCode==-1 || (CurBracketLevel < BracketLevel || (CurBracketLevel == BracketLevel && (CurOpPrior < OpPriority || (CurOpPrior == OpPriority && i>OpPos)))))
            //Если выполнено хотя бы одно условие
            //1. OpCode == -1 - это первая найденная операция в выражении
            //2. CurBracketLevel < BracketLevel - эта операция находится во внешних скобках по отношению к найденной
            //3. CurBracketLevel == BracketLevel - в тех же скобках, но при этом должно выполнять одно из условий
                //3a. CurOpPrior < OpPriority - операция имеет меньший приоритет, чем найденная
                //3b. CurOpPrior = OpPriority - операция имеет тот же приоритет, но при этом нас интересует самая правая операция (i > OpPos)
                //    (Последнее условие, чтобы правильно обрабатывать a-b-c = (a-b) -c != a - (b - c)
            {
                BracketLevel = CurBracketLevel;
                OpPriority = CurOpPrior;
                OpCode = CurOpCode;
                OpPos = i;
            };
        p++;
        i++;
   };
   if(OpCode == -1)
   {
        if(Str.empty())
        {
            TNumeric Res("0");
            this->Assign(Res);
        } else {
            TNumeric Res(Str.c_str());            
            this->Assign(Res);
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

        switch(OpCode)
        {
            case OperatorSum: Res = N1 + N2;
            break;
            case OperatorMinus: Res = N1 - N2;
            break;
            case OperatorProd: Res = N1 * N2;
            break;
            case OperatorPow: Res = N1^N2;
            break;
            case OperatorFrac: Res = N1/N2;
            break;
            default:
            break;
        };
        this->Assign(Res);
   };
   SimplifyPresentation();
};
void TNumeric::SimplifyPresentation()
{
    for(size_t i = 0; i < Operands.size(); i++)
        Operands[i].SimplifyPresentation();
    switch(Operator)
    {
        case OperatorConst:
        break;
        case OperatorEqual:
        break;
        case OperatorSum: //(a+b)+c = a + b + c
    {
            size_t i = 0;
            while(i<Operands.size())
            {
                if(Operands[i].Operator == OperatorSum)
                {
                    for(size_t j = 0; j < Operands[i].Operands.size(); j++)
                    {
                        Operands.insert(Operands.begin()+i, Operands[i].Operands[j]);
                        i++;
                    };
                    Operands.erase(Operands.begin() +  i);
                } else i++;
            };
    }
        break;
        case OperatorProd: //a*(b*c) = a*b*c
    {
            size_t i = 0;
            while(i<Operands.size())
            {
                if(Operands[i].Operator == OperatorProd)
                {
                    for(size_t j = 0; j < Operands[i].Operands.size(); j++)
                    {
                        Operands.insert(Operands.begin()+i, Operands[i].Operands[j]);
                        i++;
                    };
                    Operands.erase(Operands.begin() +  i);
                } else i++;
            };
    }
        break;
        case OperatorFrac:
        break;
        case OperatorSqrt:
        break;
        case OperatorPow:
        //1. упрощаем, если показатель степени равен 1/2
            TNumeric *N = &Operands[1];
            if(N->Operator == OperatorFrac)
                if(N->Operands[0].K == "1" && N->Operands[1].K == "2")
                {
                    TNumeric N2 = Operands[0];
                   Operands.erase(Operands.end()); //удаляем второй (он же последний) элемент
                    Operator = OperatorSqrt;
                };            
      break;
   };
};


void TNumeric::EliminateFunctions(size_t &StartID, map<string, TNumeric>& Map)
{
    bool NeedToEliminate = true;
    switch(Operator)
    {
        case OperatorConst:
        //проводим переобзначение "\var" -> "an"
            if(!(K.length() > 0 && K[0] == '\\')) NeedToEliminate = false;
            break;
        //ниже список функций, реализованных в mathomatic, которые можно не упрощать        
        case OperatorSum:
        case OperatorMinus:
        case OperatorProd:
        case OperatorFrac:
        case OperatorSqrt:
        case OperatorPow:
        case OperatorEqual:
        NeedToEliminate = false; break;
    }
    if(NeedToEliminate)
    {
    //проверяем, вдруг эта функция уже есть и как-то обозначена
        map<string, TNumeric>::iterator It;
        It = Map.begin();
        while(It!=Map.end())
        {
            if(It->second.IsEqual(*this))
            //нашли, используем уже существующее обозначение
            {
                Operator = OperatorConst;
                K = It->first;
                break;
            }
            It++;
        };
        if(It == Map.end())
        {
            //не нашли, придумываем новое обозначение
            pair<string, TNumeric> NewItem;
            NewItem.second = *this;
            for(size_t i = 0;  i<NewItem.second.Operands.size(); i++)
                NewItem.second.Operands[i] = NewItem.second.Operands[i].Simplify();

            char Buf[128];
            sprintf(Buf, "a%d", (int)StartID++);
            Operator = OperatorConst;

            K = Buf;
            NewItem.first = Buf;
            Map.insert(NewItem);
        }
    }
    //упрощаем аргументы
    for(size_t i = 0; i < Operands.size(); i++)
        Operands[i].EliminateFunctions(StartID, Map);
}

void TNumeric::RestoreFunctions(const map<string, TNumeric>& V)
{
    switch(Operator)
    {
        case OperatorConst:
        {
            map<string, TNumeric>::const_iterator it = V.find(K);
            if(it == V.end())
                break;
            else
            {
                *this = it->second;
            }
        }; break;
        default:
        {
            for(size_t i = 0; i < Operands.size(); i++)
                Operands[i].RestoreFunctions(V);
        };
    }

}

bool TNumeric::GetRational(int &Nominator, int &Denominator)
{
    if(CanCalculate() == false) return false;
    if(Operator == OperatorConst)
    {
        if(IsInteger(&Nominator))
        {
            Denominator = 1;
            return true;
        }
    };
    if(Operator == OperatorFrac)
    {
        if(Operands[0].IsInteger(&Nominator) && Operands[1].IsInteger(&Denominator))
        {
            return true;
        }
    }
    return false;
}


TNumeric TNumeric::SimplifyTrig() const
{
bool OperatorOk = false;
    switch(Operator)
    {
        case OperatorSin:
        case OperatorCos:
        case OperatorTg:
        case OperatorCtg: OperatorOk = true;
    };
    if(!OperatorOk) return *this;
    TNumeric Res = *this;
    Res.Operands[0] = Res.Operands[0].Simplify();
//double Val = Operands[0].Calculate();
    //if(Val == 0)
    if(Res.Operands[0] == TNumeric(0))
    {        
        switch(Operator)
        {
            case OperatorSin: return TNumeric("0");
            case OperatorCos: return TNumeric("1");
            case OperatorTg: return TNumeric("0");
            case OperatorCtg: return MakeCtg(TNumeric("0"));
        };
    } else
    if(Res.Operands[0] == NumPi)
    {
        switch(Operator)
        {
            case OperatorSin: return TNumeric("0");
            case OperatorCos: return TNumeric("-1");
            case OperatorTg: return TNumeric("0");
            case OperatorCtg: return MakeCtg(TNumeric(NumPi));
        };
    } else
    if(Res.Operands[0] == NumPi2)
    {
        switch(Operator)
        {
            case OperatorSin: return TNumeric("1");
            case OperatorCos: return TNumeric("0");
            case OperatorTg: return MakeTg(TNumeric(NumPi2));
            case OperatorCtg: return TNumeric("0");
        };
    } else
    if(Res.Operands[0] == NumPi3)
    {
//todo
    } else
    if(Res.Operands[0] == Num2Pi3)
    {
//todo
    };
    //todo Нужно  сделать попытку привести угол к отрезку [0, pi/2], а потом с учетом знака вычислить ответ
    return Res;
}

TNumeric TNumeric::SimplifyInverseTrig() const
{
bool OperatorOk = false;
    switch(Operator)
    {
        case OperatorArcsin:
        case OperatorArccos:
        case OperatorArctg: OperatorOk = true;
    };
    if(!OperatorOk) return *this;

    TNumeric Res = *this;
    Res.Operands[0] = Res.Operands[0].Simplify();
//double Val = Operands[0].Calculate();

    if(Res.Operands[0] == TNumeric(0))
    {
        switch(Operator)
        {
            case OperatorArcsin: return TNumeric("0");
            case OperatorArccos: return NumPi2;
            case OperatorArctg: return TNumeric("0");
        };
    } else
    if(Res.Operands[0] == TNumeric(1))
    {
        switch(Operator)
        {
            case OperatorArcsin: return NumPi2;
            case OperatorArccos: return TNumeric("0");
            case OperatorArctg: return NumPi4;
        };
    } else
    if(Res.Operands[0] == TNumeric(-1))
    {
        switch(Operator)
        {
            case OperatorArcsin: return -NumPi2;
            case OperatorArccos: return NumPi;
            case OperatorArctg: return -NumPi4;
        };
    } else
    if(Res.Operands[0] == MakeFrac(TNumeric(1), TNumeric(2)))
    {
        switch(Operator)
        {
            case OperatorArcsin: return NumPi6;
            case OperatorArccos: return NumPi3;
            case OperatorArctg: return MakeArctg(MakeFrac(TNumeric("1"), TNumeric("2")));
        };
    } else
    if(Res.Operands[0] == -MakeFrac(TNumeric(1), TNumeric(2)))
    {
        switch(Operator)
        {
            case OperatorArcsin: return -NumPi6;
            case OperatorArccos: return Num2Pi3;
            case OperatorArctg: return -MakeArctg(MakeFrac(TNumeric("1"), TNumeric("2")));
        };
    } else
    if(Res.Operands[0] == MakeFrac(MakeSqrt(TNumeric("3")), TNumeric(2)))
    {
        switch(Operator)
        {
            case OperatorArcsin: return NumPi3;
            case OperatorArccos: return NumPi6;
            case OperatorArctg: return MakeArctg(MakeFrac(MakeSqrt(TNumeric("3")), TNumeric(2)));
        };
    } else
    if(Res.Operands[0] == -MakeFrac(MakeSqrt(TNumeric("3")), TNumeric(2)))
    {
        switch(Operator)
        {
            case OperatorArcsin: return -NumPi3;
            case OperatorArccos: return Num5Pi6;
            case OperatorArctg: return -MakeArctg(MakeFrac(MakeSqrt(TNumeric("3")), TNumeric(2)));
        };
    } else
    if(Res.Operands[0] == MakeFrac(TNumeric(1), MakeSqrt(TNumeric(2))))
    {
        switch(Operator)
        {
            case OperatorArcsin: return NumPi4;
            case OperatorArccos: return NumPi4;
            case OperatorArctg: return MakeArctg(MakeFrac(TNumeric("1"), MakeSqrt(TNumeric("2"))));
        };
    } else
    if(Res.Operands[0] == MakeFrac(TNumeric(1), MakeSqrt(TNumeric(2))))
    {
        switch(Operator)
        {
            case OperatorArcsin: return -NumPi4;
            case OperatorArccos: return Num3Pi4;
            case OperatorArctg: return -MakeArctg(MakeFrac(TNumeric("1"), MakeSqrt(TNumeric("2"))));
        };
    };
    return Res;
}

TNumeric TNumeric::SimplifyLog() const
{
    if(Operator != OperatorLog) return *this;
    TNumeric A = Operands[0].Simplify();
    TNumeric B = Operands[1].Simplify();    
    if(A.CanCalculate() && B.CanCalculate())
    {
        TNumeric Res;
        double a = A.Calculate();
        double b = B.Calculate();
        if(a<0 || b<0 || b == 1)
        {
            return MakeLog(A, B);
        };
        if(a == b)
            return TNumeric(1);
        if(a == 1) return TNumeric(0);
        int AN, AD, BN, BD;
        Res.Operator = OperatorLog;
        int N, D; //коэффициент перед логарифмом
        if(A.GetRational(AN, AD))
        {
            int AN1, AD1;
            if(AN*AD<0) throw "TNumeric::SimplifyLog: AN*AD<0";
            if(AN<0)AN = -AN;
            if(AD<0)AD = -AD;
            CheckCommonPower(AN, AD, N, AN1, AD1);
            if(N>1) A = (TNumeric(AN1)/TNumeric(AD1)).Simplify();
        } else N = 1;
        if(B.GetRational(BN, BD))
        {
            int BN1, BD1;
            if(BN*BD<0) throw "TNumeric::SimplifyLog: AN*AD<0";
            if(BN<0)BN = -BN;
            if(BD<0)BD = -BD;
            CheckCommonPower(BN, BD, D, BN1, BD1);
            if(D>1) B = (TNumeric(BN1)/TNumeric(BD1)).Simplify();
        } else D = 1;
        Res.OperandsPushback(A);
        Res.OperandsPushback(B);
        if(N == 1 && D!=1) Res = MakeFrac(Res, TNumeric(D));
        if(N != 1 && D==1) Res = MakeProd(TNumeric(N), Res);
        if(N != 1 && D!=1) Res = MakeProd(MakeFrac(TNumeric(N), TNumeric(D)), Res);
        return Res;
    } else return *this;    
}

void TNumeric::SimplifyFunctions()
{
    switch(Operator)
    {
        case OperatorLog: *this = SimplifyLog(); break;
        /*case OperatorLn:
        case OperatorLg:*/
        case OperatorSin:
        case OperatorCos:
        case OperatorTg:
        case OperatorCtg: *this = SimplifyTrig(); break;
        case OperatorArcsin:
        case OperatorArccos:
        case OperatorArctg:
        *this = SimplifyInverseTrig(); break;
/*        default:
            for(size_t i = 0; i < Operands.size(); i++)
                Operands[i].SimplifyFunctions();
            break;*/
        case OperatorLn:
        case OperatorLg:
            if(Operands[0] == TNumeric(1))
                *this = TNumeric(0);
        break;
    }
    for(size_t i = 0; i < Operands.size(); i++)
        Operands[i] = Operands[i].Simplify();
}


TNumeric TNumeric::MathoCmd(const string& Cmd) const
{
TNumeric Res = *this;
//ходим по всему дереву и упрощаем все функции

    //1.Устраняем все нереализованные в mathomatic функции
    size_t StartID = 0;
    map<string, TNumeric> M;
    Res.EliminateFunctions(StartID, M);
    //2. вызываем mathomatic
    string SourceCode = Res.CodeBasic();
#ifdef __DEBUG__
                cout<<SourceCode<<"->";
                cout.flush();
#endif
static string VarName = "vartosimplify";
    SourceCode = VarName +" = " + SourceCode;
    string ResCode;
    ResCode = math_process(SourceCode.c_str());
    ResCode = math_process(Cmd.c_str());
    math_clear_all();
    //удаляем "vartosimplify = "
    size_t pos = ResCode.find(VarName + " = ");
    if(pos == string::npos)
        throw "TNumeric::Simplify: Unknown answer format from math library. Can't simplify";
    ResCode = ResCode.substr(pos + VarName.length() + 3);
#ifdef __DEBUG__
    cout<<ResCode<<endl;
#endif
    Res.Assign((char*)ResCode.c_str());
    //3. восстанавливаем функции
    Res.RestoreFunctions(M);
    return Res;
}

TNumeric TNumeric::Simplify() const
{    
    if(Operator == OperatorConst) return *this;
TNumeric Res = *this;    
    Res.SimplifyFunctions();    
    if(Operator == OperatorEqual)
    {
        TNumeric A = Operands[0].MathoCmd("simplify");
        TNumeric B = Operands[1].MathoCmd("simplify");
        return MakeEquality(A, B);
    } else {
        Res = Res.MathoCmd("simplify");
        return Res;
    };
}

TNumeric TNumeric::Unfactor() const
{
    return MathoCmd("unfactor");
}


TNumeric MakeTab(const TNumeric& N1)
{
TNumeric Res;
    Res.Operator = OperatorTab;
    Res.OperandsPushback(N1);
    return Res;
}

TNumeric MakeUnion(const TNumeric &N1, const TNumeric &N2)
{
TNumeric Res;
    Res.Operator = OperatorUnion;
    Res.OperandsPushback(N1);
    Res.OperandsPushback(N2);
    return Res;
}
TNumeric MakeSubscript(const TNumeric &N1, const TNumeric &N2)
{
    TNumeric Res;
        Res.Operator = OperatorSubIndex;
        Res.OperandsPushback(N1);
        Res.OperandsPushback(N2);
        return Res;
}

TNumeric MakeSuperscript(const TNumeric &N1, const TNumeric &N2)
{
    TNumeric Res;
        Res.Operator = OperatorSupIndex;
        Res.OperandsPushback(N1);
        Res.OperandsPushback(N2);
        return Res;
}

TNumeric MakeFrac(const TNumeric &N1, const TNumeric &N2)
{
    return N1/N2;
}
//****************************************************************************
TNumeric GetPolynom(size_t Power, size_t StartID)
//конструирует многочлен с нулевыми коэффициентами. ID устанавливаются от StartID до StartID+Power
{
   TNumeric Res;
   for(size_t i = 0; i <= Power; i++)
    {
        TNumeric a(0);
        a.ID = i + StartID;
        a.EditableFlags = ConstAllowed;

/*        char Buf[10]; //может быть полезно, присваивает ID значению коэфициента
        sprintf(Buf, "%d", a.ID);
        a.K = Buf;*/
        if(i == 0)
        {
            Res = a;
        } else {
            TNumeric Temp;
            if(i == 1) Temp = a*TNumeric("x");
            else Temp = a*(TNumeric("x")^TNumeric(i));
            Res = Temp + Res;
        }
    }
    return Res;
}
//****************************************************************************
QFont DecreaseFont(TPaintCanvas* Canvas)
{
    QFont Font = Canvas->Font;
    int PixelSize = Canvas->Font.pixelSize();
    if(PixelSize > 0)
        Canvas->Font.setPixelSize(PixelSize*2/3);
    else
    {
        int PointSize = Canvas->Font.pointSize();
        if(PointSize > 6)
        Canvas->Font.setPointSize(PointSize*2/3);
    }
    return Font;
}

TNumeric MakeFunction(const int Operator, const TNumeric& N)
{
 TNumeric Res;
   Res.Operator = Operator;
   Res.OperandsPushback(N);
   return Res;
}
TNumeric MakeFunction(const int Operator, const TNumeric& N1, const TNumeric& N2)
{
 TNumeric Res;
   Res.Operator = Operator;
   Res.OperandsPushback(N1);
   Res.OperandsPushback(N2);
   return Res;
}
TNumeric MakeProd(const TNumeric& N1, const TNumeric &N2)
{
   return MakeFunction(OperatorProd, N1, N2);
}

TNumeric MakeLn(const TNumeric& N)
{
    return MakeFunction(OperatorLn, N);
};

TNumeric MakeLog(const TNumeric& N, const TNumeric& Base)
{
    return MakeFunction(OperatorLog, N, Base);
};

TNumeric MakeCos(const TNumeric& N)
{
    return MakeFunction(OperatorCos, N);
}

TNumeric MakeSin(const TNumeric& N)
{
    return MakeFunction(OperatorSin, N);
}

TNumeric MakeTg(const TNumeric& N)
{
    return MakeFunction(OperatorTg, N);
}

TNumeric MakeEquality(const TNumeric& N1, const TNumeric& N2)
{
    return MakeFunction(OperatorEqual, N1, N2);
};

TNumeric MakeBelongsTo(const TNumeric& N1, const TNumeric& N2)
{
    return MakeFunction(OperatorBelongsTo, N1, N2);
};

TNumeric MakeInterval(const TNumeric &X1, const TNumeric& X2, bool includeleft, bool includeright)
{
TNumeric Res;
    if(includeleft && includeright) Res.Operator = OperatorSegment;
    if(!includeleft && includeright) Res.Operator = OperatorIntervalSegment;
    if(includeleft && !includeright) Res.Operator = OperatorSegmentInterval;
    if(!includeleft && !includeright) Res.Operator = OperatorInterval;
    Res.OperandsPushback(X1);
    Res.OperandsPushback(X2);
    return Res;
}
TNumeric MakeInline(const TNumeric &N1, const TNumeric &N2)
{
    return MakeFunction(OperatorInline, N1, N2);
}

TNumeric MakeInline(const TNumeric &N1, const TNumeric &N2, const TNumeric &N3)
{
TNumeric Res;
    Res.Operator = OperatorInline;
    Res.OperandsPushback(N1);
    Res.OperandsPushback(N2);
    Res.OperandsPushback(N3);
    return Res;
}

TNumeric MakeArccos(const TNumeric& N)
{
    return MakeFunction(OperatorArccos, N);
}

TNumeric MakeArcsin(const TNumeric& N)
{
    return MakeFunction(OperatorArcsin, N);
}

TNumeric MakeArctg(const TNumeric& N)
{
    return MakeFunction(OperatorArctg, N);
}

TNumeric MakePow(const TNumeric &N1, const TNumeric &N2)
{
    return MakeFunction(OperatorPow, N1, N2);
}

TNumeric MakeExp(const TNumeric& N)
{
    return MakeFunction(OperatorExp, N);
}
TNumeric MakeSh(const TNumeric& N)
{
    return MakeFunction(OperatorSh, N);
}
TNumeric MakeCh(const TNumeric& N)
{
    return MakeFunction(OperatorCh, N);
}

TNumeric MakeSqrt(const TNumeric& N)
{
    return MakeFunction(OperatorSqrt, N);
}

TNumeric MakePlusMinus(const TNumeric& N)
{
    return MakeFunction(OperatorPlusMinus, N);
}

TNumeric MakeCtg(const TNumeric& N)
{
    return MakeFunction(OperatorCtg, N);
}

TNumeric MakeAbs(const TNumeric& N)
{
    return MakeFunction(OperatorAbs, N);
}

TNumeric MakeSign(const TNumeric& N)
{
    return MakeFunction(OperatorSign, N);
}


TNumeric MakeBigO(const TNumeric& N)
{
    return MakeFunction(OperatorBigO, N);
}
TNumeric MakeSmallO(const TNumeric& N)
{
    return MakeFunction(OperatorSmallO, N);
}



TNumeric MakeSystemOfEquations(const TNumeric& N1, const TNumeric& N2)
{
TNumeric Res;
    Res.Operator = OperatorEqSystem;
    Res.OperandsPushback(N1);
    Res.OperandsPushback(N2);
    return Res;
}
TNumeric MakeSetOfEquations(const TNumeric& N1, const TNumeric& N2)
{
TNumeric Res;
    Res.Operator = OperatorEqSet;
    Res.OperandsPushback(N1);
    Res.OperandsPushback(N2);
    return Res;
}

TNumeric MakeIntegral(const TNumeric& N, const TNumeric& dx)
{
TNumeric Res;
    Res.Operator = OperatorIntegral;
    Res.OperandsPushback(N);
    Res.OperandsPushback(dx);
    return Res;
}

TNumeric MakeIntegral(const TNumeric& N, const string& dx)
{
    TNumeric Res;
        Res.Operator = OperatorIntegral;
        Res.OperandsPushback(N);
        Res.OperandsPushback(TNumeric(dx));
        return Res;
}

TNumeric* FindParent(TNumeric* Root, TNumeric* Child)
{
//Обходим рекурсивно Root, чтобы найти родителя WhatToDelete
        if(Root == Child) return 0;
        if(Child->EditableFlags == NoEditable) return 0;
        for(size_t i = 0; i < Root->Operands.size(); i++)
            if(&(Root->Operands[i]) == Child)
            {
                if(Root->EditableFlags == NoEditable) return 0;
                else return Root;
            };
        for(size_t i = 0; i < Root->Operands.size(); i++)
        {
            TNumeric* Temp = FindParent(&(Root->Operands[i]), Child);
            if(Temp)
                return Temp;
        };
        return 0;
};

bool CanErase(TNumeric* Root, TNumeric* WhatToDelete)
{
    if(FindParent(Root, WhatToDelete)) return true;
    else return false;
}

void EraseNumeric(TNumeric* Root, TNumeric* WhatToDelete)
{
TNumeric* Parent = FindParent(Root, WhatToDelete);
    if(Parent == 0) return;
    if(Parent->EditableFlags == NoEditable) return;
    if(WhatToDelete->EditableFlags == NoEditable) return;


    switch(Parent->Operator)
    {
        case OperatorSum:
        case OperatorMinus:
        case OperatorProd:
        case OperatorFrac:
            for(size_t i = 0; i < Parent->Operands.size(); i++)
                if(&Parent->Operands[i] == WhatToDelete)
                {
                    Parent->Operands.erase(Parent->Operands.begin()+i);
                    break;
                }
            if(Parent->Operands.size() == 1)
            {
                TNumeric Temp = Parent->Operands[0];
                *Parent = Temp;
            }
        break;
    }
}



TNumeric NumPi = TNumeric("\\pi");
TNumeric NumPi2 = TNumeric(MakeFrac(TNumeric("\\pi"), TNumeric(2)));
TNumeric NumPi4 = TNumeric(MakeFrac(TNumeric("\\pi"), TNumeric(4)));
TNumeric NumPi3 = TNumeric(MakeFrac(TNumeric("\\pi"), TNumeric(3)));
TNumeric Num2Pi3 = TNumeric(MakeFrac(TNumeric(2)*TNumeric("\\pi"), TNumeric(2)));
TNumeric NumPi6 = TNumeric(MakeFrac(TNumeric("\\pi"), TNumeric(6)));
TNumeric Num3Pi4 = TNumeric(MakeFrac(TNumeric(3)*TNumeric("\\pi"), TNumeric(4)));
TNumeric Num5Pi6 = TNumeric(MakeFrac(TNumeric(5)*TNumeric("\\pi"), TNumeric(6)));
