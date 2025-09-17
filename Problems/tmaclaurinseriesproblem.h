#ifndef TMACLAURINSERIESPROBLEM_H
#define TMACLAURINSERIESPROBLEM_H

#include "tproblem.h"

//2do: не раскладывает в ряд Маклорена: (2/3 x + 10/27 x^3)/(2/3 x + 4/9 x^3) - завершается с ошибкой

class TPolynomSeries
{
    public:
        vector<TNumeric> Coefs;
        int RemainderPower; //eps*x^RemainderPower

        TPolynomSeries();
        TPolynomSeries(const TPolynomSeries& P);
        TPolynomSeries(const TNumeric& Coef0);
        TPolynomSeries(const vector<TNumeric> &Coefs, int LastTerm = -1);
        TPolynomSeries Substitute(const TPolynomSeries& P, int LastTerm = -1);
        TPolynomSeries operator+(const TPolynomSeries& P);
        TPolynomSeries operator+(const TNumeric& N);
        TPolynomSeries operator-(const TPolynomSeries& P);
        TPolynomSeries operator-(const TNumeric& N);
        TPolynomSeries operator*(const TPolynomSeries& P);
        TPolynomSeries operator*(const TNumeric& N);
        TPolynomSeries operator=(const TPolynomSeries& P);

        void Cut(int RemainderPower); //Отбрасывает все члены старше x^RemainderPower
        TNumeric GetNumeric(const string &var);
};


class TMaclaurinSeriesProblem : public TProblem
{
    void Assign(const TMaclaurinSeriesProblem& S);
public:
    size_t MaxPower; //степень, до которой раскладывать включительно
    TMaclaurinSeriesProblem(size_t MaxPower = 5);
    ~TMaclaurinSeriesProblem();
    TMaclaurinSeriesProblem(const TMaclaurinSeriesProblem& D);
    void operator=(const TMaclaurinSeriesProblem& D);

    virtual string GetTask();
    virtual string GetShortTask();

    bool ExpandIntoSeries(THTMLWriter *Writer, TPolynomSeries *Res, string var, const TNumeric& N, int LastTerm);

    virtual bool GetSolution(THTMLWriter* Writer); //возвращает непосредственно само решение без дублирования условия
    virtual void BuildPhrases();

    virtual string GetClassName() { return "TMaclaurinSeriesProblem";};

    virtual void SaveToFile(ofstream &f);
    virtual void LoadFromFile(ifstream &f);
    virtual vector<string> GetKeyWords();
};

#endif // TMACLAURINSERIESPROBLEM_H
