#include<iostream>
#include<algorithm>
#include "arithmetic.h"
#include "polynom.h"
#include "algebra_operations.h"

//#define __DEBUG__

TPolynom::TPolynom()
{

}

TPolynom::TPolynom(const TPolynom& P)
{
    Assign(P);
}

TPolynom::TPolynom(const std::vector<TNumeric> &Coef)
{
    this->Coef = Coef;
}
TPolynom::TPolynom(const std::vector<int> &Coef)
{
    this->Coef.assign(Coef.size(), TNumeric("0"));
    for(size_t i = 0; i < Coef.size(); i++)
        this->Coef[i] = TNumeric(Coef[i]);
}


TPolynom::TPolynom(const TNumeric& a)
{
    Coef.push_back(a);
}

TPolynom::TPolynom(const TNumeric& a, const TNumeric &b)
{
    Coef.push_back(b);
    Coef.push_back(a);
}

TPolynom::TPolynom(const TNumeric& a, const TNumeric &b, const TNumeric& c)
{
    Coef.push_back(c);
    Coef.push_back(b);
    Coef.push_back(a);
}

TPolynom::~TPolynom()
{

}

void TPolynom::Assign(const TPolynom &P)
{
    Coef = P.Coef;
}

TNumeric& TPolynom::GetCoef(size_t Power)
{
    if(Power >= Coef.size())
    {
        if(Power == 0)
        {
            //Запрашивается Coef[0], но Coef не содержит ни одного элемента
            Coef.push_back(TNumeric(0)); //добавляем нулевой элемент
            return Coef[Power];
        } else throw "TNumeric& TPolynom::GetCoef(size_t Power): Power >= Coef.size()";
    } else {
        return Coef[Power];
    }
}

TNumeric TPolynom::GetCoef(size_t Power) const
{
    if(Power >= Coef.size())
    {
        return TNumeric(0);
    } else {
        return Coef[Power];
    }
}


void TPolynom::RemoveZeros()
{
int M = MajorPower();
    Coef.erase(Coef.begin() + M + 1, Coef.end());
}


bool TPolynom::operator==(const TPolynom& P) const
{
size_t M1 = MajorPower();
size_t M2 = P.MajorPower();
    if(M1 != M2) return false;
    for(size_t i = 0; i <= M1; i++)
        if(GetCoef(i).Simplify().Calculate() != P.GetCoef(i).Simplify().Calculate()) return false;
    return true;
}

TPolynom TPolynom::operator=(const TNumeric& N)
{
    Coef.clear();
    Coef.push_back(N);
    return *this;
}


TPolynom TPolynom::operator=(const TPolynom& P)
{
    Assign(P);
    return P;
}

TPolynom TPolynom::operator+(const TPolynom& P)
{
size_t M1 = MajorPower();
size_t M2 = P.MajorPower();
size_t M = M1>M2?M1:M2;
vector<TNumeric> Res;
    Res.assign(M + 1, TNumeric(0));
    for(size_t power = 0; power <= M; power++)
    {
        if(power>M1) Res[power] = P.GetCoef(power); else
        if(power>M2) Res[power] = GetCoef(power); else
        Res[power] = (GetCoef(power) + P.GetCoef(power)).Simplify();
    }
    return TPolynom(Res);
}

TPolynom TPolynom::operator-(const TPolynom& P)
{
size_t M1 = MajorPower();
size_t M2 = P.MajorPower();
size_t M = M1>M2?M1:M2;
vector<TNumeric> Res;
    Res.assign(M + 1, TNumeric(0));
    for(size_t power = 0; power <= M; power++)
    {
        if(power>M1) Res[power] = (TNumeric(-1)*P.GetCoef(power)).Simplify(); else
        if(power>M2) Res[power] = GetCoef(power); else
        Res[power] = (GetCoef(power) - P.GetCoef(power)).Simplify();
    }
    return TPolynom(Res);
}

