#include "polynom.h"

#include <algorithm>
#include <cassert>
#include <iostream>

#include "algebra_operations.h"
#include "arithmetic.h"

// #define __DEBUG__

TPolynom::TPolynom() {}

TPolynom::TPolynom(const TPolynom& P) {
    Assign(P);
}

TPolynom::TPolynom(const std::vector<PNumeric>& Coef) {
    this->Coef = Coef;
}
TPolynom::TPolynom(std::vector<TNumeric>&& Coef) {
    this->Coef.assign(Coef.size(), nullptr);
    for (size_t n = 0; n < Coef.size(); n++) {
        this->Coef[n] = TNumeric::create(std::move(Coef[n]));
    }
}
TPolynom::TPolynom(const std::vector<int>& Coef) {
    this->Coef.assign(Coef.size(), nullptr);
    for (size_t i = 0; i < Coef.size(); i++)
        this->Coef[i] = TNumeric::create(Coef[i]);
}

TPolynom::TPolynom(const TNumeric& a) {
    Coef.push_back(TNumeric::create(TNumeric::create(a)));
}

TPolynom::TPolynom(const TNumeric& a, const TNumeric& b) {
    Coef.push_back(TNumeric::create(b));
    Coef.push_back(TNumeric::create(a));
}

TPolynom::TPolynom(const TNumeric& a, const TNumeric& b, const TNumeric& c) {
    Coef.push_back(TNumeric::create(c));
    Coef.push_back(TNumeric::create(b));
    Coef.push_back(TNumeric::create(a));
}

TPolynom::~TPolynom() {}

void TPolynom::Assign(const TPolynom& P) {
    Coef = P.Coef;
}

PNumeric& TPolynom::GetCoef(size_t Power) {
    if (Power >= Coef.size()) {
        if (Power == 0) {
            // Запрашивается Coef[0], но Coef не содержит ни одного элемента
            Coef.push_back(TNumeric::create(0));  // добавляем нулевой элемент
            return Coef[Power];
        } else
            throw "TNumeric& TPolynom::GetCoef(size_t Power): Power >= Coef.size()";
    } else {
        return Coef[Power];
    }
}

std::shared_ptr<TNumeric> TPolynom::GetCoef(size_t Power) const {
    if (Power >= Coef.size()) {
        return TNumeric(0).shared_from_this();
    } else {
        return Coef[Power];
    }
}

void TPolynom::RemoveMajorZeros() {
    int M = MajorPower();
    Coef.erase(Coef.begin() + M + 1, Coef.end());
}

bool TPolynom::operator==(const TPolynom& P) const {
    size_t M1 = MajorPower();
    size_t M2 = P.MajorPower();
    if (M1 != M2)
        return false;
    for (size_t i = 0; i <= M1; i++)
        if (GetCoef(i)->Simplify().Calculate() != P.GetCoef(i)->Simplify().Calculate())
            return false;
    return true;
}

TPolynom TPolynom::operator=(const TNumeric& N) {
    Coef.clear();
    Coef.push_back(N.deepCopyPtr());
    return *this;
}

TPolynom TPolynom::operator=(const TPolynom& P) {
    Assign(P);
    return P;
}

TPolynom TPolynom::operator+(const TPolynom& P) {
    size_t M1 = MajorPower();
    size_t M2 = P.MajorPower();
    size_t M = M1 > M2 ? M1 : M2;
    vector<PNumeric> Res;
    Res.assign(M + 1, nullptr);
    for (size_t power = 0; power <= M; power++) {
        std::shared_ptr<TNumeric> p;
        if (power > M1)
            p = P.GetCoef(power);
        else if (power > M2)
            p = GetCoef(power);
        else
            auto p = (*GetCoef(power) + *(P.GetCoef(power))).Simplify();
        Res.push_back(p);
    }
    return TPolynom(Res);
}

