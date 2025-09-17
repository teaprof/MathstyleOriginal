#ifndef GRAPHICSPROBLEMS_H
#define GRAPHICSPROBLEMS_H
#include "tproblem.h"
#include "TPolynomProblems.h"

class TPolynomialGraphic : public TPolynomConditions
{
    public:
        TPolynomialGraphic(size_t MaxPower = 4);
        ~TPolynomialGraphic();

        virtual string GetTask();
        virtual string GetShortTask();
        virtual bool GetSolution(vector<TRectangleElement*> *Lines); //возвращает непосредственно само решение без дублирования условия

};

#endif // GRAPHICSPROBLEMS_H
