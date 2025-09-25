#ifndef TSYSTEMOFEQUATIONS_H
#define TSYSTEMOFEQUATIONS_H

#include "Base/tphrases.h"
#include "tproblem.h"

class TSystemOfEquations : public TProblem {
    vector<vector<PNumeric>> Coefs;  // EqCount x VarsCount
    vector<PNumeric> RightSide;
    bool ReadData();  // разбирает Conditions и заполняет Coefs и RightSide

    void Assign(const TSystemOfEquations& E);
    bool CheckAlwaysTrue(int EqNumber);  // возвращает истину, если уравнение номер EqNumber имеет вид 0 = 0
    bool CheckAlwaysFalse(int EqNumber);  // возвращает истину, если уравнение номер EqNumber имеет вид 0 = 1 или 1 = 0
    int MaxID;  // используется в BeginAddEquatios и AddEquation
  public:
    vector<PNumeric> Answer;
    bool SolutionExists;         // истина, если решение существует
    vector<PNumeric> Variables;  // обозначения переменных
    vector<vector<int>> role;  // EqCount x (VarsCount+1) role[i][j] - идентификатор коэффициента в i-ом уравнении при j-ой
                               // переменной; role[i][VarsCount+1] - идентификатор свободного члена

    TSystemOfEquations(size_t VarsCount = 3, size_t EqCount = 3);
    TSystemOfEquations(const TSystemOfEquations& E);
    void operator=(const TSystemOfEquations& E);
    ~TSystemOfEquations();

    size_t VarsCount() {
        return Variables.size();
    };
    size_t EqCount;  // количество уравнений

    virtual string GetTask();
    virtual string GetShortTask();
    bool GetSystemSolution(
        std::shared_ptr<THTMLWriter> Writer);  // возвращает непосредственно само решение без дублирования условия
    virtual bool
        GetSolution(std::shared_ptr<THTMLWriter> Writer);  // возвращает непосредственно само решение без дублирования условия

    virtual string GetClassName() {
        return "TSystemOfEquations";
    };
    virtual void BuildPhrases();

    void ExcludeSimilarEquations();  // исключает из системы одинаковые уравнения
    void Substitute(size_t VariableNumber,
                    const TNumeric& Expression);  // подставляет вместо переменной Substitute выражение Expression
    void RemoveEquation(size_t EqNumber);  // удаляет уравнение из системы

    virtual void SaveToFile(ofstream& f);
    virtual void LoadFromFile(ifstream& f);

    void BeginAddEquations();
    bool AddEquation(
        const TNumeric& N);  // добавляет уравнение к системе. N должно быть типа a Variables[0] + b Variables[1] + ... = d
    void EndAddEquations();
    virtual vector<string> GetKeyWords();

    void Randomize(std::mt19937& rng);
};

#endif  // TSYSTEMOFEQUATIONS_H