TPolynom TPolynom::operator*(const TPolynom& P)
{
size_t M1 = MajorPower();
size_t M2 = P.MajorPower();
size_t M = M1 + M2;
vector<TNumeric> Res;
    Res.assign(M1+M2+1, TNumeric(0));
    for(size_t power1 = 0; power1 <= M1; power1++)
        for(size_t power2 = 0; power2 <= M2; power2++)
        {
            Res[power1+power2] = Res[power1+power2] + GetCoef(power1)*P.GetCoef(power2);
        }
    for(size_t power = 0; power <= M; power ++)
        Res[power] = Res[power].Simplify();
    return TPolynom(Res);
}

TPolynom TPolynom::operator*(const TNumeric& P)
{
size_t M = MajorPower();
vector<TNumeric> Res;
    Res.assign(M + 1, TNumeric(0));
    for(size_t power = 0; power <= M; power++)
        Res[power] = (GetCoef(power)*P).Simplify();
    return TPolynom(Res);
}

TPolynom TPolynom::operator^(size_t Power)
{
TPolynom Res = TNumeric(1);
    for(size_t i = 0; i < Power; i++)
        Res = Res* (*this);
    return Res;
}

TPolynom TPolynom::operator/(const TNumeric& P)
{
    if(P == 0) throw "TPolynom::operator/: Division by zerro, P = 0";
size_t M = MajorPower();
vector<TNumeric> Res;
    Res.assign(M + 1, TNumeric(0));
    for(size_t power = 0; power <= M; power++)
        Res[power] = (GetCoef(power)/P).Simplify();
    return TPolynom(Res);
}


TPolynom TPolynom::operator/(const TPolynom& P)
//алгоритм деления в столбик
{
    return Div(P, NULL);
}

TPolynom TPolynom::operator%(const TPolynom& P)
{
TPolynom Mod;
    Div(P, &Mod);
    return Mod;
}

size_t TPolynom::MajorPower() const
{
    if(Coef.size() == 0) return 0;
size_t M = Coef.size() - 1;
    while(M>0 && Coef[M] == 0) M--;
    return M;
}

TPolynom TPolynom::Div(const TPolynom& P, TPolynom* Remainder) const
{
    size_t MajorPowerNominator = MajorPower();
    size_t MajorPowerDenominator = P.MajorPower();
    vector<TNumeric> Res;
    vector<TNumeric> Nominator = Coef;
    vector<TNumeric> Denominator = P.Coef;
    if(MajorPowerNominator >= MajorPowerDenominator)
    {
        Res.assign(MajorPowerNominator - MajorPowerDenominator + 1, TNumeric(0));
        while(MajorPowerNominator >= MajorPowerDenominator)
        {
                size_t CurPower = MajorPowerNominator - MajorPowerDenominator;
                TNumeric K = Nominator[MajorPowerNominator]/Denominator[MajorPowerDenominator];
                K = K.Simplify();
                Res[CurPower] = K;
                //Nominator[MajorPowerNominator] = TNumeric(0); //можно это не делать
                if(MajorPowerNominator == 0)
                {
                    Nominator.assign(1, TNumeric(0));
                    break;
                } else {
                    MajorPowerNominator--;
                    vector<TNumeric> Nominator1;
                    Nominator1.assign(Nominator.size()-1, TNumeric(0));
                    for(size_t power = 0; power <= MajorPowerDenominator; power++)
                    {
                        TNumeric P = Denominator[power]*K;
                        Nominator[power+CurPower] = Nominator[power+CurPower] - P;
                        Nominator[power+CurPower] = Nominator[power+CurPower].Simplify();
                    }
                };

        };
    } else {
        Res.push_back(TNumeric(0));
    }
    if(Remainder)
    {
        *Remainder = Nominator;
        Remainder->Coef.erase(Remainder->Coef.begin()+MajorPowerNominator + 1, Remainder->Coef.end());
    };

    return TPolynom(Res);
}


