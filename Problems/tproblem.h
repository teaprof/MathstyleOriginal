#ifndef TPROBLEM_H
#define TPROBLEM_H
#include <memory>
#include <random>
#include <string>

#include "Base/THTMLWriter.h"
#include "Base/arithmetic.h"
#include "Base/tinterval.h"
#include "Base/tline.h"
#include "Base/tphrases.h"
using namespace std;

extern TNumeric X_1;
extern TNumeric X_2;

typedef unsigned long int seed_t;
class TRandom;

class TProblem {
  public:
    bool CanRandomize{false};
    std::shared_ptr<TNumeric> Conditions;

    TProblem();
    TProblem(const TProblem& P);
    virtual ~TProblem();
    TProblem& operator=(const TProblem& other);

    virtual string GetTask() {
        return {};
    }
    virtual string GetShortTask() {
        return {};
    }
    virtual std::vector<std::shared_ptr<TNumeric>> getEditables() {
        return {};
    }
    virtual bool Solve(std::shared_ptr<THTMLWriter> Writer);

    // возвращает непосредственно само решение без дублирования условия
    virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer) {
        Q_UNUSED(Writer);
        return false;
    }

    // возвращает непосредственно само решение без дублирования условия
    virtual vector<std::shared_ptr<TNumeric>> GetTypes() {
        return GetTypes(Conditions);  // todo: why?
    }
    // выдаёт все возможные типы задачи, когда кликается по объекту N
    virtual vector<std::shared_ptr<TNumeric>> GetTypes(std::shared_ptr<const TNumeric> N);
    virtual void SetType(std::shared_ptr<TNumeric> N, size_t Type) {
        Q_UNUSED(N);
        Q_UNUSED(Type);
    }

    virtual vector<string> GetKeyWords() {
        return {};
    }  // выдает ключевые слова

    virtual void SaveToFile(ofstream& f);
    virtual void LoadFromFile(ifstream& f);

    virtual string GetClassName() {
        return "TProblem";
    }
    virtual void BuildPhrases();

    virtual void Randomize(std::mt19937& rng) {
        Q_UNUSED(rng);
    };
};

class TSimplifyProblem : public TProblem {
    void Assign(const TSimplifyProblem& S);

  public:
    TSimplifyProblem();

    virtual string GetTask();
    virtual string GetShortTask();
    virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer);  // возвращает непосредственно само решение без дублирования условия
    virtual std::vector<std::shared_ptr<TNumeric>> getEditables();

    virtual string GetClassName() {
        return "TSimplifyProblem";
    }
    virtual void BuildPhrases();
    virtual vector<string> GetKeyWords();
};

class TEquality {
  public:
    TIntervalsSet Result;
};

class TInequality {
  public:
    TIntervalsSet Result;
};

#endif  // TPROBLEM_H
