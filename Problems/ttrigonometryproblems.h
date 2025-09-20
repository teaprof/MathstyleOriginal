#ifndef TTRIGONOMETRYPROBLEMS_H
#define TTRIGONOMETRYPROBLEMS_H

#include "tproblem.h"
#include "TPolynomProblems.h"


class TLinearTrigEquality : public TProblem, public TEquality
//Уравнение вида
// a cos x + b sin x = c
{
        const static int IDa = 0;
        const static int IDb = 1;
        const static int IDc = 2;

        void Init(const TNumeric& UnknownVar, const TNumeric& CounterVar); //конструктор
    public:        
        TNumeric UnknownVar; //обозначение для неизвестной переменной
        TNumeric CounterVar; //переменная для обозначения сдвига по фазе в серии решений (типа arccos(a) + pi*n, n=CounterVar)
        vector<TNumeric> TrigRoots; //найденные серии решений тригонометрического уравнения вида alpha + pi*n/k
        TLinearTrigEquality();
        TLinearTrigEquality(const TNumeric& UnknownVar, const TNumeric& CounterVar);
        ~TLinearTrigEquality();

        virtual string GetTask();
        virtual string GetShortTask();
        virtual string GetClassName() { return "TLinearTrigEquality";};


        virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer);
        void SetCoef(const TNumeric& a, const TNumeric& b, const TNumeric& c);

        virtual void LoadFromFile(ifstream &f)
        {
            UnknownVar.LoadFromFile(f);
            CounterVar.LoadFromFile(f);
            TProblem::LoadFromFile(f);
        }

        virtual void SaveToFile(ofstream &f)
        {
            UnknownVar.WriteToFile(f);
            CounterVar.WriteToFile(f);
            TProblem::SaveToFile(f);
        }
        virtual void BuildPhrases();
        virtual vector<string> GetKeyWords();

        void Randomize(std::mt19937& rng);
};


class THomogeneousTrigEquality : public TPolynomConditions, public TEquality
//Уравнение вида
// p_n cos^n + p_{n-1} cos^(n-1)sin + p_1 cos^(n-2)sin^2 + ...+p_0 sin^n = 0
{
public:
//    TNumeric UnknownVar; //обозначение для неизвестной переменной
    TNumeric CounterVar; //переменная для обозначения сдвига по фазе в серии решений (типа arccos(a) + pi*n, n=CounterVar)
    vector<TNumeric> TrigRoots; //найденные серии решений тригонометрического уравнения вида alpha + pi*n/k
    THomogeneousTrigEquality(size_t MaxPower = 2);
    THomogeneousTrigEquality(size_t MaxPower, string UnknownVar, string CounterVar);
    ~THomogeneousTrigEquality();

    virtual string GetTask();
    virtual string GetShortTask();
    virtual string GetClassName() { return "THomogeneousTrigEquality";};

    //virtual void SetLeftPartP(const TPolynom &P, bool AllCoef = true); //Если AllCoef = true, то будут учитываться все коэфициенты, если false - то нулевые коэфициенты старше MajorPower будут отброшены
    //virtual void SetMaxPower(size_t MaxPower, int operation); //устанавливает максимальную степень многочлена MaxPower


    virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer);


//    virtual void SaveToFile(ofstream &f);
//    virtual void LoadFromFile(ifstream &f);
    virtual TNumeric GetVarPower(size_t power); //возвращает UnknownVar^power

    virtual void LoadFromFile(ifstream &f)
    {
        CounterVar.LoadFromFile(f);
        TPolynomConditions::LoadFromFile(f);
    }

    virtual void SaveToFile(ofstream &f)
    {
        CounterVar.WriteToFile(f);
        TPolynomConditions::SaveToFile(f);
    }
    virtual void BuildPhrases();
    virtual vector<string> GetKeyWords();

    void Randomize(std::mt19937& rng);
};

#endif // TTRIGONOMETRYPROBLEMS_H
