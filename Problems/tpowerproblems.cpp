#include <sstream>
#include "tpowerproblems.h"

size_t TPowerProblemConditions::GetStartID(size_t Part)
{
    if(Part==0) return PowerStartID;
    else return GetStartID(Part-1) + MaxPower + 1;
}

TPowerProblemConditions::TPowerProblemConditions(size_t MaxPower)
{
    this->MaxPower = MaxPower;
    TNumeric Left;
    TNumeric Right;
    TNumeric A(2), B(3);
    A.EditableFlags = ConstAllowed;
    A.ID = LeftBaseID;
    B.EditableFlags = ConstAllowed;
    B.ID = RightBaseID;
    Left = A^GetPolynom(MaxPower, GetStartID(0));
    Right = B^GetPolynom(MaxPower, GetStartID(1));
    Conditions = std::make_shared<TNumeric>();
    Conditions->Operator = OperatorEqual;
    Conditions->OperandsPushback(Left);
    Conditions->OperandsPushback(Right);
}

TPowerProblemConditions::~TPowerProblemConditions()
{

}
TNumeric* TPowerProblemConditions::GetLeftBase()
{
    auto v = Conditions->GetByID(LeftBaseID);
    if(v.has_value())
        return &(v->get());
    return nullptr;
}

TNumeric* TPowerProblemConditions::GetRightBase()
{
    auto v = Conditions->GetByID(RightBaseID);
    if(v.has_value())
        return &(v->get());
    return nullptr;
}

TPolynom TPowerProblemConditions::GetLeftP()
{
vector<TNumeric> Coef;
    Coef.assign(MaxPower+1, TNumeric(0));
    for(size_t i = 0; i <= MaxPower; i++)
    {
        Coef[i] = *(Conditions->GetByID(GetStartID(0) + i));
    };
    return TPolynom(Coef);
}

TPolynom TPowerProblemConditions::GetRightP()
{
vector<TNumeric> Coef;
    Coef.assign(MaxPower+1, TNumeric(0));
    for(size_t i = 0; i <= MaxPower; i++)
    {
        Coef[i] = *(Conditions->GetByID(GetStartID(1) + i));
    };
    return TPolynom(Coef);
}

void TPowerProblemConditions::SaveToFile(ofstream &f)
{
__int16 MaxPower = this->MaxPower;
    f.write((char*)&MaxPower, sizeof(MaxPower));
    TProblem::SaveToFile(f);
}

void TPowerProblemConditions::LoadFromFile(ifstream &f)
{
__int16 MaxPower;
    f.read((char*)&MaxPower, sizeof(MaxPower));
    this->MaxPower = MaxPower;
    TProblem::LoadFromFile(f);
}

void TPowerProblemConditions::Randomize(std::mt19937& rng)
{
    std::uniform_int_distribution<int> dist(-20, 20);
    for(size_t i = 0; i <= MaxPower; i++)
    {
        int ID = GetStartID(0) + i;
        TNumeric& N = Conditions->GetByID(ID).value().get();
        N = TNumeric(dist(rng));
        N.ID = ID;
        N.SetEditableFlags(ConstAllowed);
    };
    for(size_t i = 0; i <= MaxPower; i++)
    {
        int ID = GetStartID(1) + i;
        TNumeric& N = Conditions->GetByID(ID).value().get();
        N = TNumeric(dist(rng));
        N.ID = ID;
        N.SetEditableFlags(ConstAllowed);
    };
    //building the bases A and B
    vector<int> Primes;
    Primes.push_back(2); Primes.push_back(3); Primes.push_back(5); Primes.push_back(7); Primes.push_back(11); Primes.push_back(13);    
    int base = Primes[rng() % Primes.size()];
    std::uniform_int_distribution<int> dist2(1, 3);
    int d1 = dist2(rng);
    int d2;
    do
    {
        d2 = dist2(rng);
    } while (d1 == d2);
    int base1 = base;
    while(d1-- > 0) base1 *= base;
    int base2 = base;
    while(d2-- > 0) base2 *= base;
    TNumeric *N = GetLeftBase();
    *N = TNumeric(base1);
    N->SetEditableFlags(ConstAllowed);
    N->ID = LeftBaseID;

    N = GetRightBase();
    *N = TNumeric(base2);
    N->SetEditableFlags(ConstAllowed);
    N->ID = RightBaseID;
}