/*vector<TNumeric> PolyDiv(vector<TNumeric> Nominator, vector<TNumeric> Denominator, vector<TNumeric> *Remainder)
//делит один многочлен на другой
{
size_t MajorPowerNominator = Nominator.size() - 1;
size_t MajorPowerDenominator = Denominator.size() - 1;
    while(MajorPowerDenominator >= 0)
    {
        TNumeric D = Denominator[MajorPowerDenominator];
        D = D.Simplify();
        if(D.Calculate() == 0)
        {
            if(MajorPowerDenominator == 0)
                throw "PolyDiv: Denominator is equal to 0";
            MajorPowerDenominator--;
        } else break;
    };
vector<TNumeric> Res;
    if(MajorPowerNominator >= MajorPowerDenominator)
    {
        size_t CurPower = MajorPowerNominator - MajorPowerDenominator;
        Res.assign(CurPower + 1, TNumeric(0));
        TNumeric K = Nominator[MajorPowerNominator]/Denominator[MajorPowerDenominator];
        K = K.Simplify();
        Res[CurPower] = K;
        vector<TNumeric> Nominator1;
        Nominator1.assign(Nominator.size()-1, TNumeric(0));
        vector<TNumeric> Diff; //произведение целой части от деления на текущем шаге и Denominator, степень равна MajorPowerNominator и равна MajorPowerDenominator + CurPower
        Diff.assign(MajorPowerNominator+1, TNumeric(0));
        for(size_t i = 0; i <= MajorPowerDenominator; i++)
        {
            Diff[i+CurPower] = Denominator[i]*K;
            Diff[i+CurPower] = Diff[i+CurPower].Simplify();
        };
        for(size_t i = 0; i < Nominator1.size(); i++)
            Nominator1[i] = ((Nominator[i] - Diff[i]).Simplify());
        vector<TNumeric> Res1 = PolyDiv(Nominator1, Denominator, Remainder);
        for(size_t i = 0; i < Res1.size(); i++)
            Res[i] = Res1[i];
    } else {
        if(Remainder)
        {
            *Remainder = Nominator;
            for(size_t i = 0; i < Remainder->size(); i++)
                Remainder->at(i) = Remainder->at(i).Simplify();
        };
    }
    return Res;
}*/


TNumeric TPolynom::Calculate(const TNumeric &x) const
{
    TNumeric Res;
    size_t M = MajorPower();
        for(size_t power = 0; power <= M; power++)
        {
            if(power == 0)
            {
                Res = GetCoef(power);
            } else {
                Res = (GetCoef(power)*(x^TNumeric(power))) + Res;
            };
        }
        return Res.Simplify();
}

vector<double> TPolynom::Calculate(const vector<double> X) const
{
vector<double> Coef;
size_t M = MajorPower();
    Coef.assign(M+1, 0);
    for(size_t i = 0; i <= M; i++)
        Coef[i] = GetCoef(i).Calculate();

vector<double> Res;
vector<double> XPower;
    Res.assign(X.size(), 0);
    XPower.assign(X.size(), 1);
    for(size_t i = 0; i <= M; i++)
    {
        for(size_t j = 0; j < X.size(); j++)
        {
            Res[j] += XPower[j]*Coef[i];
            XPower[j] *= X[j];
        }
    }
    return Res;
}


TNumeric TPolynom::GetNumeric(const TNumeric& UnknownVar) const
{
TNumeric Res;
size_t M = MajorPower();
bool Zero = true;
    for(size_t power = 0; power <= M; power++)
    {
        TNumeric C = GetCoef(power);
        TNumeric xpowered;
        if(power == 1) xpowered = UnknownVar;
        else xpowered = UnknownVar^TNumeric(power);
        if(C != 0)
        {
            if(Zero)
            {
                if(power == 0)
                    Res = C;
                else
                {
                    if(C.K != "1")
                        Res = C*xpowered;
                    else
                        Res = xpowered;
                };
                Zero = false;
            } else {
                if(power == 0)
                    Res = TNumeric(0) + Res;
                else
                {
                    if(C.K != "1")
                        Res = C*xpowered + Res;
                    else
                        Res = xpowered + Res;
                }
            };
        }
    }
    if(Zero) Res = TNumeric(0);
    return Res;
}

TNumeric TPolynom::GetNumeric() const
{
    return GetNumeric(TNumeric("x"));
}

