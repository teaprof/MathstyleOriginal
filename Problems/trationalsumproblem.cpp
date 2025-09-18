#include <sstream>
#include "Base/polynom.h"
#include "TPolynomProblems.h"
#include "trationalsumproblem.h"

size_t TRationalSumConditions::GetPStartIndex(size_t N)
{
    //N must be in set {0, 1, 2, 3}
    if(N == 0) return 0;
    else return GetPStartIndex(N-1)+MaxPower+1;
}

size_t TRationalSumConditions::GetQStartIndex(size_t N)
{
    //N must be in set {0, 1, 2, 3}
    if(N == 0) return GetPStartIndex(3)+MaxPower+1;
    else return GetQStartIndex(N-1)+MaxPower+1;
}

void TRationalSumConditions::SetSigns(int Sign1, int Sign2)
{
    int Op1, Op2;

    if(Sign1 == 1)Op1 = OperatorSum;
    else Op1 = OperatorMinus;

    if(Sign2 == 1)Op2 = OperatorSum;
    else Op2 = OperatorMinus;

    Conditions->Operands[0].Operator = Op1;
    Conditions->Operands[1].Operator = Op2;

    this->Sign1 = Sign1;
    this->Sign2 = Sign2;
}


TRationalSumConditions::TRationalSumConditions(size_t MaxPower, int Sign1, int Sign2) : TProblem()
{
    this->MaxPower = MaxPower;
    TNumeric Res;
    Res.Operator = OperatorEqual;
    TNumeric Left, Right;
    Left.Operator = OperatorSum;
    Left.OperandsPushback(MakeFrac(GetPolynom(MaxPower, GetPStartIndex(0)), GetPolynom(MaxPower, GetQStartIndex(0))));
    Left.OperandsPushback(MakeFrac(GetPolynom(MaxPower, GetPStartIndex(1)), GetPolynom(MaxPower, GetQStartIndex(1))));
    Right.Operator = OperatorSum;
    Right.OperandsPushback(MakeFrac(GetPolynom(MaxPower, GetPStartIndex(2)), GetPolynom(MaxPower, GetQStartIndex(2))));
    Right.OperandsPushback(MakeFrac(GetPolynom(MaxPower, GetPStartIndex(3)), GetPolynom(MaxPower, GetQStartIndex(3))));
    Res.OperandsPushback(Left);
    Res.OperandsPushback(Right);
    Conditions = std::make_shared<TNumeric>(Res);

    if(MaxPower == 1)
    {
        int SampleCoefs[] = {-3, 1, +3, 1, +6, 1, -6, 1, -1, 1,  +1, 1, +2, 1, -2, 1};
        for(size_t i = 0; i < 16; i++)
        {
            char Buf[20];
            sprintf(Buf, "%d", SampleCoefs[i]);
            Conditions->GetByID(i).value().get().K = Buf;
        };
    } else {
        for(size_t i = 0; i < (MaxPower+1)*10; i++) //берём с запасом
        {
            if(Conditions->hasID(i)) {
                auto v = Conditions->GetByID(i);;
                if(v.has_value())
                {
                    TNumeric& P = v.value().get();
                    char Buf[20];
                    sprintf(Buf, "%d", (int) i);
                    P.K = Buf;
                };
            }
        }
    }
    SetSigns(Sign1, Sign2);
}

TRationalSumConditions::TRationalSumConditions(const TRationalSumConditions& R, int Sign1, int Sign2) : TProblem(R)
{
    this->MaxPower = MaxPower;
    Conditions = std::make_shared<TNumeric>(*R.Conditions);
    SetSigns(Sign1, Sign2);
}


TRationalSumConditions::~TRationalSumConditions()
{
//nothing to do
}

vector<TNumeric> TRationalSumConditions::GetCoefs(size_t StartID)
{
vector<TNumeric> Res;
    Res.assign(MaxPower+1, TNumeric(0));
    for(size_t power = 0; power <= MaxPower; power++)
    {
        Res[power] = *(Conditions->GetByID(StartID + power));
    }
    return Res;
}
vector<TNumeric> TRationalSumConditions::GetPCoef(size_t N)
{
    return GetCoefs(GetPStartIndex(N));
}

vector<TNumeric> TRationalSumConditions::GetQCoef(size_t N)
{
    return GetCoefs(GetQStartIndex(N));
}
TPolynom TRationalSumConditions::GetP(size_t N)
{
    return TPolynom(GetPCoef(N));
}

