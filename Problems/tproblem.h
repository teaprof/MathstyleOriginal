#ifndef TPROBLEM_H
#define TPROBLEM_H
#include <string>
#include <memory>
#include <random>
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
public:        
    bool CanRandomize;
    std::shared_ptr<TNumeric> Conditions;

    TProblem();    
    TProblem(const TProblem &P);
    virtual ~TProblem();
    TProblem& operator=(const TProblem &other);

    virtual string GetTask() = 0;
    virtual string GetShortTask() = 0;
    virtual bool Solve(std::shared_ptr<THTMLWriter> Writer);

    //возвращает непосредственно само решение без дублирования условия
    virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer) {Q_UNUSED(Writer); return false;}


    //возвращает непосредственно само решение без дублирования условия
    virtual vector<TNumeric> GetTypes() {
        return GetTypes(Conditions); // todo: why?
    }
    //выдаёт все возможные типы задачи, когда кликается по объекту N
    virtual vector<TNumeric> GetTypes(const std::shared_ptr<TNumeric>& N);
    virtual void SetType(std::shared_ptr<TNumeric> N, size_t Type) {
        Q_UNUSED(N);
        Q_UNUSED(Type);
    }

    virtual vector<string> GetKeyWords() = 0; //выдает ключевые слова

    virtual void SaveToFile(ofstream &f);
    virtual void LoadFromFile(ifstream &f);

    virtual string GetClassName() = 0;
    virtual void BuildPhrases();

    virtual void Randomize(std::mt19937& rng) {Q_UNUSED(rng);};
};


class TSimplifyProblem : public TProblem
{
    void Assign(const TSimplifyProblem& S);
public:
    TSimplifyProblem();

    virtual string GetTask();
    virtual string GetShortTask();
    virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer); //возвращает непосредственно само решение без дублирования условия

    virtual string GetClassName() { return "TSimplifyProblem";}
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

#endif // TPROBLEM_H

