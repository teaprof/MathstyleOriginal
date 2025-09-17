#ifndef TRATIONALSUMCONDITIONS_H
#define TRATIONALSUMCONDITIONS_H

#include "tproblem.h"

class TRationalSumConditions : public TProblem
{
    //уравнение вида
    // P1_n(x)   P2_n(x)    P3_n(x)      P4_n(x)
    // ------- + -------- = --------- +  ----------
    // Q1_n(x)   Q2_n(x)   Q3_n(x)       Q4_n(x)
    //знаки определяются Sign1 и Sign2
    size_t GetPStartIndex(size_t N); //возвращает StartID для числителя N-ой дроби, выполняется Coef[0].ID = StartID, Coef[1].ID = StartID+1, ...
    size_t GetQStartIndex(size_t N); //то же для знаменателя
    vector<TNumeric> GetCoefs(size_t StartID); //возвращает коэффициенты с ID от StartIndex до StartIndex+MaxPower+1

public:
    int Sign1, Sign2;
    void SetSigns(int Sign1, int Sign2);
    size_t MaxPower;
    TRationalSumConditions(size_t MaxPower = 1, int Sign1 = +1, int Sign2 = +1);
    TRationalSumConditions(const TRationalSumConditions& R, int Sign1 = +1, int Sign2 = +1);
    ~TRationalSumConditions();

    vector<TNumeric> GetPCoef(size_t N); //возвращает числитель N-ой дроби (P_N)
    vector<TNumeric> GetQCoef(size_t N); //возвращает знаменатель N-ой дроби (P_N)
    TPolynom GetP(size_t N);  //возвращает числитель N-ой дроби
    TPolynom GetQ(size_t N);  //возвращает знаменатель N-ой дроби

    void Randomize(TRandom *Rng);
};


class TRationalSumEquality : public TRationalSumConditions, public TEquality
{
public:
    TRationalSumEquality(size_t MaxPower = 1, int Sign1 = +1, int Sign2 = +1);
    TRationalSumEquality(const TRationalSumEquality& R, int Sign1 = +1, int Sign2 = +1);
    ~TRationalSumEquality();
    virtual string GetTask();
    virtual string GetShortTask();
    virtual bool GetSolution(THTMLWriter* Writer);

    virtual vector<TNumeric> GetTypes(TNumeric* N);
    virtual void SetType(TNumeric* N, size_t Type);    
    virtual string GetClassName() { return "TRationalSumEquality";};
    virtual void BuildPhrases();
    virtual vector<string> GetKeyWords();
};

#endif // TRATIONALSUMCONDITIONS_H