TPolynom TRationalSumConditions::GetQ(size_t N)
{
    return TPolynom(GetQCoef(N));
}


void TRationalSumConditions::Randomize(std::mt19937& rng)
{
    std::uniform_int_distribution<int> dist(-20, 20);
    for(size_t i = 0; i <= MaxPower; i++)
        for(size_t j = 0; j < 4; j++)
        {
            int ID = GetPStartIndex(j) + i;
            TNumeric& N = Conditions->GetByID(ID).value().get();
            N = TNumeric(dist(rng));
            N.SetEditableFlags(ConstAllowed);
            N.ID = ID;

            ID = GetQStartIndex(j) + i;
            TNumeric& N2 = Conditions->GetByID(ID).value().get();
            N2 = TNumeric(dist(rng));
            N2.SetEditableFlags(ConstAllowed);
            N2.ID = ID;
        }
}

//*******************************************************************************************************************
TRationalSumEquality::TRationalSumEquality(size_t MaxPower, int Sign1, int Sign2) : TRationalSumConditions(MaxPower, Sign1, Sign2), TEquality()
{
    CanRandomize = true;
    BuildPhrases();
}

TRationalSumEquality::TRationalSumEquality(const TRationalSumEquality& R, int Sign1, int Sign2) : TRationalSumConditions(R, Sign1, Sign2), TEquality()
{
    BuildPhrases();
}

void TRationalSumEquality::BuildPhrases()
{
    if(MyTranslator.CheckDictionary(GetClassName()))return;
    MyTranslator.AddDictionary(GetClassName());
    MyTranslator.AddEng("Finding the range of function definition: all denominators should be non-zero.");
    MyTranslator.AddRus("Ищем область определения: все знаменатели должны быть ненулевыми");
    MyTranslator.AddEng("The range of function definition: %N");
    MyTranslator.AddRus("Область определения: %N");
    MyTranslator.AddEng("The range of definition is empty. So there is no solution.");
    MyTranslator.AddRus("Область определения пуста. Следовательно, решений нет.");
    MyTranslator.AddEng("Reducing to common denominator: ");
    MyTranslator.AddRus("Приводим к общему знаменателю: ");
    MyTranslator.AddEng("Carrying all to the left side and reducing to common denominator");
    MyTranslator.AddRus("Переносим все в левую часть и приводим к общему знаменателю");
    MyTranslator.AddEng("Answer taking into account rangle of definition: ");
    MyTranslator.AddRus("Учитывая область допустимых значений, выписываем ответ");
    MyTranslator.AddEng("Solve rational equality with polynoms of %d power");
    MyTranslator.AddRus("Решить рациональное уравнение с многочленами %d степени");
    MyTranslator.AddEng("rational equality of %d power");
    MyTranslator.AddRus("рациональное уравнение %d степени");
}

TRationalSumEquality::~TRationalSumEquality()
{

}

string TRationalSumEquality::GetTask()
{
    char Buf[128];
    sprintf(Buf, MyTranslator.tr("Solve rational equality with polynoms of %d power").c_str(), MaxPower);
    return string(Buf);
}

string TRationalSumEquality::GetShortTask()
{
    char Buf[128];
    sprintf(Buf, MyTranslator.tr("rational equality of %d power").c_str(), MaxPower);
    return string(Buf);
}

