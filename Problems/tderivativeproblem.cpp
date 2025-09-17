#include "tderivativeproblem.h"
#include "RandomFunction.h"

TDerivativeProblem::TDerivativeProblem()
{
    CanRandomize = true;
    Conditions = new TNumeric;
    Conditions->Operator = OperatorArctg;
    Conditions->OperandsPushback(MakePow(TNumeric("x"), TNumeric(2))+TNumeric(2));
    //Conditions->Operator = OperatorFrac;
    //Conditions->OperandsPushback(TNumeric(1));
    //Conditions->OperandsPushback(MakeLog(TNumeric("x"), TNumeric(5)));
    Conditions->SetEditableFlags(ConstAllowed | FunctionsAllowed);
    BuildPhrases();
}

TDerivativeProblem::TDerivativeProblem(const TDerivativeProblem& D) : TProblem(D)
{
    this->Assign(D);
    BuildPhrases();
}

TDerivativeProblem::~TDerivativeProblem()
{
}

void TDerivativeProblem::BuildPhrases()
{
    if(MyTranslator.CheckDictionary(GetClassName()))return;
    MyTranslator.AddDictionary(GetClassName());
    MyTranslator.AddEng("Derivative");
    MyTranslator.AddRus("Производная");
    MyTranslator.AddEng("After simplifying");
    MyTranslator.AddRus("После упрощения");
    MyTranslator.AddEng("Find derivative of the function");
    MyTranslator.AddRus("Найти производную функции");
    MyTranslator.AddEng("Derivative");
    MyTranslator.AddRus("производная");
}

vector<string> TDerivativeProblem::GetKeyWords()
{
    if(MyTranslator.CheckDictionary(GetClassName()+"Keywords") == false)
    {
        MyTranslator.AddDictionary(GetClassName()+"Keywords");
        MyTranslator.AddEng("derivative");
        MyTranslator.AddRus("производная");
    }
vector<string> Res;
    Res.push_back(MyTranslator.tr("derivative"));
    return Res;
}


void TDerivativeProblem::Assign(const TDerivativeProblem& S)
{
    if(Conditions) delete Conditions;
    Conditions = new TNumeric;
    *this->Conditions = *S.Conditions;
    /*if(Solution) delete Solution;
    Solution = 0;
    if(S.Solution)
    {
        Solution = new TLines;
        *Solution = *S.Solution;
    }*/
}

string TDerivativeProblem::GetTask()
{
    return MyTranslator.tr("Find derivative of the function");
}

string TDerivativeProblem::GetShortTask()
{
    return MyTranslator.tr("Derivative");
}

bool TDerivativeProblem::GetSolution(THTMLWriter* Writer)
{
    TNumeric XDeriv = Conditions->Derivative();
    TNumeric XDerivSimplified = XDeriv.Simplify();
    if(Writer)
    {
        Writer->BeginParagraph();
        Writer->AddFormula(XDeriv);
        Writer->Add("After simplifying");
        Writer->AddFormula(XDerivSimplified);
        Writer->EndParagraph();
    };
    return true;
}

void TDerivativeProblem::Randomize(TRandom *Rng)
{
TNumeric Res;
TRandomFunction RF(Rng);
    switch(Rng->Random(0, 3))
    {
        case 0:
            Res.Operator = OperatorFrac;
            Res.OperandsPushback(RF.RandomFunction(RF.AllowTrigFunction | RF.AllowSum | RF.AllowLnFunction | RF.AllowExpFunction | RF.AllowInverseTrigFunction, 1));
            Res.OperandsPushback(RF.RandomFunction(RF.AllowTrigFunction | RF.AllowSum | RF.AllowLnFunction | RF.AllowExpFunction | RF.AllowInverseTrigFunction, 1));
        break;
        case 1:
            Res.Operator = OperatorPow;
            Res.OperandsPushback(RF.RandomFunction(RF.AllowTrigFunction | RF.AllowSum | RF.AllowLnFunction | RF.AllowExpFunction | RF.AllowInverseTrigFunction, 2));
            Res.OperandsPushback(RF.RandomFunction(RF.AllowTrigFunction | RF.AllowSum, 1));
        break;
        case 2:
            Res.Operator = OperatorProd;
            Res.OperandsPushback(RF.RandomFunction(RF.AllowTrigFunction | RF.AllowSum | RF.AllowLnFunction | RF.AllowExpFunction | RF.AllowInverseTrigFunction, 1));
            Res.OperandsPushback(RF.RandomFunction(RF.AllowTrigFunction | RF.AllowSum | RF.AllowLnFunction | RF.AllowExpFunction | RF.AllowInverseTrigFunction, 1));
        break;
        case 3:
            Res.Operator = OperatorSum;
            Res.OperandsPushback(RF.RandomFunction(RF.AllowTrigFunction | RF.AllowSum | RF.AllowLnFunction | RF.AllowExpFunction | RF.AllowInverseTrigFunction, 2));
            Res.OperandsPushback(RF.RandomFunction(RF.AllowTrigFunction | RF.AllowSum | RF.AllowLnFunction | RF.AllowExpFunction | RF.AllowInverseTrigFunction, 2));
        break;
    }
    Res.SetEditableFlags(ConstAllowed | FunctionsAllowed);
    *Conditions = Res;
}
