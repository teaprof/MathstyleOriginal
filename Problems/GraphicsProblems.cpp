#include "Base/taxes.h"
#include "GraphicsProblems.h"

TPolynomialGraphic::TPolynomialGraphic(size_t MaxPower) : TPolynomConditions(MaxPower)
{
}

TPolynomialGraphic::~TPolynomialGraphic()
{
};

string TPolynomialGraphic::GetTask()
{
    string str("Построить график многочлена");
    return str;
}

string TPolynomialGraphic::GetShortTask()
{
    return "График многочлена";
}
bool TPolynomialGraphic::GetSolution(vector<TRectangleElement*> *Lines)
{
TAxes *A = new TAxes;
    A->Width = 300;
    A->Height = 300;
    A->AddLine(-100.0, 0.0, 100.0, 0.0);    
    A->AddLine(0.0, -100.0, 0.0, +100.0);
vector<double> X;
vector<double> Y;
const size_t NPoints = 100;
    X.assign(NPoints, 0);
double XMin = -10;
double XMax = 10;
    for(size_t i = 0; i < NPoints; i++)
        X[i] = XMin + (XMax-XMin)*i/(NPoints-1);
    Y = GetP().Calculate(X);
    A->AddCurve(X, Y);
TText* T = new TText(string("График"));
    if(Lines)Lines->push_back(T);
//TText* R = (TText*)Lines->at(0);
    if(Lines)Lines->push_back(A);
    return true;
}