TPolynom TPolynom::operator-(const TPolynom& P) {
    size_t M1 = MajorPower();
    size_t M2 = P.MajorPower();
    size_t M = M1 > M2 ? M1 : M2;
    vector<std::shared_ptr<TNumeric>> Res;
    Res.assign(M + 1, nullptr);
    for (size_t power = 0; power <= M; power++) {
        if (power > M1)
            Res[power] = (TNumeric(-1) * (*P.GetCoef(power))).Simplify().shared_from_this();
        else if (power > M2)
            Res[power] = GetCoef(power);
        else
            Res[power] = (*GetCoef(power) - (*P.GetCoef(power))).Simplify().shared_from_this();
    }
    return TPolynom(Res);
}

TPolynom TPolynom::operator*(const TPolynom& P) {
    size_t M1 = MajorPower();
    size_t M2 = P.MajorPower();
    size_t M = M1 + M2;
    vector<PNumeric> Res;
    Res.assign(M1 + M2 + 1, nullptr);
    for (size_t power1 = 0; power1 <= M1; power1++)
        for (size_t power2 = 0; power2 <= M2; power2++) {
            Res[power1 + power2] = (*Res[power1 + power2] + *GetCoef(power1) * (*P.GetCoef(power2))).shared_from_this();
        }
    for (size_t power = 0; power <= M; power++)
        Res[power] = Res[power]->Simplify().shared_from_this();
    return TPolynom(Res);
}

TPolynom TPolynom::operator*(const TNumeric& P) {
    size_t M = MajorPower();
    vector<PNumeric> Res;
    Res.assign(M + 1, nullptr);
    for (size_t power = 0; power <= M; power++)
        Res[power] = (*GetCoef(power) * P).Simplify().shared_from_this();
    return TPolynom(Res);
}

TPolynom TPolynom::operator^(size_t Power) {
    TPolynom Res = TNumeric(1);
    for (size_t i = 0; i < Power; i++)
        Res = Res * (*this);
    return Res;
}

TPolynom TPolynom::operator/(const TNumeric& P) {
    if (P == 0)
        throw "TPolynom::operator/: Division by zerro, P = 0";
    size_t M = MajorPower();
    vector<PNumeric> Res;
    Res.reserve(M + 1);
    ;
    for (size_t power = 0; power <= M; power++)
        Res.push_back(TNumeric::create(std::move((*GetCoef(power) / P).Simplify())));
    return TPolynom(Res);
}

TPolynom TPolynom::operator/(const TPolynom& P)
// алгоритм деления в столбик
{
    return Div(P, NULL);
}

TPolynom TPolynom::operator%(const TPolynom& P) {
    TPolynom Mod;
    Div(P, &Mod);
    return Mod;
}

size_t TPolynom::MajorPower() const {
    if (Coef.size() == 0)
        return 0;
    size_t M = Coef.size() - 1;
    while (M > 0 && Coef[M] == 0)
        M--;
    return M;
}

TPolynom TPolynom::Div(const TPolynom& P, TPolynom* Remainder) const {
    // implements long division
    size_t MajorPowerNumerator = MajorPower();
    size_t MajorPowerDenominator = P.MajorPower();
    vector<PNumeric> Res;
    vector<PNumeric> Numerator = Coef;
    const vector<PNumeric>& Denominator = P.Coef;
    if (MajorPowerNumerator >= MajorPowerDenominator) {
        Res.assign(MajorPowerNumerator - MajorPowerDenominator + 1, nullptr);
        while (MajorPowerNumerator >= MajorPowerDenominator) {
            size_t CurPower = MajorPowerNumerator - MajorPowerDenominator;
            TNumeric val = *Numerator[MajorPowerNumerator] / *Denominator[MajorPowerDenominator];
            val = val.Simplify();
            assert(Res[CurPower] == nullptr);
            Res[CurPower] = TNumeric::create(std::move(val));
            // Numerator[MajorPowerNumerator] = TNumeric(0); //можно это не делать
            if (MajorPowerNumerator == 0) {
                Numerator.assign(1, TNumeric::create(0));
                break;
            } else {
                MajorPowerNumerator--;
                for (size_t power = 0; power <= MajorPowerDenominator; power++) {
                    TNumeric P = *Denominator[power] * val;
                    TNumeric new_coef = *Numerator[power + CurPower] - P;
                    new_coef = new_coef.Simplify();
                    Numerator[power + CurPower] = TNumeric::create(std::move(new_coef));
                }
            }
        }
    } else {
        /// \todo: maybe simply return empty Res?
        Res.push_back(TNumeric::create(0));
    }
    if(Remainder)
    {        
        *Remainder = TPolynom(Numerator);
        // all these coefs are zeros
        Remainder->Coef.erase(Remainder->Coef.begin()+MajorPowerNumerator + 1, Remainder->Coef.end());
    };
    for (auto& it : Res) {
        if (it == nullptr) {
            it = TNumeric::create(0);
        }
    }

    return TPolynom(Res);
}

