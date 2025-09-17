#include <string.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>
#include "tproblem.h"
#include "Base/polynom.h"


TProblem::TProblem()
{
    Conditions = 0;
    //Solution = 0;
    //HTMLWriter = 0;
    DuplicateToHTML = true;
    CanRandomize = false;
    BuildPhrases();    
}
TProblem::TProblem(const TProblem& P)
{
    if(P.Conditions)
        Conditions = new TNumeric(*P.Conditions);
    else
        Conditions = 0;    
    /*if(P.Solution)
        Solution = new TLines(*P.Solution);
    else
        Solution = 0;*/
    //Tr копировать не надо, так как он должен быть один и тот же у проблем (TProblem*) одного типа
}
TProblem::~TProblem()
{
    if(Conditions) delete Conditions;
    //if(Solution) delete Solution;
}

void TProblem::BuildPhrases()
{
    if(MyTranslator.CheckDictionary("TProblem"))return;
    MyTranslator.AddDictionary("TProblem");
    MyTranslator.AddEng("Solution");
    MyTranslator.AddRus("Решение");
}

bool TProblem::Solve(THTMLWriter *Writer)
{    
    //this->HTMLWriter = Writer;
    //ClearSolution();
    if(Writer)
    {
        Writer->PushTranslator(&MyTranslator);
        Conditions->DrawMouse = false;
        Writer->BeginDiv("cond");
        Writer->Add(GetTask());
        Writer->AddFormula(*Conditions);
        Writer->EndDiv();
        Writer->BeginDiv("sol");
        Writer->AddHeader1("Solution");
    };
bool res = GetSolution(Writer);
    if(Writer)
    {
            Writer->EndDiv();
            Writer->PopTranslator();
    };
    return res;
}

/*void TProblem::ClearSolution()
{
    if(Solution) delete Solution;
    Solution = new TLines;
}*/

/*void TProblem::AddLine(vector<TRectangleElement*>* Lines, int Ntabs, TRectangleElement* R)
{
    if(Lines == NULL)return;
    R->PaddingLeft = Ntabs*20;
    R->ColorIndex = Ntabs;
    Lines->push_back(R);
    if(HTMLWriter)
    {
        for(size_t i = 0; i < Lines->size(); i++)
            HTMLWriter->WriteRectangleElement(Lines->at(i));
    };
}



void TProblem::AddLine(string str)
{
TLine *Line = new TLine;
    Line->push_back(new THSpace);
    Line->push_back(new TText(str));
    if(Solution == 0) Solution = new TLines;
    Solution->AddLine(Line);
    if(HTMLWriter && DuplicateToHTML)
    {
        HTMLWriter->AddParagraph(str);
    };
}

void TProblem::AddHeader(string str)
{
    if(Solution == 0) Solution = new TLines;
    Solution->AddLine(new TText(str));
    if(HTMLWriter && DuplicateToHTML)HTMLWriter->Add(string("<h2>")+str+"</h2>");
}

void TProblem::AddLines(vector<TRectangleElement*> Lines)
{
    if(Solution == 0) Solution = new TLines;
    for(size_t i = 0; i < Lines.size(); i++)
    {
        TLine* L = new TLine;
        L->push_back(new THSpace);
        L->push_back(Lines[i]);
        Solution->AddLine(L);
    }
    if(HTMLWriter && DuplicateToHTML)
    {
        for(size_t i = 0; i < Lines.size(); i++)
            HTMLWriter->WriteRectangleElement(Lines[i]);
    };
}*/

vector<TNumeric> TProblem::GetTypes(TNumeric* N)
{
    Q_UNUSED(N);
vector<TNumeric> Empty;
    return Empty;
}

//********************************************************************************************************
//********************************************************************************************************
void TSimplifyProblem::Assign(const TSimplifyProblem& S)
{
    if(Conditions) delete Conditions;
    Conditions = new TNumeric;
    *this->Conditions = *S.Conditions;
    //if(Solution) delete Solution;
    //Solution = 0;
    /*if(S.Solution)
    {
        Solution = new TLines;
        *Solution = *S.Solution;
    }*/    
}

TSimplifyProblem::TSimplifyProblem() : TProblem()
{
    Conditions = new TNumeric;
    Conditions->Operator = OperatorSum;    
    Conditions->OperandsPushback(TNumeric("1"));
    Conditions->OperandsPushback(TNumeric("2"));
    Conditions->Assign("((x^2+(-1*x))+1");
    Conditions->SetEditableFlags(ConstAllowed | FunctionsAllowed);    
    BuildPhrases();
}

void TSimplifyProblem::BuildPhrases()
{
    if(MyTranslator.CheckDictionary("TSimplifyProblem"))return;
    MyTranslator.AddDictionary("TSimplifyProblem");
    MyTranslator.AddEng("Answer: %N");
    MyTranslator.AddRus("Ответ: %N");
    MyTranslator.AddEng("Simplify equation");
    MyTranslator.AddRus("Упростить выражение");
    MyTranslator.AddEng("Simplify equation");
    MyTranslator.AddRus("Упростить выражение");
    MyTranslator.AddEng("simplify");
    MyTranslator.AddRus("упростить");
}

TSimplifyProblem::TSimplifyProblem(const TSimplifyProblem &S) : TProblem(S)
{
    Assign(S);
}

TSimplifyProblem::~TSimplifyProblem()
{

}

string TSimplifyProblem::GetTask()
{
    return MyTranslator.tr("Simplify equation");
}

string TSimplifyProblem::GetShortTask()
{
    return MyTranslator.tr("simplify");
}

bool TSimplifyProblem::GetSolution(THTMLWriter* Writer)
{
    TNumeric XSimplified = Conditions->Simplify();
    if(Writer)
       Writer->AddParagraph("Answer: %N", XSimplified);
    return true;
}


vector<string> TSimplifyProblem::GetKeyWords()
{
    vector<string> Res;
    Res.push_back(MyTranslator.tr("simplify"));
    return Res;
};
//===============================================================================================
TRandom::TRandom()
{
    Multiplier = 1664525;
    Seed = 16807;
    Offset = 0;
    MaxSeed = 0xFFFFFFFF;
}
void TRandom::InitSeed(seed_t Seed)
{
    Seed = Seed % MaxSeed;
}

int TRandom::Random(int Max)
{
    return Random(0, Max);
}

int TRandom::Random(int Min, int Max)
{
    if(Min>Max) return Random(Max, Min);
    Seed = (Seed*Multiplier + Offset) % MaxSeed;
    int r = Min + (Seed % (Max - Min + 1));
    return r;
}
seed_t TRandom::GetRandomSeed()
{
    return Seed;
}

seed_t *TRandom::GetRandomSeedP()
{
    return &Seed;
}