TNumeric TPolynom::GetSeriesNumeric(const TNumeric& UnknownVar) const
{
    TNumeric Res;
    size_t M = MajorPower();
    bool Zero = true;
        for(size_t power = 0; power <= M; power++)
        {
            TNumeric C = GetCoef(power);
            TNumeric xpowered;
            if(power == 1) xpowered = UnknownVar;
            else xpowered = UnknownVar^TNumeric(power);
            if(C != 0)
            {
                if(Zero)
                {
                    if(power == 0)
                        Res = C;
                    else
                    {
                        if(C.K != "1")
                            Res = C*xpowered;
                        else
                            Res = xpowered;
                    };
                    Zero = false;
                } else {
                    if(power == 0)
                        Res = Res + TNumeric(0);
                    else
                    {
                        if(C.K != "1")
                            Res = Res + C*xpowered;
                        else
                            Res = Res + xpowered;
                    }
                };
            }
        }
        if(Zero) Res = TNumeric(0);
        return Res;
}



TPolynom TPolynom::Derivative()
{
vector<TNumeric> Result;
size_t M = MajorPower();
    Result.assign(M, TNumeric(0));
    for(size_t i = 1; i <= M; i++)
    {
        Result[i-1] = GetCoef(i)*TNumeric(i);
    };
    return TPolynom(Result);
}

bool TPolynom::IsZero() const
{
    if(Coef.size() == 0) return true;
    for(size_t i = 0; i < Coef.size(); i++)
        if(Coef[i] != 0) return false;
    return true;
}

TPolynom TPolynom::GetMultiplicity(TPolynom Divisor, size_t& Multiplicity) const
//определяет, сколько раз многочлен делится на Divisor
//возвращает многочлен после деления на Divisor^Multiplicty
{
TPolynom PRemaining(*this);
    Multiplicity = 0; //кратность корня
    do
    {
        TPolynom Reminder;
        TPolynom Ratio = PRemaining.Div(Divisor, &Reminder);
        if(Reminder.IsZero())
            //делится без остатка
        {
            Multiplicity++;
            PRemaining = Ratio;
        }
        else break;
    }while(true);
    return PRemaining;

}

TPolynom TPolynom::TestX1(vector<int> Nom, vector<int> Denom, vector<TPolynom> &Res, vector<size_t> &Multiplicities) const
//из множества X находит такие X, которые являются корнями многочлена
//записывает в Res одночлены x - x0, кратность корней - в Multiplicities, и возвращает многочлен, полученный делением исходного на найденные (x-x0) с учётом их кратности
{
TPolynom PRemaining(*this);
    for(size_t i = 0; i < Nom.size(); i++) //перебираем делители свободного члена
        for(size_t j = 0; j < Denom.size(); j++) //перебираем делители коэф при старшем члене
            for(int sign = -1; sign <=1; sign+=2) //перебераем знак
            {
                TNumeric Test; //тестовый корень
                int N = Nom[i];
                int D = Denom[j];
                if(D == 1)
                {
                    Test = TNumeric(sign*N);
                } else {
                    Test.operation = OperatorFrac;
                    Test.operands.clear();
                    Test.OperandsPushback(TNumeric(sign*N));
                    Test.OperandsPushback(TNumeric(D));
                };
                //проверяем тестовый корень
                if(PRemaining.Calculate(Test) == 0)
                {
                    //тестовый корень подошёл, ищем его кратность
                    TPolynom Divisor;
                    Divisor.Coef.assign(2, TNumeric(1));
                    Divisor.Coef[0] = (-Test).Simplify();
                    size_t M;
                    PRemaining = GetMultiplicity(Divisor, M);
                    Res.push_back(Divisor);
                    Multiplicities.push_back(M);
                };
            }
    return PRemaining;
}

/*TPolynom TPolynom::TestX2(vector<int> a, vector<int> b, vector<int> c, vector<TPolynom> &Res, vector<size_t> &Multiplicities) const
//ищет делители вида ax^2+bx+c
{
TPolynom PRemaining(*this);
    for(size_t i = 0; i < a.size(); i++)
        for(size_t j = 0; j < b.size(); j++)
            for(size_t k = 0; k < c.size(); k++)
            {
                TPolynom Divisor;
                Divisor.Coef.assign(3, TNumeric(1));
                Divisor.Coef[0] = TNumeric(c[k]);
                Divisor.Coef[1] = TNumeric(b[j]);
                Divisor.Coef[2] = TNumeric(a[i]);
                size_t M;
                PRemaining = GetMultiplicity(Divisor, M);
                if(M > 0)
                {
                    Res.push_back(Divisor);
                    Multiplicities.push_back(M);
                };
            };
        return PRemaining;
}*/

