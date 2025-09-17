#ifndef TPOWERPROBLEMS_H
#define TPOWERPROBLEMS_H

#include "tproblem.h"
//#include "Base/polynom.h"
#include "TPolynomProblems.h"

class TPowerProblemConditions : public TProblem
{
    //A^(ax^n+...+cx+d) = B^(ax^n+...+cx+d)

    static const int LeftBaseID = 0;
    static const int RightBaseID = 1;
    static const int PowerStartID = 2;

    size_t GetStartID(size_t Part); //Part = 0 for left part, and Part = 1 for right
    public:
        size_t MaxPower;
        TPowerProblemConditions(size_t MaxPower = 2);
        ~TPowerProblemConditions();

        TNumeric* GetLeftBase();
        TNumeric* GetRightBase();
        TPolynom GetLeftP();
        TPolynom GetRightP();
        void Randomize(TRandom* Rng);

        virtual void SaveToFile(ofstream &f);
        virtual void LoadFromFile(ifstream &f);        
};

class TPowerEquality : public TPowerProblemConditions, public TEquality
{
    //A^(ax^n+...+cx+d) = B^(ax^n+...+cx+d)

    size_t GetStartID(size_t Part); //Part = 0 for left part, and Part = 1 for right
    public:
        TPowerEquality(size_t MaxPower = 2);
        ~TPowerEquality();
        virtual string GetTask();
        virtual string GetShortTask();
        virtual bool GetSolution(THTMLWriter* Writer);
        virtual string GetClassName() { return "TPowerEquality";};
        virtual void BuildPhrases();
        virtual vector<string> GetKeyWords();

};


#endif // TPOWERPROBLEMS_H