/*vector<TNumeric> PolyDiv(vector<TNumeric> Numerator, vector<TNumeric> Denominator, vector<TNumeric> *Remainder)
//делит один многочлен на другой
{
size_t MajorPowerNumerator = Numerator.size() - 1;
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
    if(MajorPowerNumerator >= MajorPowerDenominator)
    {
        size_t CurPower = MajorPowerNumerator - MajorPowerDenominator;
        Res.assign(CurPower + 1, TNumeric(0));
        TNumeric strval = Numerator[MajorPowerNumerator]/Denominator[MajorPowerDenominator];
        strval = strval.Simplify();
        Res[CurPower] = strval;
        vector<TNumeric> Numerator1;
        Numerator1.assign(Numerator.size()-1, TNumeric(0));
        vector<TNumeric> Diff; //произведение целой части от деления на текущем шаге и Denominator, степень равна
MajorPowerNumerator и равна MajorPowerDenominator + CurPower Diff.assign(MajorPowerNumerator+1, TNumeric(0)); for(size_t i = 0; i
<= MajorPowerDenominator; i++)
        {
            Diff[i+CurPower] = Denominator[i]*strval;
            Diff[i+CurPower] = Diff[i+CurPower].Simplify();
        };
        for(size_t i = 0; i < Numerator1.size(); i++)
            Numerator1[i] = ((Numerator[i] - Diff[i]).Simplify());
        vector<TNumeric> Res1 = PolyDiv(Numerator1, Denominator, Remainder);
        for(size_t i = 0; i < Res1.size(); i++)
            Res[i] = Res1[i];
    } else {
        if(Remainder)
        {
            *Remainder = Numerator;
            for(size_t i = 0; i < Remainder->size(); i++)
                Remainder->at(i) = Remainder->at(i).Simplify();
        };
    }
    return Res;
}*/

TNumeric TPolynom::Calculate(const TNumeric& x) const {
    TNumeric Res;
    size_t M = MajorPower();
    for (size_t power = 0; power <= M; power++) {
        if (power == 0) {
            Res = *GetCoef(power);
        } else {
            Res = (*GetCoef(power) * (x ^ TNumeric(static_cast<int>(power)))) + Res;
        };
    }
    return Res.Simplify();
}

vector<double> TPolynom::Calculate(const vector<double> X) const {
    vector<double> Coef;
    size_t M = MajorPower();
    Coef.assign(M + 1, 0);
    for (size_t i = 0; i <= M; i++)
        Coef[i] = GetCoef(i)->Calculate();

    vector<double> Res;
    vector<double> XPower;
    Res.assign(X.size(), 0);
    XPower.assign(X.size(), 1);
    for (size_t i = 0; i <= M; i++) {
        for (size_t j = 0; j < X.size(); j++) {
            Res[j] += XPower[j] * Coef[i];
            XPower[j] *= X[j];
        }
    }
    return Res;
}

TNumeric TPolynom::asNumeric(const TNumeric& UnknownVar) const {
    TNumeric Res;
    size_t M = MajorPower();
    bool Zero = true;
    for (size_t power = 0; power <= M; power++) {
        TNumeric C = *GetCoef(power);
        TNumeric xpowered;
        if (power == 1)
            xpowered = UnknownVar;
        else
            xpowered = UnknownVar ^ TNumeric(static_cast<int>(power));
        if (C != 0) {
            if (Zero) {
                if (power == 0)
                    Res = C;
                else {
                    if (C.strval != "1")
                        Res = C * xpowered;
                    else
                        Res = xpowered;
                };
                Zero = false;
            } else {
                if (power == 0)
                    Res = TNumeric(0) + Res;
                else {
                    if (C.strval != "1")
                        Res = C * xpowered + Res;
                    else
                        Res = xpowered + Res;
                }
            };
        }
    }
    if (Zero)
        Res = TNumeric(0);
    return Res;
}