TNumeric TPolynom::GetX0Candidate()
{
size_t MaxPower = MajorPower();
    if(MaxPower <= 0)
    {
        return TNumeric("1");
    } else {
        vector<int> IntCoef;
        IntCoef.assign(MaxPower+1, 0);
        for(size_t i = 0; i <= MaxPower; i++)
            if(GetCoef(i).IsInteger(&IntCoef[i]) == false)
            {
                return TNumeric(1);
            }
        int NOD = GetNOD(IntCoef[0], IntCoef[1]);
        for(size_t i = 2; i < IntCoef.size(); i++)
            NOD = GetNOD(NOD, IntCoef[i]);
        return TNumeric(NOD);
   };
}

/*size_t TPolynom::GetX2Candidates(vector<int> &a, vector<int> &b, vector<int> &c)
{
int MaxPower = MajorPower();
    if(MaxPower <= 2)
    {
        return 0;
    } else {
        //находим свободный член, член при первой степени и главный член
        int FreeTerm, LinearTerm, LeaderTerm;
        if(GetCoef(0).IsInteger(&FreeTerm) && GetCoef(1).IsInteger(&LinearTerm) && GetCoef(MaxPower).IsInteger(&LeaderTerm))
        {
            vector<int> FreeTermMults = IntFactorize(FreeTerm); //раскладываем свободный член на множители
            vector<int> LinearTermMults = IntFactorize(LinearTerm);
            vector<int> LeaderTermMults = IntFactorize(LeaderTerm); //раскладываем коэффициент при старшем члене на множители
            if(find(FreeTermMults.begin(), FreeTermMults.end(), 1) == FreeTermMults.end()) FreeTermMults.push_back(1); //если небыло единицы среди множителей, добавляем её
            if(find(LinearTermMults.begin(), LinearTermMults.end(), 1) == LinearTermMults.end()) LinearTermMults.push_back(1); //если небыло единицы среди множителей, добавляем её
            if(find(LeaderTermMults.begin(), LeaderTermMults.end(), 1) == LeaderTermMults.end()) LeaderTermMults.push_back(1); //если небыло единицы среди множителей, добавляем её
            FreeTermMults.push_back(-1);
            LinearTermMults.push_back(-1);
            LeaderTermMults.push_back(-1);
            vector<int> FreeProds = GetAllProds(FreeTermMults);
            vector<int> LinearProds = GetAllProds(LinearTermMults);
            vector<int> LeaderProds = GetAllProds(LeaderTermMults);

            vector<int> a;
            vector<int> b;
            vector<int> c;
            for(size_t i = 0; i < FreeProds.size(); i++)
                for(size_t j = 0; j < LinearProds.size(); j++)
                    for(size_t k = 0; k < LeaderProds.size(); k++)
                    {
                        int tempc = FreeProds[i];
                        int tempa = LeaderProds[k];
                        //if(LinearProds[j] - c*)
                    }

        } else {
            return 0;
        }
    }
}*/