bool TRationalSumEquality::GetSolution(THTMLWriter* Writer)
{
TPolynom P1 = GetP(0);
TPolynom P2 = GetP(1);
TPolynom P3 = GetP(2);
TPolynom P4 = GetP(3);

TPolynom Q1 = GetQ(0);
TPolynom Q2 = GetQ(1);
TPolynom Q3 = GetQ(2);
TPolynom Q4 = GetQ(3);

TIntervalsSet ODZ(TInterval(NumericMinusInf, NumericPlusInf, false, false));

    //Находим ОДЗ
    if(Writer)Writer->AddParagraph("Finding the range of function definition: all denominators should be non-zero.");
    TPolynomialEquality E1(Q1);
    if(E1.GetSolution(Writer))
    {
       ODZ = ODZ - E1.Result;
    };
    TPolynomialEquality E2(Q2);
    if(E2.GetSolution(Writer))
    {
        ODZ = ODZ - E2.Result;
    };
    TPolynomialEquality E3(Q3);
    if(E3.GetSolution(Writer))
    {
        ODZ = ODZ - E3.Result;
    };
    TPolynomialEquality E4(Q4);
    if(E4.GetSolution(Writer))
    {
        ODZ = ODZ - E4.Result;
    };

    if(Writer)
        Writer->AddParagraph("The range of function definition: %N", MakeBelongsTo(TNumeric("x"), ODZ.GetNumeric()));

    if(ODZ.Intervals.size() == 0)
    {
        if(Writer)
        {
            Writer->BeginParagraph();
            Writer->Add("The range of definition is empty. So there is no solution.");
            Writer->AddFormula(MakeBelongsTo(TNumeric("x"), EmptySet));
            Writer->EndParagraph();
        }
        return true;
    } else {        
        TNumeric Left, Right;
        Left = (P1.GetNumeric()*Q2.GetNumeric()+P2.GetNumeric()*Q1.GetNumeric())/(Q1.GetNumeric()*Q2.GetNumeric());
        Right = (P3.GetNumeric()*Q4.GetNumeric()+P4.GetNumeric()*Q3.GetNumeric())/(Q3.GetNumeric()*Q4.GetNumeric());
        if(Writer)
        {
            Writer->BeginParagraph();
            Writer->AddParagraph("Reducing to common denominator: ");
            Writer->AddFormula(MakeEquality(Left, Right));
            Writer->EndParagraph();
        };
        TPolynom LeftP, LeftQ, RightP, RightQ;
        TPolynom LeftPSimplified, LeftQSimplified, RightPSimplified, RightQSimplified;
        if(Sign1 == +1)
            LeftP = P1*Q2+P2*Q1;
        else
            LeftP = P1*Q2-P2*Q1;
        LeftQ = Q1*Q2;
        if(Sign2 == +1)
            RightP = P3*Q4+P4*Q3;
        else
            RightP = P3*Q4-P4*Q3;
        RightQ = Q3*Q4;
        if(Writer)Writer->AddFormula(MakeEquality(LeftP.GetNumeric()/LeftQ.GetNumeric(), RightP.GetNumeric()/RightQ.GetNumeric()));

        TPolynom BigL = LeftP*RightQ - RightP*LeftQ;
        if(Writer)
        {
            Writer->BeginParagraph();
            Writer->Add("Carrying all to the left side and reducing to common denominator");
            Writer->AddFormula(MakeEquality(BigL.GetNumeric(), TNumeric("0")));
            Writer->EndParagraph();
        };

        TPolynomialEquality PE(BigL, false);        
        if(PE.GetSolution(Writer))
        {

                //копируем решение с учётом ОДЗ
            TIntervalsSet Res = PE.Result * ODZ;
            if(Writer)
            {
                Writer->BeginParagraph();
                Writer->Add("Answer taking into account rangle of definition: ");
                Writer->AddFormula(MakeBelongsTo(TNumeric("x"), Res.GetNumeric()));
                Writer->EndParagraph();
            }
            return true;
        } return false;
    };
}

vector<TNumeric> TRationalSumEquality::GetTypes(TNumeric* N)
{
vector<TNumeric> Res;
    if(N == &Conditions->Operands[0])
    {
        Res.push_back(*(TRationalSumEquality(*this, +1, Sign2).Conditions));
        Res.push_back(*(TRationalSumEquality(*this, -1, Sign2).Conditions));
    };
    if(N == &Conditions->Operands[1])
    {
        Res.push_back(*(TRationalSumEquality(*this, Sign1, +1).Conditions));
        Res.push_back(*(TRationalSumEquality(*this, Sign1, -1).Conditions));
    };
    return Res;
}

void TRationalSumEquality::SetType(TNumeric* N, size_t Type)
{
 vector<TNumeric> Res;
    if(N == &Conditions->Operands[0])
    {
        switch(Type)
        {
            case 0: SetSigns(+1, Sign2); break;
            case 1: SetSigns(-1, Sign2); break;
        };
    };
    if(N == &Conditions->Operands[1])
    {
        switch(Type)
        {
            case 0: SetSigns(Sign1, +1); break;
            case 1: SetSigns(Sign1, -1); break;
        };
    };
}
vector<string> TRationalSumEquality::GetKeyWords()
{
    if(MyTranslator.CheckDictionary(GetClassName()+"Keywords") == false)
    {
        MyTranslator.AddDictionary(GetClassName()+"Keywords");
        MyTranslator.AddEng("equality");
        MyTranslator.AddRus("уравнение");
        MyTranslator.AddEng("rational");
        MyTranslator.AddRus("рациональная");
    }
vector<string> Res;
    Res.push_back(MyTranslator.tr("rational"));
    Res.push_back(MyTranslator.tr("equality"));
    return Res;
}
