#ifndef TPROBLEM_H
#define TPROBLEM_H
#include <string>
#include "Base/arithmetic.h"
#include "Base/tline.h"
#include "Base/tinterval.h"
#include "Base/tphrases.h"
#include "Base/THTMLWriter.h"
using namespace std;

extern TNumeric X_1;
extern TNumeric X_2;

typedef unsigned long int seed_t;
class TRandom;

class TProblem
{
    bool DuplicateToHTML;
public:        
    bool CanRandomize;
    TNumeric* Conditions;

    TProblem();
    TProblem(const TProblem &P);
    virtual ~TProblem();

    virtual string GetTask() = 0;
    virtual string GetShortTask() = 0;
    virtual bool Solve(THTMLWriter *Writer);

/*    void ClearSolution();
    void AddLine(string str);
    void AddLine(vector<TRectangleElement*>* Lines, int Ntabs, TRectangleElement* R);
    void AddHeader(string str);
    void AddLines(vector<TRectangleElement*> Lines);*/

    virtual bool GetSolution(THTMLWriter* Writer) = 0; //возвращает непосредственно само решение без дублирования условия, записывает ответ в Intervals
    //Если Lines == NULL, то просто ищется ответ, вывод хода решения осуществляться не будет;
    //ответ записывается в дополнительную переменную, объявленную в производном классе (если она объявлена)
    //Ntabs - количество отступов слева перед строками решения


    virtual vector<TNumeric> GetTypes() {return GetTypes(Conditions);} //выдаёт все возможные типы задачи
    virtual vector<TNumeric> GetTypes(TNumeric* N); //выдаёт все возможные типы задачи, когда кликается по объекту N
    virtual void SetType(TNumeric* N, size_t Type) {Q_UNUSED(N); Q_UNUSED(Type);};

    virtual vector<string> GetKeyWords() = 0; //выдает ключевые слова

    virtual void SaveToFile(ofstream &f)
    {
        char ConditionsPresent;
        if(Conditions)
        {
            ConditionsPresent = 1;
            f.write((char*)&ConditionsPresent, sizeof(ConditionsPresent));
            Conditions->WriteToFile(f);
        } else {
            ConditionsPresent = 0;
            f.write((char*)&ConditionsPresent, sizeof(ConditionsPresent));
        }
    }

    virtual void LoadFromFile(ifstream &f)
    {
        if(Conditions) delete Conditions;
        Conditions = new TNumeric;
        //if(Solution) delete Solution;
        //Solution = 0;
        char ConditionsPresent;
        f.read(&ConditionsPresent, sizeof(ConditionsPresent));
        if(ConditionsPresent == 1)
        {
            Conditions->LoadFromFile(f);
        }
    }

    virtual string GetClassName() = 0;
    virtual void BuildPhrases();

    virtual void Randomize(TRandom* Random) { Q_UNUSED(Random);};
};


class TSimplifyProblem : public TProblem
{
    void Assign(const TSimplifyProblem& S);    
public:
    TSimplifyProblem();
    ~TSimplifyProblem();
    TSimplifyProblem(const TSimplifyProblem& S);
    void operator=(const TSimplifyProblem& S) {Assign(S);};

    virtual string GetTask();
    virtual string GetShortTask();
    virtual bool GetSolution(THTMLWriter* Writer); //возвращает непосредственно само решение без дублирования условия

    virtual string GetClassName() { return "TSimplifyProblem";};
    virtual void BuildPhrases();
    virtual vector<string> GetKeyWords();
};

class TEquality
{
    public:
    TIntervalsSet Result;
};


class TInequality
{
    public:
    TIntervalsSet Result;
};


class TRandom
{
    public:
    seed_t Seed, MaxSeed;
    seed_t Multiplier, Offset;
    TRandom();
    void InitSeed(seed_t Seed);
    int Random(int Max); //rand in [0, Max]
    int Random(int Min, int Max); //rand in [Min, Max]
    seed_t GetRandomSeed();
    seed_t *GetRandomSeedP();
};

#endif // TPROBLEM_H