vector<TPolynom> TPolynom::Factorize() const
{
vector<TPolynom> Res;
vector<int> IntCoef;  //целочисленные коэффициенты
size_t MaxPower = MajorPower();
TPolynom P(*this);

    //проверяем, чтобы все коэффициенты были рациональными
    //заодно вычисляем НОК знаменателей всех коэффициентов
    int NOK = 1;
    for(size_t Power = 0; Power <= MaxPower; Power++)
    {
        if(P.GetCoef(Power).operation != OperatorConst)
        {
            P.GetCoef(Power) = P.GetCoef(Power).Simplify(); //пробуем сделать вычисления
        };

        if(P.GetCoef(Power).operation == OperatorFrac)
        {
            TNumeric Denom = P.GetCoef(Power).operands[1]->Simplify();
            P.GetCoef(Power).operands[1] = std::make_shared<TNumeric>(Denom);

            int intD;
            if(Denom.operation != OperatorConst || !Denom.IsInteger(&intD))
            {
                //не могу привести коэффициенты к рациональному виду
                Res.push_back(*this);
                return Res;
            };
            NOK = GetNOK(NOK, intD);

            TNumeric Nom = P.GetCoef(Power).operands[0]->Simplify();
            P.GetCoef(Power).operands[0] = std::make_shared<TNumeric>(Nom);
            if(Nom.operation != OperatorConst || !Nom.IsInteger(0))
            {
                //не могу привести коэффициенты к рациональному виду
                Res.push_back(*this);
                return Res;
             };
        } else
        if(!P.GetCoef(Power).IsInteger(0))
        {
            //не могу привести коэффициенты к рациональному виду
            Res.push_back(*this);
            return Res;
        };
    }

        //умножаем все коэф на общий знаменатель
        P = P * TNumeric(TNumeric(NOK));
        IntCoef.assign(MaxPower+1, 0);
        for(size_t Power = 0; Power <= MaxPower; Power++)
        {
            if(!P.GetCoef(Power).IsInteger(&IntCoef[Power]))
            {
                //не могу привести коэффициенты к рациональному виду
                Res.push_back(*this);
                return Res;
            };
        }

        size_t MajorPower = P.MajorPower(); //старшая степень с отличным от нуля коэффициентом
        vector<TNumeric> RationalRoots; //найденные корни без учёта кратности

        if(MajorPower == 0)
            // a * x^0 = 0
        {
            //Многочлен вырожденный, график многочлена - прямая горизонтальная линия
            Res.push_back(*this);
            return Res;
        } else {
            int FreeMember = IntCoef[0];
            int MajorMember = IntCoef[MajorPower];
            vector<int> FreeMemberMults = IntFactorize(FreeMember); //раскладываем свободный член на множители
            vector<int> MajorMemberMults = IntFactorize(MajorMember); //раскладываем коэффициент при старшем члене на множители
            //Методом подбора ищем рациональные корни
            if(find(FreeMemberMults.begin(), FreeMemberMults.end(), 1) == FreeMemberMults.end()) FreeMemberMults.push_back(1); //если небыло единицы среди множителей, добавляем её
            if(find(MajorMemberMults.begin(), MajorMemberMults.end(), 1) == MajorMemberMults.end()) MajorMemberMults.push_back(1); //если небыло единицы среди множителей, добавляем её
            vector<int> FreeProds = GetAllProds(FreeMemberMults);
            vector<int> MajorProds = GetAllProds(MajorMemberMults);
            vector<TPolynom> Multipliers;
            vector<size_t> Multiplicities;
            TPolynom PRemaining = TestX1(FreeProds, MajorProds, Multipliers, Multiplicities);
            if(Multipliers.size() == 0)
            {
                //рациональных корней нет
                Res.push_back(*this);
                return Res;
            }
            else {
                if(PRemaining.MajorPower() > 0 || PRemaining.GetCoef(0) != 1)
                    Res.push_back(PRemaining);
                for(size_t i = 0; i < Multipliers.size(); i++)
                {
                    for(size_t j = 0; j < Multiplicities[i]; j++)
                        Res.push_back(Multipliers[i]);
                }
            }
        };
        return Res;
}

bool PCancel(const TPolynom &P1, const TPolynom &P2, TPolynom *Res1, TPolynom *Res2)
{
    bool Res = false;
vector<TPolynom> M1 = P1.Factorize();
vector<TPolynom> M2 = P2.Factorize();
    for(size_t i = 0; i < M1.size(); i++)
    {
        size_t JIndex = 0;
        bool Found = false;
        for(size_t j = 0; j < M2.size(); j++)
        {
            if(M2[j] == M1[i])
            {
                Found = true;
                JIndex = j;
            }
        }
        if(Found)
        {
            M1.erase(M1.begin() + i);
            M2.erase(M2.begin()+JIndex);
            Res = true;
        };
    }
    if(Res)
    {
        Res1->Coef.clear();
        Res1->Coef.push_back(TNumeric(1));
        for(size_t i = 0; i < M1.size(); i++)
        {
            *Res1 = (*Res1)*M1[i];
        };
        Res2->Coef.clear();
        Res2->Coef.push_back(TNumeric(1));
        for(size_t i = 0; i < M2.size(); i++)
        {
            *Res2 = (*Res2)*M2[i];
        };
    }else {
        *Res1 = P1;
        *Res2 = P2;
    }
    return Res;
}