//*************************************************************
TPowerEquality::TPowerEquality(size_t MaxPower) : TPowerProblemConditions(MaxPower), TEquality()
{
    CanRandomize = true;
    BuildPhrases();
}

TPowerEquality::~TPowerEquality()
{    

}

void TPowerEquality::BuildPhrases()
{
    if(MyTranslator.CheckDictionary(GetClassName()))return;
    MyTranslator.AddDictionary(GetClassName());
    MyTranslator.AddEng("Base of power function should be positive.");
    MyTranslator.AddRus("Основание степенной функции должно быть положительным");
    MyTranslator.AddEng("Finding logarithm of left and right parts.");
    MyTranslator.AddRus("Ищем логарифм от левой и правой частей");
    MyTranslator.AddEng("Solve exponential equality with polynom of %d power");
    MyTranslator.AddRus("Решить показательное уравнение с многочленом %d степени");
    MyTranslator.AddEng("exponential with %d power polynom");
    MyTranslator.AddRus("Показательное с многочленом степени %d");
}
vector<string> TPowerEquality::GetKeyWords()
{
    if(MyTranslator.CheckDictionary(GetClassName()+"Keywords") == false)
    {
        MyTranslator.AddDictionary(GetClassName()+"Keywords");
        MyTranslator.AddEng("power");
        MyTranslator.AddRus("степенная");
        MyTranslator.AddEng("equality");
        MyTranslator.AddRus("уравнение");
    }
vector<string> Res;
    Res.push_back(MyTranslator.tr("equality"));
    Res.push_back(MyTranslator.tr("power"));
    return Res;
}


string TPowerEquality::GetTask()
{
    char Buf[128];
    sprintf(Buf, MyTranslator.tr("Solve exponential equality with polynom of %d power").c_str(), MaxPower);
    return string(Buf);
}

string TPowerEquality::GetShortTask()
{
    char Buf[128];
    sprintf(Buf, MyTranslator.tr("exponential with %d power polynom").c_str(), MaxPower);
    return string(Buf);
}

bool TPowerEquality::GetSolution(THTMLWriter* Writer)
{
TPolynom L = GetLeftP();
TPolynom R = GetRightP();
TNumeric A = *GetLeftBase();
TNumeric B = *GetRightBase();
    if(A.Calculate() < 0 || B.Calculate() < 0)
    {
        if(Writer)Writer->WriteError("Base of power function should be positive.");
        return false;
    }
    if(Writer)Writer->AddParagraph("Finding logarithm of left and right parts.");
    if(Writer)Writer->AddFormula(MakeEquality(MakeProd(L.GetNumeric(), MakeLn(A)), MakeProd(R.GetNumeric(), MakeLn(B))));
    if(A.Calculate() == 1)
    {
        if(B.Calculate() == 1)
        {
            //Случай L*0 = R*0, решение - любое число
            Result.Intervals.push_back(IntervalAllRealNumbers);
            if(Writer)Writer->AddFormula(MakeBelongsTo(TNumeric("x"), NumericAllReal));
        } else {
            //Случай L*0 = R*lnB
            TPolynomialEquality PE(R, false);
            if(PE.GetSolution(Writer))
            {
                Result= PE.Result;
            } else {
                return false;
            }
        }
    } else {
        if(B.Calculate() == 1)
        {
        //случай L*lnA = R*0
            TPolynomialEquality PE(L, false);            
            if(PE.GetSolution(Writer))
            {
                Result= PE.Result;
            } else {
                return false;
            }
        } else {
        //общий случай L*lnA = R * lnB
            TNumeric Coef = MakeLog(B, A);
            TNumeric CoefSimplified = Coef.Simplify();
            CoefSimplified = TNumeric(1);
            CoefSimplified = Coef;
            TPolynom PP(L - R*CoefSimplified);
            if(Writer)Writer->AddFormula(MakeEquality(PP.GetNumeric(), TNumeric(0)));
            TPolynomialEquality PE(PP);
            if(PE.GetSolution(Writer))
            {
                Result= PE.Result;
            } else {
                return false;
            }
        }

    }


    return true;
}