TNumeric TPolynom::asNumeric() const {
    return asNumeric(TNumeric("x"));
}

TNumeric TPolynom::GetSeriesNumeric(const TNumeric& UnknownVar) const {
    TNumeric Res;
    size_t M = MajorPower();
    bool Zero = true;
    for (size_t power = 0; power <= M; power++) {
        TNumeric C = *GetCoef(power);
        TNumeric xpowered;
        if (power == 1)
            xpowered = UnknownVar;
        else
            xpowered = UnknownVar ^ TNumeric(static_cast<int>(power));
        if (C != 0) {
            if (Zero) {
                if (power == 0)
                    Res = C;
                else {
                    if (C.strval != "1")
                        Res = C * xpowered;
                    else
                        Res = xpowered;
                };
                Zero = false;
            } else {
                if (power == 0)
                    Res = Res + TNumeric(0);
                else {
                    if (C.strval != "1")
                        Res = Res + C * xpowered;
                    else
                        Res = Res + xpowered;
                }
            };
        }
    }
    if (Zero)
        Res = TNumeric(0);
    return Res;
}

TPolynom TPolynom::Derivative() {
    vector<PNumeric> Result;
    size_t M = MajorPower();
    Result.assign(M, nullptr);
    for (size_t i = 1; i <= M; i++) {
        const TNumeric& coef = *GetCoef(i) * TNumeric(static_cast<int>(i));
        Result[i - 1] = TNumeric::create(std::move(coef));
    };
    return TPolynom(std::move(Result));
}

bool TPolynom::IsZero() const {
    if (Coef.size() == 0)
        return true;
    for (size_t i = 0; i < Coef.size(); i++)
        if (Coef[i] != 0)
            return false;
    return true;
}

TPolynom TPolynom::GetMultiplicity(const TPolynom& Divisor, size_t& Multiplicity) const {
    TPolynom PRemaining(*this);
    Multiplicity = 0;  // кратность корня
    do {
        TPolynom Reminder;
        TPolynom Ratio = PRemaining.Div(Divisor, &Reminder);
        if (Reminder.IsZero())
        // делится без остатка
        {
            Multiplicity++;
            PRemaining = Ratio;
        } else
            break;
    } while (true);
    return PRemaining;
}

TPolynom TPolynom::TestX1(const vector<int>& Nom,
                          const vector<int>& Denom,
                          vector<TPolynom>& Res,
                          vector<size_t>& Multiplicities) const {
    TPolynom PRemaining(*this);
    for (size_t i = 0; i < Nom.size(); i++)        // перебираем делители свободного члена
        for (size_t j = 0; j < Denom.size(); j++)  // перебираем делители коэф при старшем члене
            for (int sign = -1; sign <= 1; sign += 2)  // перебераем знак
            {
                TNumeric Test;  // тестовый корень
                int N = Nom[i];
                int D = Denom[j];
                if (D == 1) {
                    Test = TNumeric(sign * N);
                } else {
                    Test = TNumeric(sign * N) / TNumeric(D);
                };
                // проверяем тестовый корень
                if (PRemaining.Calculate(Test) == 0) {
                    // тестовый корень подошёл, ищем его кратность
                    TPolynom Divisor;
                    Divisor.Coef.assign(2, TNumeric::create(1));
                    Divisor.Coef[0] = TNumeric::create(std::move((-Test).Simplify()));
                    size_t M;
                    PRemaining = GetMultiplicity(Divisor, M);
                    Res.push_back(Divisor);
                    Multiplicities.push_back(M);
                };
            }
    return PRemaining;
}