void TPolynom::SaveToFile(ofstream &f)
{
    uint16_t s = Coef.size();
    f.write((char*)&s, sizeof(s));
    for(size_t i = 0; i < Coef.size(); i++)
        Coef[i].WriteToFile(f);
}

void TPolynom::LoadFromFile(ifstream &f)
{
    uint16_t s;
    f.read((char*)&s, sizeof(s));
    Coef.clear();
    for(size_t i = 0; i < s; i++)
    {
        TNumeric N;
        N.LoadFromFile(f);
        Coef.push_back(N);
    }

}

bool NextCombination(vector<size_t> &Combination, vector<size_t> MaxValues)
//перебирает все Combinations, такие что 0<=Combinations[i]<MaxValues[i]
{
    size_t i = 0;
    do
    {
        Combination[i]++;
        if(Combination[i] >= MaxValues[i])
        {
            Combination[i] = 0;
            i++;
        } else break;
    }while(i < Combination.size());
    return (i < Combination.size());
}

vector<TPolynom> TPolynom::FactorizeKroneker(vector<size_t> *ResMultiplicty) const
{
size_t MaxPower = MajorPower()/2;
    for(size_t m = 1; m <= MaxPower; m++)
        //m - степень многочлена-делителя
    {
        vector< vector<int> > Y; //Y[i] -- вектор возможных значений в точках X(i)
        vector<int> X; //точки, в которых вычисляются значения
#ifdef __DEBUG__
        cout<<"Testing polynoms of power "<<m<<endl;
#endif

        //готовим X и Y
        for(size_t i = 0; i <= m; i++)
        {

            TNumeric Value = Calculate(TNumeric(i));
            int ValueD;
            if(Value.IsInteger(&ValueD) == false)
            //нецелый коэффициент, сдаёмся
            {
                vector<TPolynom> Res;
                Res.push_back(*this);
                return Res;
            }
            vector<int> Multiplicators = IntFactorize(ValueD);
            Multiplicators.push_back(-1);
            vector<int> M = GetAllProds(Multiplicators);
            Y.push_back(M);
            X.push_back(i);
        };

        //Перебираем комбинации
        vector<size_t> SelectedCombination;
        SelectedCombination.assign(m+1, 0);
        vector<size_t> MaxValues;
        MaxValues.assign(m+1, 0);
        for(size_t i = 0; i <= m; i++)
            MaxValues[i] = Y[i].size();
        vector<int> SelectedValues;
        SelectedValues.assign(m+1, 0);
        do
        {
            for(size_t i = 0; i <= m; i++)
                SelectedValues[i] = Y[i][SelectedCombination[i]];
            TPolynom P;
            if(BuildIntegerPolynom(X, SelectedValues, P) == false) continue;
            TPolynom Q, R;
            size_t MP = P.MajorPower();
            if(MP == m && !(MP == 0 && (P.Coef[0] == TNumeric("1") || P.Coef[0] == TNumeric("-1")))) //не делим на 1 или на -1
            {
                size_t Multiplicity = 0;
                TPolynom Source = *this;
                do
                {
                    Q = Source.Div(P, &R);
                    if(R.IsZero())
                    {
                        Multiplicity++;
                        Source = Q;
                    };
                } while(R.IsZero());
                if(Multiplicity > 0)
                //нашли делитель, остальные делители ищем рекурсивно
                {
                    vector<TPolynom> Res;
                    Res = Source.FactorizeKroneker(ResMultiplicty);
                    if(ResMultiplicty == 0)
                    {
                        for(size_t c = 0; c < Multiplicity; c++)
                            Res.push_back(P);
                    } else {
                        Res.push_back(P);
                        ResMultiplicty->push_back(Multiplicity);
                    }

                    return Res;
                };
            };
        }while(NextCombination(SelectedCombination, MaxValues));
    }
    //ничего не нашли, возвращаем себя
    vector<TPolynom> Res;
    Res.push_back(*this);
    if(ResMultiplicty) ResMultiplicty->push_back(1);

    return Res;
}

//*****************************************************************************************
//*****************************************************************************************
//*****************************************************************************************
/*void TRationalFunction::Assign(const TRationalFunction &R)
{
    P = R.P;
    Q = R.Q;
}*/

