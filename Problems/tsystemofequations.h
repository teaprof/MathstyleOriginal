#ifndef TSYSTEMOFEQUATIONS_H
#define TSYSTEMOFEQUATIONS_H

#include "tproblem.h"
#include "Base/tphrases.h"


class TSystemOfEquations : public TProblem
{
    vector< vector<TNumeric> > Coefs; //EqCount x VarsCount
    vector<TNumeric> RightSide;
    bool ReadData(); //разбирает Conditions и заполняет Coefs и RightSide

    void Assign(const TSystemOfEquations& E);
    bool CheckAlwaysTrue(int EqNumber); //возвращает истину, если уравнение номер EqNumber имеет вид 0 = 0
    bool CheckAlwaysFalse(int EqNumber); //возвращает истину, если уравнение номер EqNumber имеет вид 0 = 1 или 1 = 0
    int MaxID; //используется в BeginAddEquatios и AddEquation        
public:    
    vector<TNumeric> Answer;
    bool SolutionExists; //истина, если решение существует
    vector<TNumeric> Variables; //обозначения переменных
    vector< vector <int> > ID; //EqCount x (VarsCount+1) ID[i][j] - идентификатор коэффициента в i-ом уравнении при j-ой переменной; ID[i][VarsCount+1] - идентификатор свободного члена

    TSystemOfEquations(size_t VarsCount = 3, size_t EqCount = 3);
    TSystemOfEquations(const TSystemOfEquations& E);
    void operator=(const TSystemOfEquations& E);
    ~TSystemOfEquations();

    size_t VarsCount() { return Variables.size();};
    size_t EqCount; //количество уравнений

    virtual string GetTask();
    virtual string GetShortTask();
    bool GetSystemSolution(THTMLWriter* Writer); //возвращает непосредственно само решение без дублирования условия
    virtual bool GetSolution(THTMLWriter* Writer); //возвращает непосредственно само решение без дублирования условия

    virtual string GetClassName() { return "TSystemOfEquations";};
    virtual void BuildPhrases();

    void ExcludeSimilarEquations(); //исключает из системы одинаковые уравнения
    void Substitute(size_t VariableNumber, const TNumeric& Expression); //подставляет вместо переменной Substitute выражение Expression
    void RemoveEquation(size_t EqNumber); //удаляет уравнение из системы

    virtual void SaveToFile(ofstream &f);
    virtual void LoadFromFile(ifstream &f);

    void BeginAddEquations();
    bool AddEquation(const TNumeric& N); //добавляет уравнение к системе. N должно быть типа a Variables[0] + b Variables[1] + ... = d
    void EndAddEquations();
    virtual vector<string> GetKeyWords();

    void Randomize(TRandom *Rng);
};

#endif // TSYSTEMOFEQUATIONS_H
