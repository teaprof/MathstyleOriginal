#include "tderivativeproblem.h"

#include "RandomFunction.h"

TDerivativeProblem::TDerivativeProblem() {
    CanRandomize = true;
    Conditions = std::make_shared<TNumeric>();
    Conditions->operation = OperatorArctg;
    Conditions->OperandsPushback(MakePow(TNumeric("x"), TNumeric(2)) + TNumeric(2));
    // Conditions->operation = OperatorFrac;
    // Conditions->OperandsPushback(TNumeric(1));
    // Conditions->OperandsPushback(MakeLog(TNumeric("x"), TNumeric(5)));
    Conditions->SetEditableFlags(ConstAllowed | FunctionsAllowed);
    BuildPhrases();
}

TDerivativeProblem::TDerivativeProblem(const TDerivativeProblem& D) : TProblem(D) {
    this->Assign(D);
    BuildPhrases();
}

TDerivativeProblem::~TDerivativeProblem() {}

void TDerivativeProblem::BuildPhrases() {
    if (MyTranslator.CheckDictionary(GetClassName()))
        return;
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

vector<string> TDerivativeProblem::GetKeyWords() {
    if (MyTranslator.CheckDictionary(GetClassName() + "Keywords") == false) {
        MyTranslator.AddDictionary(GetClassName() + "Keywords");
        MyTranslator.AddEng("derivative");
        MyTranslator.AddRus("производная");
    }
    vector<string> Res;
    Res.push_back(MyTranslator.tr("derivative"));
    return Res;
}

void TDerivativeProblem::Assign(const TDerivativeProblem& S) {
    Conditions = std::make_shared<TNumeric>(*S.Conditions);
}

string TDerivativeProblem::GetTask() {
    return MyTranslator.tr("Find derivative of the function");
}

string TDerivativeProblem::GetShortTask() {
    return MyTranslator.tr("Derivative");
}

bool TDerivativeProblem::GetSolution(std::shared_ptr<THTMLWriter> Writer) {
    TNumeric XDeriv = Conditions->Derivative();
    TNumeric XDerivSimplified = XDeriv.Simplify();
    if (Writer) {
        Writer->BeginParagraph();
        Writer->AddFormula(XDeriv);
        Writer->Add("After simplifying");
        Writer->AddFormula(XDerivSimplified);
        Writer->EndParagraph();
    };
    return true;
}

void TDerivativeProblem::Randomize(std::mt19937& rng) {
    TNumeric Res;
    TRandomFunction RF;
    switch (rng() % 3) {
        case 0:
            Res.operation = OperatorFrac;
            Res.OperandsPushback(RF.RandomFunction(RF.AllowTrigFunction | RF.AllowSum | RF.AllowLnFunction | RF.AllowExpFunction | RF.AllowInverseTrigFunction, 1, rng));
            Res.OperandsPushback(RF.RandomFunction(RF.AllowTrigFunction | RF.AllowSum | RF.AllowLnFunction | RF.AllowExpFunction | RF.AllowInverseTrigFunction, 1, rng));
            break;
        case 1:
            Res.operation = OperatorPow;
            Res.OperandsPushback(RF.RandomFunction(RF.AllowTrigFunction | RF.AllowSum | RF.AllowLnFunction | RF.AllowExpFunction | RF.AllowInverseTrigFunction, 2, rng));
            Res.OperandsPushback(RF.RandomFunction(RF.AllowTrigFunction | RF.AllowSum, 1, rng));
            break;
        case 2:
            Res.operation = OperatorProd;
            Res.OperandsPushback(RF.RandomFunction(RF.AllowTrigFunction | RF.AllowSum | RF.AllowLnFunction | RF.AllowExpFunction | RF.AllowInverseTrigFunction, 1, rng));
            Res.OperandsPushback(RF.RandomFunction(RF.AllowTrigFunction | RF.AllowSum | RF.AllowLnFunction | RF.AllowExpFunction | RF.AllowInverseTrigFunction, 1, rng));
            break;
        case 3:
            Res.operation = OperatorSum;
            Res.OperandsPushback(RF.RandomFunction(RF.AllowTrigFunction | RF.AllowSum | RF.AllowLnFunction | RF.AllowExpFunction | RF.AllowInverseTrigFunction, 2, rng));
            Res.OperandsPushback(RF.RandomFunction(RF.AllowTrigFunction | RF.AllowSum | RF.AllowLnFunction | RF.AllowExpFunction | RF.AllowInverseTrigFunction, 2, rng));
            break;
    }
    Res.SetEditableFlags(ConstAllowed | FunctionsAllowed);
    *Conditions = Res;
}