TRationalFunction::TRationalFunction()
{
    P.Coef.assign(1, TNumeric(0));
    Q.Coef.assign(1, TNumeric(1));
}

TRationalFunction::TRationalFunction(const TPolynom P, const TPolynom Q)
{
    this->P = P;
    this->Q = Q;
}

TRationalFunction::TRationalFunction(const std::vector<TNumeric> PCoef, const std::vector<TNumeric> QCoef)
{
    TRationalFunction(TPolynom(PCoef), TPolynom(QCoef));
}

TRationalFunction::~TRationalFunction()
{

}

TNumeric TRationalFunction::GetNumeric() const
{
TNumeric Res;
    Res.operation = OperatorFrac;
    Res.OperandsPushback(P.GetNumeric());
    Res.OperandsPushback(Q.GetNumeric());
    return Res;
}

/*TRationalFunction TRationalFunction::operator=(const TRationalFunction& R)
{
    Assign(R);
    return R;
}*/

TRationalFunction TRationalFunction::operator+(const TRationalFunction& R)
{
TRationalFunction Res;
    Res.Q = Q*R.Q;
    Res.P = P*R.Q + Q*R.P;
    Res.Cancel();
    return Res;
}

TRationalFunction TRationalFunction::operator-(const TRationalFunction& R)
{
    TRationalFunction Res;
        Res.Q = Q*R.Q;
        Res.P = P*R.Q - Q*R.P;
        Res.Cancel();
        return Res;
}

TRationalFunction TRationalFunction::operator*(const TRationalFunction& R)
{
    TRationalFunction Res;
    Res.Q = Q*R.Q;
    Res.P = P*R.P;
    Res.Cancel();
    return Res;
}

TRationalFunction TRationalFunction::operator/(const TRationalFunction& R)
{
    TRationalFunction Res;
    Res.Q = Q*R.P;
    Res.P = P*R.Q;
    Res.Cancel();
    return Res;
 }
TNumeric TRationalFunction::Calculate(const TNumeric& x)
{
    TNumeric Res;
    Res.operation = OperatorFrac;
    Res.OperandsPushback(P.Calculate(x));
    Res.OperandsPushback(Q.Calculate(x));
    return Res.Simplify();
}

TRationalFunction TRationalFunction::Derivative()
{
TRationalFunction Res;
    Res.P = P.Derivative()*Q - Q.Derivative()*P;
    Res.Q = Q*Q;
    return Res;
}


TNumeric TRationalFunction::GetMainPartAndO()
{
TPolynom A, R;
    A = P.Div(Q, &R);
    if(A.IsZero())
    {
        return R.GetNumeric()/Q.GetNumeric();
    } else {
        return A.GetNumeric() + R.GetNumeric()/Q.GetNumeric();
    }
}

void TRationalFunction::Cancel()
{
TPolynom NewP, NewQ;
    PCancel(P, Q, &NewP, &NewQ);
    P = NewP;
    Q = NewQ;
}



bool BuildIntegerPolynom(const vector<TNumeric> &X, const vector<TNumeric> &Y, TPolynom& P)
{
vector< vector<TNumeric> > Coefs;
    for(size_t i = 0; i < X.size(); i++)
    {
        vector<TNumeric> Line;
        Line.assign(X.size(), TNumeric("0"));
        for(size_t j = 0; j < X.size(); j++)
            Line[j] = (X[i]^TNumeric(j)).Simplify();
        Coefs.push_back(Line);
    };
vector<TNumeric> Res;
    if(SolveLinearSystem(Coefs, Y, Res, true))
    {
        P = TPolynom(Res);
        return true;
    } else return false;
}

bool BuildIntegerPolynom(const vector<int> &X, const vector<int> &Y, TPolynom& P)
{
    vector<TNumeric> XN;
    vector<TNumeric> YN;
    XN.assign(X.size(), TNumeric("0"));
    YN.assign(X.size(), TNumeric("0"));
    for(size_t i = 0; i < X.size(); i++)
        XN[i] = TNumeric(X[i]);
    for(size_t i = 0; i < Y.size(); i++)
        YN[i] = TNumeric(Y[i]);
    return BuildIntegerPolynom(XN, YN, P);
}
