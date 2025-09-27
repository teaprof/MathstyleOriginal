#ifndef TDERIVATIVEPROBLEM_H
#define TDERIVATIVEPROBLEM_H

#include "tproblem.h"

class TDerivativeProblem : public TProblem {
    void Assign(const TDerivativeProblem& S);

  public:
    TDerivativeProblem();
    ~TDerivativeProblem();
    TDerivativeProblem(const TDerivativeProblem& D);
    void operator=(const TDerivativeProblem& D) {
        Assign(D);
    };

    virtual string GetTask();
    virtual string GetShortTask();
    virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer);  // возвращает непосредственно само решение без дублирования условия

    virtual string GetClassName() {
        return "TDerivativeProblem";
    };
    virtual void BuildPhrases();
    virtual vector<string> GetKeyWords();

    void Randomize(std::mt19937& rng);
};

#endif  // TDERIVATIVEPROBLEM_H