TPolynom TPolynom::TestX2(const vector<int>& a,
                          const vector<int>& b,
                          const vector<int>& c,
                          vector<TPolynom>& Res,
                          vector<size_t>& Multiplicities) const {
    TPolynom PRemaining(*this);
    for (size_t i = 0; i < a.size(); i++)
        for (size_t j = 0; j < b.size(); j++)
            for (size_t k = 0; k < c.size(); k++) {
                TPolynom Divisor;
                Divisor.Coef.assign(3, TNumeric::create(1));
                Divisor.Coef[0] = TNumeric::create(c[k]);
                Divisor.Coef[1] = TNumeric::create(b[j]);
                Divisor.Coef[2] = TNumeric::create(a[i]);
                size_t M;
                PRemaining = GetMultiplicity(Divisor, M);
                if (M > 0) {
                    Res.push_back(Divisor);
                    Multiplicities.push_back(M);
                };
            };
    return PRemaining;
}

TNumeric TPolynom::GetX0Candidate() {
    size_t MaxPower = MajorPower();
    if (MaxPower <= 0) {
        return TNumeric("1");
    } else {
        vector<int> IntCoef;
        IntCoef.assign(MaxPower + 1, 0);
        for (size_t i = 0; i <= MaxPower; i++)
            if (GetCoef(i)->isInteger(&IntCoef[i]) == false) {
                return TNumeric(1);
            }
        int NOD = GetNOD(IntCoef);
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
        if(GetCoef(0).isInteger(&FreeTerm) && GetCoef(1).isInteger(&LinearTerm) && GetCoef(MaxPower).isInteger(&LeaderTerm))
        {
            vector<int> FreeTermMults = IntFactorize(FreeTerm); //раскладываем свободный член на множители
            vector<int> LinearTermMults = IntFactorize(LinearTerm);
            vector<int> LeaderTermMults = IntFactorize(LeaderTerm); //раскладываем коэффициент при старшем члене на множители
            if(find(FreeTermMults.begin(), FreeTermMults.end(), 1) == FreeTermMults.end()) FreeTermMults.push_back(1); //если
небыло единицы среди множителей, добавляем её if(find(LinearTermMults.begin(), LinearTermMults.end(), 1) == LinearTermMults.end())
LinearTermMults.push_back(1); //если небыло единицы среди множителей, добавляем её if(find(LeaderTermMults.begin(),
LeaderTermMults.end(), 1) == LeaderTermMults.end()) LeaderTermMults.push_back(1); //если небыло единицы среди множителей,
добавляем её FreeTermMults.push_back(-1); LinearTermMults.push_back(-1); LeaderTermMults.push_back(-1); vector<int> FreeProds =
GetAllProds(FreeTermMults); vector<int> LinearProds = GetAllProds(LinearTermMults); vector<int> LeaderProds =
GetAllProds(LeaderTermMults);

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

vector<TPolynom> TPolynom::Factorize() const {
    vector<TPolynom> Res;
    vector<int> IntCoef;  // целочисленные коэффициенты
    size_t MaxPower = MajorPower();
    TPolynom P(*this);

    // проверяем, чтобы все коэффициенты были рациональными
    // заодно вычисляем НОК знаменателей всех коэффициентов
    int NOK = 1;
    for (size_t Power = 0; Power <= MaxPower; Power++) {
        if (P.GetCoef(Power)->operation != OperatorConst) {
            // пробуем сделать вычисления
            auto val = std::move(P.GetCoef(Power)->Simplify());
            P.GetCoef(Power) = TNumeric::create(std::move(val));
        };

        if (P.GetCoef(Power)->operation == OperatorFrac) {
            TNumeric Denom = P.GetCoef(Power)->operands[1]->Simplify();
            P.GetCoef(Power)->operands[1] = std::make_shared<TNumeric>(Denom);

            int intD;
            if (Denom.operation != OperatorConst || !Denom.isInteger(&intD)) {
                // не могу привести коэффициенты к рациональному виду
                Res.push_back(*this);
                return Res;
            };
            NOK = GetNOK(NOK, intD);

            TNumeric Nom = P.GetCoef(Power)->operands[0]->Simplify();
            P.GetCoef(Power)->operands[0] = std::make_shared<TNumeric>(Nom);
            if (Nom.operation != OperatorConst || !Nom.isInteger()) {
                // не могу привести коэффициенты к рациональному виду
                Res.push_back(*this);
                return Res;
            };
        } else if (!P.GetCoef(Power)->isInteger()) {
            // не могу привести коэффициенты к рациональному виду
            Res.push_back(*this);
            return Res;
        };
    }

    // умножаем все коэф на общий знаменатель
    P = P * TNumeric(TNumeric(NOK));
    IntCoef.assign(MaxPower + 1, 0);
    for (size_t Power = 0; Power <= MaxPower; Power++) {
        if (!P.GetCoef(Power)->isInteger(&IntCoef[Power])) {
            // не могу привести коэффициенты к рациональному виду
            Res.push_back(*this);
            return Res;
        };
    }

    size_t MajorPower = P.MajorPower();  // старшая степень с отличным от нуля коэффициентом
    vector<TNumeric> RationalRoots;      // найденные корни без учёта кратности

    if (MajorPower == 0)
    // a * x^0 = 0
    {
        // Многочлен вырожденный, график многочлена - прямая горизонтальная линия
        Res.push_back(*this);
        return Res;
    } else {
        int FreeMember = IntCoef[0];
        int MajorMember = IntCoef[MajorPower];
        vector<int> FreeMemberMults = IntFactorize(FreeMember);  // раскладываем свободный член на множители
        vector<int> MajorMemberMults = IntFactorize(MajorMember);  // раскладываем коэффициент при старшем члене на множители
        // Методом подбора ищем рациональные корни
        if (find(FreeMemberMults.begin(), FreeMemberMults.end(), 1) == FreeMemberMults.end())
            FreeMemberMults.push_back(1);  // если небыло единицы среди множителей, добавляем её
        if (find(MajorMemberMults.begin(), MajorMemberMults.end(), 1) == MajorMemberMults.end())
            MajorMemberMults.push_back(1);  // если небыло единицы среди множителей, добавляем её
        vector<int> FreeProds = GetAllProds(FreeMemberMults);
        vector<int> MajorProds = GetAllProds(MajorMemberMults);
        vector<TPolynom> Multipliers;
        vector<size_t> Multiplicities;
        TPolynom PRemaining = TestX1(FreeProds, MajorProds, Multipliers, Multiplicities);
        if (Multipliers.size() == 0) {
            // рациональных корней нет
            Res.push_back(*this);
            return Res;
        } else {
            if (PRemaining.MajorPower() > 0 || *PRemaining.GetCoef(0) != 1)
                Res.push_back(PRemaining);
            for (size_t i = 0; i < Multipliers.size(); i++) {
                for (size_t j = 0; j < Multiplicities[i]; j++)
                    Res.push_back(Multipliers[i]);
            }
        }
    };
    return Res;
}

bool PCancel(const TPolynom& P1, const TPolynom& P2, TPolynom* Res1, TPolynom* Res2) {
    bool Res = false;
    vector<TPolynom> M1 = P1.Factorize();
    vector<TPolynom> M2 = P2.Factorize();
    for (size_t i = 0; i < M1.size(); i++) {
        size_t JIndex = 0;
        bool Found = false;
        for (size_t j = 0; j < M2.size(); j++) {
            if (M2[j] == M1[i]) {
                Found = true;
                JIndex = j;
            }
        }
        if (Found) {
            M1.erase(M1.begin() + i);
            M2.erase(M2.begin() + JIndex);
            Res = true;
        };
    }
    if (Res) {
        Res1->Coef.clear();
        Res1->Coef.push_back(TNumeric::create(1));
        for (size_t i = 0; i < M1.size(); i++) {
            *Res1 = (*Res1) * M1[i];
        };
        Res2->Coef.clear();
        Res2->Coef.push_back(TNumeric::create(1));
        for (size_t i = 0; i < M2.size(); i++) {
            *Res2 = (*Res2) * M2[i];
        };
    } else {
        *Res1 = P1;
        *Res2 = P2;
    }
    return Res;
}

void TPolynom::SaveToFile(ofstream& f) {
    /*    uint16_t s = Coef.size();
        f.write((char*)&s, sizeof(s));
        for (size_t i = 0; i < Coef.size(); i++)
            Coef[i]->WriteToFile(f);*/
    assert(false);
}

void TPolynom::LoadFromFile(ifstream& f) {
    /*    uint16_t s;
        f.read((char*)&s, sizeof(s));
        Coef.clear();
        for (size_t i = 0; i < s; i++) {
            TNumeric N;
            N.LoadFromFile(f);
            Coef.push_back(N);
        }*/
    assert(false);
}

bool NextCombination(vector<size_t>& Combination, vector<size_t> MaxValues)
// перебирает все Combinations, такие что 0<=Combinations[i]<MaxValues[i]
{
    size_t i = 0;
    do {
        Combination[i]++;
        if (Combination[i] >= MaxValues[i]) {
            Combination[i] = 0;
            i++;
        } else
            break;
    } while (i < Combination.size());
    return (i < Combination.size());
}

vector<TPolynom> TPolynom::FactorizeKroneker(vector<size_t>* ResMultiplicty) const {
    size_t MaxPower = MajorPower() / 2;
    for (size_t m = 1; m <= MaxPower; m++)
    // m - степень многочлена-делителя
    {
        vector<vector<int>> Y;  // Y[i] -- вектор возможных значений в точках X(i)
        vector<int> X;          // точки, в которых вычисляются значения
#ifdef __DEBUG__
        cout << "Testing polynoms of power " << m << endl;
#endif

        // готовим X и Y
        for (size_t i = 0; i <= m; i++) {
            TNumeric N(static_cast<int>(i));
            TNumeric Value = Calculate(N);
            int ValueD;
            if (Value.isInteger(&ValueD) == false)
            // нецелый коэффициент, сдаёмся
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

        // Перебираем комбинации
        vector<size_t> SelectedCombination;
        SelectedCombination.assign(m + 1, 0);
        vector<size_t> MaxValues;
        MaxValues.assign(m + 1, 0);
        for (size_t i = 0; i <= m; i++)
            MaxValues[i] = Y[i].size();
        vector<int> SelectedValues;
        SelectedValues.assign(m + 1, 0);
        do {
            for (size_t i = 0; i <= m; i++)
                SelectedValues[i] = Y[i][SelectedCombination[i]];
            TPolynom P;
            if (BuildIntegerPolynom(X, SelectedValues, P) == false)
                continue;
            TPolynom Q, R;
            size_t MP = P.MajorPower();
            if (MP == m &&
                !(MP == 0 && (*P.Coef[0] == TNumeric("1") || *P.Coef[0] == TNumeric("-1"))))  // не делим на 1 или на -1
            {
                size_t Multiplicity = 0;
                TPolynom Source = *this;
                do {
                    Q = Source.Div(P, &R);
                    if (R.IsZero()) {
                        Multiplicity++;
                        Source = Q;
                    };
                } while (R.IsZero());
                if (Multiplicity > 0)
                // нашли делитель, остальные делители ищем рекурсивно
                {
                    vector<TPolynom> Res;
                    Res = Source.FactorizeKroneker(ResMultiplicty);
                    if (ResMultiplicty == 0) {
                        for (size_t c = 0; c < Multiplicity; c++)
                            Res.push_back(P);
                    } else {
                        Res.push_back(P);
                        ResMultiplicty->push_back(Multiplicity);
                    }

                    return Res;
                };
            };
        } while (NextCombination(SelectedCombination, MaxValues));
    }
    // ничего не нашли, возвращаем себя
    vector<TPolynom> Res;
    Res.push_back(*this);
    if (ResMultiplicty)
        ResMultiplicty->push_back(1);

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

TRationalFunction::TRationalFunction() {
    P.Coef.assign(1, TNumeric::create(0));
    Q.Coef.assign(1, TNumeric::create(1));
}

TRationalFunction::TRationalFunction(const TPolynom P, const TPolynom Q) {
    this->P = P;
    this->Q = Q;
}

TRationalFunction::TRationalFunction(const std::vector<PNumeric>& PCoef, const std::vector<PNumeric>& QCoef) :
    P(PCoef), Q(QCoef) {}
TRationalFunction::TRationalFunction(std::vector<TNumeric>&& PCoef, std::vector<TNumeric>&& QCoef) :
    P(std::move(PCoef)), Q(std::move(QCoef)) {}

TRationalFunction::~TRationalFunction() {}

TNumeric TRationalFunction::asNumeric() const {
    TNumeric Res;
    Res.operation = OperatorFrac;
    Res.OperandsPushback(P.asNumeric());
    Res.OperandsPushback(Q.asNumeric());
    return Res;
}

/*TRationalFunction TRationalFunction::operator=(const TRationalFunction& R)
{
    Assign(R);
    return R;
}*/

TRationalFunction TRationalFunction::operator+(const TRationalFunction& R) {
    TRationalFunction Res;
    Res.Q = Q * R.Q;
    Res.P = P * R.Q + Q * R.P;
    Res.reduce();
    return Res;
}

TRationalFunction TRationalFunction::operator-(const TRationalFunction& R) {
    TRationalFunction Res;
    Res.Q = Q * R.Q;
    Res.P = P * R.Q - Q * R.P;
    Res.reduce();
    return Res;
}

TRationalFunction TRationalFunction::operator*(const TRationalFunction& R) {
    TRationalFunction Res;
    Res.Q = Q * R.Q;
    Res.P = P * R.P;
    Res.reduce();
    return Res;
}

TRationalFunction TRationalFunction::operator/(const TRationalFunction& R) {
    TRationalFunction Res;
    Res.Q = Q * R.P;
    Res.P = P * R.Q;
    Res.reduce();
    return Res;
}
TNumeric TRationalFunction::Calculate(const TNumeric& x) {
    TNumeric Res;
    Res.operation = OperatorFrac;
    Res.OperandsPushback(P.Calculate(x));
    Res.OperandsPushback(Q.Calculate(x));
    return Res.Simplify();
}

TRationalFunction TRationalFunction::Derivative() {
    TRationalFunction Res;
    Res.P = P.Derivative() * Q - Q.Derivative() * P;
    Res.Q = Q * Q;
    return Res;
}

TNumeric TRationalFunction::GetMainPartAndO() {
    TPolynom A, R;
    A = P.Div(Q, &R);
    if (A.IsZero()) {
        return R.asNumeric() / Q.asNumeric();
    } else {
        return A.asNumeric() + R.asNumeric() / Q.asNumeric();
    }
}

void TRationalFunction::reduce() {
    TPolynom NewP, NewQ;
    PCancel(P, Q, &NewP, &NewQ);
    P = NewP;
    Q = NewQ;
}

bool BuildIntegerPolynom(const vector<TNumeric>& X, const vector<TNumeric>& Y, TPolynom& P) {
    vector<vector<TNumeric>> Coefs;
    for (size_t i = 0; i < X.size(); i++) {
        vector<TNumeric> Line;
        Line.assign(X.size(), TNumeric("0"));
        for (size_t j = 0; j < X.size(); j++)
            Line[j] = (X[i] ^ TNumeric(static_cast<int>(j))).Simplify();
        Coefs.push_back(Line);
    };
    vector<TNumeric> Res;
    if (SolveLinearSystem(Coefs, Y, Res, true)) {
        P = TPolynom(std::move(Res));
        return true;
    } else
        return false;
}

bool BuildIntegerPolynom(const vector<int>& X, const vector<int>& Y, TPolynom& P) {
    vector<TNumeric> XN;
    vector<TNumeric> YN;
    XN.assign(X.size(), TNumeric("0"));
    YN.assign(X.size(), TNumeric("0"));
    for (size_t i = 0; i < X.size(); i++)
        XN[i] = TNumeric(X[i]);
    for (size_t i = 0; i < Y.size(); i++)
        YN[i] = TNumeric(Y[i]);
    return BuildIntegerPolynom(XN, YN, P);
}
