#include <string.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>
#include "tproblem.h"
#include "Base/polynom.h"


TProblem::TProblem()
{
    Conditions = std::make_shared<TNumeric>();
    EditableFormula = std::make_shared<TEditableFormula>(Conditions);
    CanRandomize = false;
//    BuildPhrases();
}
TProblem::TProblem(const TProblem& P)
{
    Conditions = std::make_shared<TNumeric>(*P.Conditions);
    EditableFormula = std::make_shared<TEditableFormula>(*P.EditableFormula);
}
TProblem::~TProblem()
{
    // nothing to do
}
TProblem& TProblem::operator=(const TProblem &other) {
    if(other.Conditions)
        Conditions = std::make_shared<TNumeric>(*other.Conditions);
    else {
        Conditions = nullptr;
        assert(false);
    }
    return *this;
}


void TProblem::BuildPhrases()
{
    if(MyTranslator.CheckDictionary("TProblem"))return;
    MyTranslator.AddDictionary("TProblem");
    MyTranslator.AddEng("Solution");
    MyTranslator.AddRus("Решение");
}

bool TProblem::Solve(std::shared_ptr<THTMLWriter> Writer)
{    
    if(Writer)
    {
        Writer->PushTranslator(&MyTranslator);
        //Conditions->DrawMouse = false;
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

void TProblem::SaveToFile(ofstream &f)
{
    char ConditionsPresent;
    if(Conditions)
    {
        ConditionsPresent = 1;
        f.write((char*)&ConditionsPresent, sizeof(ConditionsPresent));
        Conditions->WriteToFile(f);
    } else {
        ConditionsPresent = 0;
        f.write((char*)&ConditionsPresent, sizeof(ConditionsPresent));
    }
}

void TProblem::LoadFromFile(ifstream &f)
{
    Conditions = std::make_shared<TNumeric>();
    char conditions_found;
    f.read(&conditions_found, sizeof(conditions_found));
    if(conditions_found == 1)
    {
        Conditions->LoadFromFile(f);
    }
}


vector<std::shared_ptr<TNumeric>> TProblem::GetTypes(std::shared_ptr<const TNumeric> N)
{
    Q_UNUSED(N);
    return {};
}

//********************************************************************************************************
//********************************************************************************************************
TSimplifyProblem::TSimplifyProblem() : TProblem()
{
    Conditions->operation = OperatorSum;
    Conditions->OperandsPushback(TNumeric("1"));
    Conditions->OperandsPushback(TNumeric("2"));
    Conditions->Assign("((x^2+(-1*x))+1");
    Conditions->SetEditableFlags(ConstAllowed | FunctionsAllowed);
    EditableFormula = std::make_shared<TEditableFormula>(Conditions);
    EditableFormula->addEditableChild(Conditions);
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

string TSimplifyProblem::GetTask()
{
    return MyTranslator.tr("Simplify equation");
}

string TSimplifyProblem::GetShortTask()
{
    return MyTranslator.tr("simplify");
}

bool TSimplifyProblem::GetSolution(std::shared_ptr<THTMLWriter> Writer)
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
}
