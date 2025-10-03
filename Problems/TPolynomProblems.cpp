#include "TPolynomProblems.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
// #include "Base/tinterval.h"
#include "algebra_operations.h"

// todo: не показывает полное решение в линейных неравенствах

using namespace std;

TNumeric X_1 = MakeSubscript(TNumeric("x"), TNumeric("1"));
TNumeric X_2 = MakeSubscript(TNumeric("x"), TNumeric("2"));
TNumeric EmptySet = MakeInline(TNumeric("{"), TNumeric("\\empty"), TNumeric("}"));
TNumeric NumericAllReal = MakeInterval(TNumeric("-\\infty"), TNumeric("+\\infty"), false, false);
TInterval IntervalAllRealNumbers(TNumeric("-\\infty"), TNumeric("+\\infty"), false, false);
TNumeric NumPhi = TNumeric("\\phi");

const char TSetOfLinearInequalitiesStr[] = "TSetOfLinearInequalities";
const char TSystemOfLinearInequalitiesStr[] = "TSystemOfLinearInequalities";
const char TSetOfSquareInequalitiesStr[] = "TSetOfSquareInequalitiesStr";
const char TSystemOfSquareInequalitiesStr[] = "TSystemOfSquareInequalitiesStr";

TPolynomConditions::TPolynomConditions(size_t MaxPower, bool HaveRightPart, Operation operation) : TProblem() {
    UnknownVar = TNumeric("x");
    this->HaveRightPart = HaveRightPart;
    SetMaxPower(MaxPower, operation);
}

TPolynomConditions::TPolynomConditions(const TPolynom& P, bool HaveRightPart, Operation operation, bool AllCoef) {
    UnknownVar = TNumeric("x");
    Conditions = std::make_shared<TNumeric>();
    this->HaveRightPart = HaveRightPart;
    if (HaveRightPart) {
        Conditions->operation = operation;
        Conditions->OperandsPushback(TNumeric("0"));
        TNumeric RightPart("0");
        RightPart.role = RightPartID;
        // RightPart.SetEditableFlags(ConstAllowed);
        Conditions->OperandsPushback(TNumeric("0"));
    };

    SetLeftPartP(P, AllCoef);
}

TNumeric TPolynomConditions::GetVarPower(size_t power) {
    if (power == 0)
        return TNumeric("1");
    else {
        if (power == 1)
            return (UnknownVar);
        else
            return (UnknownVar ^ TNumeric(static_cast<int>(power)));
    };
}

void TPolynomConditions::SetMaxPower(size_t MaxPower, Operation comparison_operation) {
    this->MaxPower = MaxPower;
    TNumeric Res;
    for (size_t i = 0; i <= MaxPower; i++) {
        TNumeric a(0);
        a.role = RolePowerBegin + i;
        // a.SetEditableFlags(ConstAllowed);
        TNumeric Temp = GetVarPower(i);
        if (i == 0) {
            if (Temp.operation == OperatorConst && Temp.strval == "1")
                // случай специально для многочленов по x - вместо 0+a*x^0 делаем просто a
                Res = a;
            else
                Res = a * Temp;
        } else {
            Res = a * Temp + Res;
        }
    }
    if (HaveRightPart) {
        TNumeric RightPart("0");
        RightPart.role = RightPartID;
        // RightPart.SetEditableFlags(NoEditable);
        Conditions = std::make_shared<TNumeric>();
        Conditions->operation = comparison_operation;
        Conditions->OperandsPushback(Res);
        Conditions->OperandsPushback(RightPart);
    } else {
        Conditions = std::make_shared<TNumeric>(Res);
    }
}

TPolynomConditions::~TPolynomConditions() {
    // nothing to do
}

void TPolynomConditions::SetUnknownVar(TNumeric UnknownVar) {
    this->UnknownVar = UnknownVar;
    if (Conditions) {
        const TPolynom& P = asPolynom();
        SetLeftPartP(P, true);
    }
}

std::vector<std::shared_ptr<const TNumeric>> TPolynomConditions::GetCoef() const {
    assert(Conditions != nullptr);
    std::vector<std::shared_ptr<const TNumeric>> Coef2;
    Coef2.assign(MaxPower + 1, nullptr);
    for (size_t i = 0; i <= MaxPower; i++) {
        Coef2[i] = Conditions->GetByRoleConst(RolePowerBegin + i);
    }
    return Coef2;
}

std::vector<std::shared_ptr<TNumeric>> TPolynomConditions::GetCoef() {
    assert(Conditions != nullptr);
    std::vector<std::shared_ptr<TNumeric>> Coef2;
    Coef2.assign(MaxPower + 1, nullptr);
    for (size_t i = 0; i <= MaxPower; i++) {
        Coef2[i] = Conditions->GetByRole(RolePowerBegin + i, Conditions);
    }
    return Coef2;
}

TPolynom TPolynomConditions::asPolynom() const {
    /// \todo: try to make this function returning const Polynom
    auto coefs = GetCoef();
    std::vector<PNumeric> c(coefs.size(), nullptr);
    for (auto n = 0; n < coefs.size(); n++)
        c[n] = std::const_pointer_cast<TNumeric>(coefs[n]);
    return TPolynom(c);
}

void TPolynomConditions::SetLeftPartP(const TPolynom& P, bool AllCoef) {
    if (AllCoef)
        if (P.Coef.size() == 0)
            MaxPower = 0;
        else
            MaxPower = P.Coef.size() - 1;
    else
        MaxPower = P.MajorPower();

    TNumeric Sum;
    for (size_t i = 0; i <= MaxPower; i++) {
        auto a = P.GetCoef(i);
        a->role = RolePowerBegin + i;
        // a.SetEditableFlags(ConstAllowed);
        TNumeric Temp = GetVarPower(i);
        if (i == 0) {
            if (Temp.operation == OperatorConst && Temp.strval == "1")
                // случай специально для многочленов по x - вместо 0+a*x^0 делаем просто a
                Sum = *a;
            else
                Sum = *a * Temp;
        } else {
            Sum = *a * Temp + Sum;
        }
    }
    if (HaveRightPart) {
        // Обновляем Conditions, оставляя RightPart и operation не тронутым (если Conditions != NULL)
        TNumeric RightPart = TNumeric("0");
        Operation operation = OperatorEqual;
        if (Conditions == NULL) {
            Conditions = std::make_shared<TNumeric>();
        } else {
            RightPart = *Conditions->operands[1];
            operation = Conditions->operation;
        };
        RightPart.role = RightPartID;
        Conditions->operation = operation;
        Conditions->role = -1;
        Conditions->OperandsClear();
        Conditions->OperandsPushback(Sum);
        Conditions->OperandsPushback(RightPart);
    } else {
        Conditions = std::make_shared<TNumeric>(Sum);
    }
}

void TPolynomConditions::SetLeftPart(const TNumeric& a, const TNumeric& b) {
    vector<PNumeric> v;
    v.push_back(TNumeric::create(b));  // x^0
    v.push_back(TNumeric::create(a));  // x^1
    SetLeftPartP(TPolynom(v));
}

void TPolynomConditions::SetLeftPart(const TNumeric& a, const TNumeric& b, const TNumeric& c) {
    vector<PNumeric> v;
    v.push_back(TNumeric::create(c));  // x^0
    v.push_back(TNumeric::create(b));  // x^1
    v.push_back(TNumeric::create(a));  // x^2
    SetLeftPartP(TPolynom(v));
}

std::shared_ptr<TNumeric>& TPolynomConditions::GetCoefP(size_t power) {
    if (power <= MaxPower) {
        return Conditions->GetByRole(RolePowerBegin + power, Conditions);
    };
    throw "TPolynomConditions::GetCoef(size_t power): power > MaxPower";
}

std::shared_ptr<const TNumeric> TPolynomConditions::GetCoefP(size_t power) const {
    if (power <= MaxPower) {
        return Conditions->GetByRoleConst(RolePowerBegin + power);
    };
    throw "TPolynomConditions::GetCoef(size_t power): power > MaxPower";
}
std::shared_ptr<TNumeric>& TPolynomConditions::GetRightPartP() {
    return Conditions->GetByRole(RightPartID, Conditions);
}

TNumeric TPolynomConditions::GetRightPart() const {
    return *(Conditions->GetByRoleConst(RightPartID));
}

void TPolynomConditions::SetRightPart(const TNumeric& N) {
    std::shared_ptr<TNumeric>& R = Conditions->GetByRole(RightPartID, Conditions);
    if (R) {
        *R = N;
        R->role = RightPartID;
    }
}

void TPolynomConditions::SaveToFile(ofstream& f) {
    uint16_t MaxPower = this->MaxPower;
    f.write((char*)&MaxPower, sizeof(MaxPower));
    int16_t RightPartID = static_cast<int16_t>(this->RightPartID);
    f.write((char*)&RightPartID, sizeof(RightPartID));
    UnknownVar.SaveToFile(f);
    uint16_t HaveRightPart;
    if (this->HaveRightPart)
        HaveRightPart = 1;
    else
        HaveRightPart = 0;
    f.write((char*)&HaveRightPart, sizeof(HaveRightPart));
    TProblem::SaveToFile(f);
}

void TPolynomConditions::LoadFromFile(ifstream& f) {
    uint16_t MaxPower;
    int16_t RightPartID;
    f.read((char*)&MaxPower, sizeof(MaxPower));
    this->MaxPower = MaxPower;
    if (MaxPower > 100)
        throw "Incorrect file format";
    f.read((char*)&RightPartID, sizeof(RightPartID));
    assert(this->RightPartID == RightPartID);
    UnknownVar.LoadFromFile(f);
    uint16_t HaveRightPart;
    f.read((char*)&HaveRightPart, sizeof(HaveRightPart));
    if (HaveRightPart)
        this->HaveRightPart = true;
    else
        this->HaveRightPart = false;
    TProblem::LoadFromFile(f);
}

void TPolynomConditions::Randomize(std::mt19937& rng) {
    TPolynom P;
    P.Coef.assign(MaxPower + 1, nullptr);
    std::uniform_int_distribution<int> dist(-20, 20);
    for (size_t i = 0; i <= MaxPower; i++)
        P.Coef[i] = TNumeric::create(dist(rng));
    SetLeftPartP(P, true);
}

//******************************************************************************************
//******************************************************************************************
TPolynomialEquality::TPolynomialEquality(size_t MaxPower) : TPolynomConditions(MaxPower), TEquality() {
    if (MaxPower >= 3) {
        GetCoefP(0)->strval = "1";
        GetCoefP(3)->strval = "1";
    };
    if (MaxPower >= 6) {
        vector<PNumeric> R;
        /*R.push_back(TNumeric::create(TNumeric(-18) / TNumeric(25)));   // x^0
        R.push_back(TNumeric::create(TNumeric(111) / TNumeric(25)));   // x^1
        R.push_back(TNumeric::create(TNumeric(-253) / TNumeric(50)));  // x^2
        R.push_back(TNumeric::create(TNumeric(-143) / TNumeric(20)));  // x^3
        R.push_back(TNumeric::create(TNumeric(309) / TNumeric(100)));  // x^4
        R.push_back(TNumeric::create(TNumeric(22) / TNumeric(5)));     // x^5
        R.push_back(TNumeric::create(1));                    // x^6
        R.push_back(TNumeric::create(0));                    // x^7
        R.push_back(TNumeric::create(0));                    // x^8
        R.clear();*/
        R.assign(9, nullptr);
        for (size_t n = 0; n < R.size(); n++)
            R[n] = TNumeric::create(1);
        this->SetLeftPartP(R);
    }
    BuildPhrases();
}

TPolynomialEquality::TPolynomialEquality(const TPolynom& P, bool AllCoef) : TPolynomConditions(P, true, OperatorEqual, AllCoef), TEquality() {
    BuildPhrases();
}

TPolynomialEquality::~TPolynomialEquality() {}

void TPolynomialEquality::BuildPhrases() {
    if (MyTranslator.CheckDictionary(GetClassName()))
        return;
    MyTranslator.AddDictionary(GetClassName());
    MyTranslator.AddEng("All denominators should be integer.");
    MyTranslator.AddRus("Все знаменатели должны быть целыми");
    MyTranslator.AddEng("All numerators should be integer.");
    MyTranslator.AddRus("Все числители должны быть целыми");
    MyTranslator.AddEng("All coefficients should be rational.");
    MyTranslator.AddRus("Все коэффициенты должны быть рациональными");
    MyTranslator.AddEng("Reducing to common denominator: multiplying by %N");
    MyTranslator.AddRus("Приводим к общему знаменателю: умножаем на %N");
    MyTranslator.AddEng("Can not reduce to rational form.");
    MyTranslator.AddRus("Не могу привести к рациональной форме.");
    MyTranslator.AddEng("Factorizing out %n");
    MyTranslator.AddRus("Выносим %n");
    MyTranslator.AddEng("Searching for rational roots. These roots are like n/m, where n is divisor of free term, m - is divisor "
                        "of coefficient of major term.");
    MyTranslator.AddRus("Ищем рациональные корни. Эти корни должны быть вида n/m, где n - делитель свободного члена, m - "
                        "делитель старшего коэффициента.");
    MyTranslator.AddEng("Factorizing free term: ");
    MyTranslator.AddRus("Раскладываем на множители свободный член.");
    MyTranslator.AddEng("%d is simple number");
    MyTranslator.AddRus("%d - простое число");
    MyTranslator.AddEng("Factorizing major term");
    MyTranslator.AddRus("Раскладываем на множители коэффициент при главном члене");
    MyTranslator.AddEng("%d is simple number");
    MyTranslator.AddRus("%d - простое число");
    MyTranslator.AddEng("Checking roots candidates...");
    MyTranslator.AddRus("Ищем корни...");
    MyTranslator.AddEng("No rational roots found");
    MyTranslator.AddRus("Рациональных корней не найдено");
    MyTranslator.AddEng("Checking roots multiplicity");
    MyTranslator.AddRus("Проверяем кратность корней");
    MyTranslator.AddEng("Solving linear equality: %N");
    MyTranslator.AddRus("Решаем линейное уравнение: %N");
    MyTranslator.AddEng("Solving square equality: %N");
    MyTranslator.AddRus("Решаем квадратное уравнение: %N");
    MyTranslator.AddEng("Using the Kroneckers method");
    MyTranslator.AddRus("Используем метод Кронекера");
    MyTranslator.AddEng("Can not factorize polynom %n");
    MyTranslator.AddRus("Не могу разложить на множители многочлен %n");
    MyTranslator.AddEng("Factorization: %N");
    MyTranslator.AddRus("Разложение на множители %N");
    MyTranslator.AddEng("%n - root with multiplicity of %n");
    MyTranslator.AddRus("%n - корень кратности %n");
    MyTranslator.AddEng("Found root: %n");
    MyTranslator.AddRus("Найден корень: %n");
    MyTranslator.AddEng("Solving square equality: %N");
    MyTranslator.AddRus("Решаем квадратное уравнение: %N");
    MyTranslator.AddEng("Using the Kronecker method");
    MyTranslator.AddRus("Используем метод Кронекера");
    MyTranslator.AddEng("Can not find roots of remaining polynom:");
    MyTranslator.AddRus("Не могу найти корни оставшегося многочлена");
    MyTranslator.AddEng("Solving linear equality: %N");
    MyTranslator.AddRus("Решаем линейное уравнение: %N");
    MyTranslator.AddEng("Solving square equality: %N");
    MyTranslator.AddRus("Решаем квадратное уравнение: %N");
    MyTranslator.AddEng("After simplifying coefficients: %N");
    MyTranslator.AddRus("После упрощения коэффициентов: %N");
    MyTranslator.AddEng("Found roots:");
    MyTranslator.AddRus("Найденные корни:");
    MyTranslator.AddEng("%n has multiplicity %d");
    MyTranslator.AddRus("%n имеет кратность %d");
    MyTranslator.AddEng("No roots found.");
    MyTranslator.AddRus("Корней не найдено");
    MyTranslator.AddEng("Can not find all roots.");
    MyTranslator.AddRus("Не могу найти все корни.");
    MyTranslator.AddEng("Factorial expansion: %N");
    MyTranslator.AddRus("Разложение на множители: %N");
    MyTranslator.AddEng("Can not factorize polynom.");
    MyTranslator.AddRus("Не могу разложить многочлен на множители.");
    MyTranslator.AddEng("Find roots of polynom of %d power");
    MyTranslator.AddRus("Найти корни многочлена %d степени");
    MyTranslator.AddEng("polynom of %d power");
    MyTranslator.AddRus("многочлен %d степени");
}

vector<string> TPolynomialEquality::GetKeyWords() {
    if (MyTranslator.CheckDictionary(GetClassName() + "Keywords") == false) {
        MyTranslator.AddDictionary(GetClassName() + "Keywords");
        MyTranslator.AddEng("equality");
        MyTranslator.AddRus("уравнение");
        MyTranslator.AddEng("polynom");
        MyTranslator.AddRus("многочлен");
    }
    vector<string> Res;
    Res.push_back(MyTranslator.tr("equality"));
    Res.push_back(MyTranslator.tr("polynom"));
    return Res;
}

string TPolynomialEquality::GetTask() {
    char Buf[128];
    sprintf(Buf, MyTranslator.tr("Find roots of polynom of %d power").c_str(), MaxPower);
    return Buf;
}

string TPolynomialEquality::GetShortTask() {
    char Buf[128];
    sprintf(Buf, MyTranslator.tr("polynom of %d power").c_str(), MaxPower);
    return Buf;
}

void TPolynomialEquality::CopyAnswer(const TPolynomialEquality* Eq) {
    Degenerate = Eq->Degenerate;
    AllRootsFound = Eq->AllRootsFound;
    Roots = Eq->Roots;
    RootsMultiplicity = Eq->RootsMultiplicity;
    SquareMods = Eq->SquareMods;
    SquareModsMultiplicity = Eq->SquareModsMultiplicity;
    Multiplicators = Eq->Multiplicators;
    MMultiplicity = Eq->MMultiplicity;

    Result = Eq->Result;
    LinearMultiplier = Eq->LinearMultiplier;
};
void TPolynomialEquality::AddAnswer(const TPolynomialEquality* Eq) {
    Degenerate = Degenerate | Eq->Degenerate;
    AllRootsFound = AllRootsFound & Eq->AllRootsFound;
    for (size_t i = 0; i < Eq->Roots.size(); i++) {
        bool Found = false;
        for (size_t j = 0; j < Roots.size(); j++)
            if (Roots[j] == Eq->Roots[i]) {
                RootsMultiplicity[j] += Eq->RootsMultiplicity[i];
                Found = true;
            };
        if (!Found) {
            Roots.push_back(Eq->Roots[i]);
            RootsMultiplicity.push_back(Eq->RootsMultiplicity[i]);
        };
    };
    for (size_t i = 0; i < Eq->SquareMods.size(); i++) {
        bool Found = false;
        for (size_t j = 0; j < SquareMods.size(); j++)
            if (SquareMods[j] == Eq->SquareMods[i]) {
                SquareModsMultiplicity[j] += Eq->SquareModsMultiplicity[i];
                Found = true;
            };
        if (!Found) {
            SquareMods.push_back(Eq->SquareMods[i]);
            SquareModsMultiplicity.push_back(Eq->SquareModsMultiplicity[i]);
        };
    };
    for (size_t i = 0; i < Eq->Multiplicators.size(); i++) {
        bool Found = false;
        for (size_t j = 0; j < Multiplicators.size(); j++)
            if (Multiplicators[j] == Eq->Multiplicators[i]) {
                MMultiplicity[j] += Eq->MMultiplicity[i];
                Found = true;
            };
        if (!Found) {
            Multiplicators.push_back(Eq->Multiplicators[i]);
            MMultiplicity.push_back(Eq->MMultiplicity[i]);
        };
    }
    Result = Result + Eq->Result;
    LinearMultiplier = TNumeric::create(((*LinearMultiplier) * (*Eq->LinearMultiplier)).Simplify());
};
void TPolynomialEquality::AddRoot(const TNumeric& N, size_t Multiplicity) {
    TPolynomialEquality PE;
    PE.ClearSolution();
    PE.Degenerate = false;
    PE.Roots.push_back(TNumeric::create(N));
    PE.RootsMultiplicity.push_back(Multiplicity);

    TPolynom P;
    P.Coef.push_back(TNumeric::create(TNumeric("-1") * N));
    P.Coef.push_back(TNumeric::create("1"));
    PE.Multiplicators.push_back(P);
    PE.MMultiplicity.push_back(Multiplicity);

    PE.Result.Intervals.push_back(TInterval(N, N, true, true));  // adds [N, N] = {N}
    AddAnswer(&PE);
}

void TPolynomialEquality::AddMultiplicator(const TPolynom& P, size_t Multiplicity) {
    TPolynomialEquality PE;
    PE.ClearSolution();
    PE.Multiplicators.push_back(P);
    PE.MMultiplicity.push_back(Multiplicity);
    if (P.MajorPower() == 2) {
        SquareMods.push_back(P);
        SquareModsMultiplicity.push_back(Multiplicity);
    };
    if (P.MajorPower() == 0 && P.Coef.size() > 0)
        LinearMultiplier = TNumeric::create((*LinearMultiplier * (*P.Coef[0])).Simplify());

    AddAnswer(&PE);
}

bool TPolynomialEquality::CheckRationalAndGetNOK(std::shared_ptr<THTMLWriter> Writer, const TPolynom& P, int& NOK) {
    NOK = 1;
    for (size_t Power = 0; Power <= MaxPower; Power++) {
        if (P.GetCoef(Power)->operation != OperatorConst) {
            P.GetCoef(Power) = TNumeric::create(P.GetCoef(Power)->Simplify());  // пробуем сделать вычисления
        };

        if (P.GetCoef(Power)->operation == OperatorFrac) {
            TNumeric Denom = P.GetCoef(Power)->operands[1]->Simplify();
            P.GetCoef(Power)->operands[1] = std::make_shared<TNumeric>(Denom);

            int intD;
            if (!Denom.isInteger(&intD)) {
                if (Writer)
                    Writer->WriteError("All denominators should be integer.");
                return false;
            };

            NOK = GetNOK(NOK, intD);

            TNumeric Nom = P.GetCoef(Power)->operands[0]->Simplify();
            P.GetCoef(Power)->operands[0] = std::make_shared<TNumeric>(Nom);
            if (!Nom.isInteger(0)) {
                if (Writer)
                    Writer->WriteError("All numerators should be integer.");
                return false;
            };
        } else if (!P.GetCoef(Power)->isInteger(0)) {
            if (Writer)
                Writer->WriteError("All coefficients should be rational.");
            return false;
        };
    }
    return true;
}

bool TPolynomialEquality::GetIntCoefs(std::shared_ptr<THTMLWriter> Writer, int NOK, const TPolynom& P, vector<int>& IntCoefs) {
    // умножаем все коэф на общий знаменатель
    if (NOK > 1) {
        if (Writer)
            Writer->AddParagraph("Reducing to common denominator: multiplying by %N", TNumeric(NOK));
    };
    size_t MajorPower = P.MajorPower();
    IntCoefs.assign(MajorPower + 1, 0);
    for (size_t Power = 0; Power <= MajorPower; Power++) {
        if (!P.GetCoef(Power)->isInteger(&IntCoefs[Power])) {
            if (Writer)
                Writer->WriteError("Can not reduce to rational form.");
            return false;
        };
    }
    return true;
}

int TPolynomialEquality::factorOutCommonMultiplicator(std::shared_ptr<THTMLWriter> Writer, vector<int>& IntCoefs) {
    Q_UNUSED(Writer);
    int NOD = GetNOD(IntCoefs);
    for (size_t i = 0; i < IntCoefs.size(); i++)
        IntCoefs[i] = IntCoefs[i] / NOD;
    return NOD;
}

bool TPolynomialEquality::factorOutXk(std::shared_ptr<THTMLWriter> Writer, vector<int>& IntCoefs) {
    // Рассматриваем случай x^k*P(x) = 0
    size_t ZeroXMult = 0;  // кратность корня, равна k
    while (ZeroXMult <= IntCoefs.size() && IntCoefs[ZeroXMult] == 0)
        ZeroXMult++;

    if (ZeroXMult > 0) {
        if (Writer) {
            Writer->AddParagraph("Factorizing out %n", MakePow(UnknownVar, TNumeric(static_cast<int>(ZeroXMult))));

            Writer->AddParagraph("%n - root with multiplicity of %n", MakeEquality(UnknownVar, TNumeric("0")), TNumeric(static_cast<int>(ZeroXMult)));

            AddRoot(TNumeric("0"), ZeroXMult);
            Writer->EndParagraph();
        }

        IntCoefs.erase(IntCoefs.begin(), IntCoefs.begin() + ZeroXMult);
    };
    return true;
};

bool TPolynomialEquality::SearchRationalRoots(std::shared_ptr<THTMLWriter> Writer, vector<int>& IntCoefs, TPolynom& PRemaining) {
    if (IntCoefs.size() == 0)
        return true;
    vector<TNumeric> RationalRoots;  // найденные корни без учёта кратности
    Degenerate = false;
    int FreeMember = IntCoefs[0];
    int MajorMember = IntCoefs[IntCoefs.size() - 1];
    vector<int> FreeMemberMults = IntFactorize(FreeMember);
    vector<int> MajorMemberMults = IntFactorize(MajorMember);

    if (Writer)
        // Writer->AddParagraph("Ищем рациональные корни. Эти корни должны быть вида n/m, где n - делитель свободного члена, m -
        // делитель старшего коэффициента.");
        Writer->AddParagraph("Searching for rational roots. These roots are like n/m, where n is divisor of free term, m - is "
                             "divisor of coefficient of major term.");

    if (Writer)
    // рисуем найденные множители
    {
        Writer->AddParagraph("Factorizing free term: ");
        Writer->IncrementNestingLevel();
        TNumeric Temp;
        Temp.operation = OperatorProd;
        if (FreeMemberMults.size() == 1) {
            Writer->AddParagraph("%d is simple number", FreeMember);
        } else {
            for (size_t i = 0; i < FreeMemberMults.size(); i++)
                Temp.OperandsPushback(TNumeric(FreeMemberMults[i]));
            Writer->AddFormula(MakeEquality(TNumeric(abs(FreeMember)), Temp));
        }
        Writer->DecrementNestingLevel();
        Writer->AddParagraph("Factorizing major term");
        Writer->IncrementNestingLevel();
        Temp.OperandsClear();
        if (MajorMemberMults.size() == 1) {
            Writer->AddParagraph("%d is simple number", FreeMember);
        } else {
            for (size_t i = 0; i < MajorMemberMults.size(); i++)
                Temp.OperandsPushback(TNumeric(MajorMemberMults[i]));
            Writer->AddFormula(MakeEquality(TNumeric(abs(FreeMember)), Temp));
        };
        Writer->DecrementNestingLevel();
    }

    if (Writer) {
        Writer->BeginParagraph();
        Writer->Add("Checking roots candidates...");
    };
    if (find(FreeMemberMults.begin(), FreeMemberMults.end(), 1) == FreeMemberMults.end())
        FreeMemberMults.push_back(1);  // если небыло единицы среди множителей, добавляем её
    if (find(MajorMemberMults.begin(), MajorMemberMults.end(), 1) == MajorMemberMults.end())
        MajorMemberMults.push_back(1);  // если небыло единицы среди множителей, добавляем её

    TPolynom SourceP(IntCoefs);
    vector<int> FreeProds = GetAllProds(FreeMemberMults);
    vector<int> MajorProds = GetAllProds(MajorMemberMults);
    for (size_t i = 0; i < FreeProds.size(); i++)       // перебираем делители свободного члена
        for (size_t j = 0; j < MajorProds.size(); j++)  // перебираем делители коэф при старшем члене
            for (int sign = -1; sign <= 1; sign += 2)   // перебираем знак
            {
                TNumeric Test;  // тестовый корень
                // Инициализируем тестовый корень
                int N = FreeProds[i];
                int D = MajorProds[j];
                if (D == 1) {
                    Test = TNumeric(sign * N);
                } else {
                    Test.operation = OperatorFrac;
                    Test.OperandsClear();
                    Test.OperandsPushback(TNumeric(sign * N));
                    Test.OperandsPushback(TNumeric(D));
                };
                Test = Test.Simplify();

                bool AllreadyFound = false;
                for (size_t r = 0; r < RationalRoots.size(); r++) {
                    if (RationalRoots[r] == Test)  // Для левой и правой части метод Simplify уже вызывался
                    {
                        AllreadyFound = true;
                        break;
                    }
                }
                if (AllreadyFound == false)
                    // проверяем тестовый корень
                    if (SourceP.Calculate(Test) == 0) {
                        if (Writer)
                            Writer->AddFormula(MakeEquality(UnknownVar, Test));
                        RationalRoots.push_back(Test);
                    };
            }
    if (Writer)
        Writer->EndParagraph();

    PRemaining = TPolynom(IntCoefs);
    if (RationalRoots.size() == 0) {
        // рациональные корни найти не удалось
        if (Writer)
            Writer->AddParagraph("No rational roots found");
    } else {
        // Теперь проверяем кратность корней
        if (Writer) {
            Writer->BeginParagraph();
            Writer->Add("Checking roots multiplicity");
        };
        TNumeric Res;  // разложение на множители
        Res.operation = OperatorProd;
        for (size_t i = 0; i < RationalRoots.size(); i++) {
            vector<TNumeric> divisor_n;
            // устанавливаем делитель в x - RationalRoots[i]
            divisor_n.assign(2, TNumeric(0));
            divisor_n[0] = (RationalRoots[i] * TNumeric(-1)).Simplify();
            divisor_n[1] = TNumeric(1);
            TPolynom divisor(std::move(divisor_n));

            TPolynom Reminder;
            TPolynom PRemainingPrev = PRemaining;
            int Multiplicity = 0;  // кратность корня
            do {
                TPolynom Ratio = PRemaining.Div(divisor, &Reminder);
                if (Reminder.MajorPower() == 0 && Reminder.GetCoef(0) == 0)
                // делится без остатка
                {
                    Multiplicity++;
                    PRemaining = Ratio;
                } else
                    break;
            } while (true);

            if (Multiplicity > 0) {
                TNumeric Multiplier;
                Multiplier.operation = OperatorSum;
                Multiplier.OperandsPushback(UnknownVar);
                Multiplier.OperandsPushback((TNumeric(-1) * TNumeric(RationalRoots[i])).Simplify());
                if (Multiplicity > 1) {
                    TNumeric Temp = Multiplier;
                    Multiplier.operands[0] = TNumeric::create(std::move(Temp));
                    Multiplier.operands[1] = TNumeric::create(Multiplicity);
                    Multiplier.operation = OperatorPow;
                };
                if (Writer)
                    Writer->AddFormula(MakeEquality(PRemainingPrev.asNumeric(UnknownVar), Multiplier * PRemaining.asNumeric(UnknownVar)));

                // Добавляем найденный корень к результатам
                AddRoot(RationalRoots[i], Multiplicity);
            };
        };
        if (Writer)
            Writer->EndParagraph();
    };
    return true;
};

bool TPolynomialEquality::AnalyzePRemaining(std::shared_ptr<THTMLWriter> Writer, const TPolynom& PRemaining) {
    AllRootsFound = true;

    // Рассматриваем оставшийся многочлен, который не смогли разложить
    switch (PRemaining.MajorPower()) {
        case 0:
            break;
        case 1:  // остался многочлен первой степени
        {
            TLinearEquality Eq(PRemaining);
            if (Writer)
                Writer->AddParagraph("Solving linear equality: %N", *Eq.Conditions);
            if (Writer)
                Writer->IncrementNestingLevel();
            Eq.GetSolution(Writer);
            if (Writer)
                Writer->DecrementNestingLevel();
            AddAnswer(&Eq);
        } break;

        case 2:  // остался многочлен второй степени
        {
            TSquareEquality Eq(PRemaining);
            if (Writer)
                Writer->AddParagraph("Solving square equality: %N", *Eq.Conditions);
            if (Writer)
                Writer->IncrementNestingLevel();
            Eq.GetSolution(Writer);
            if (Writer)
                Writer->DecrementNestingLevel();
            AddAnswer(&Eq);
        } break;

        default: {
            // остался многочлен третьей или более высокой степени
            // используем схему Кронекера для факторизации этого многочлена
            vector<size_t> MMults;
            vector<TPolynom> Mults = PRemaining.FactorizeKroneker(&MMults);
            // проверяем, что все множители - не более, чем квадратичные
            if (Writer) {
                Writer->AddParagraph("Using the Kroneckers method");
                Writer->IncrementNestingLevel();
                if (Mults.size() == 1) {
                    Writer->AddParagraph("Can not factorize polynom %n", PRemaining.asNumeric(UnknownVar));
                } else {
                    TNumeric Factorization;
                    for (size_t i = 0; i < Mults.size(); i++) {
                        TNumeric Term = Mults[i].asNumeric(UnknownVar);
                        if (MMults[i] > 1)
                            Term = Term ^ TNumeric(static_cast<int>(MMults[i]));  /// \todo: why MMults is size_t?
                        if (i == 0)
                            Factorization = Term;
                        else
                            Factorization = Factorization * Term;
                    };
                    Writer->AddParagraph("Factorization: %N", Factorization);
                };
            };

            for (size_t i = 0; i < Mults.size(); i++) {
                TPolynomialEquality Eq;
                switch (Mults[i].MajorPower()) {
                    case 0:
                        break;
                    case 1: {
                        TNumeric X = (TNumeric(-1) * (*Mults[i].Coef[0])).Simplify();
                        AddRoot(X, MMults[i]);
                        if (Writer)
                            Writer->AddParagraph("Found root: %n", X);
                        break;
                    }
                    case 2: {
                        TSquareEquality E;
                        E.UnknownVar = UnknownVar;
                        E.SetLeftPartP(Mults[i]);
                        if (Writer)
                            Writer->AddParagraph("Solving square equality: %N", *E.Conditions);
                        if (Writer)
                            Writer->IncrementNestingLevel();
                        if (E.GetSolution(Writer) == false) {
                            if (Writer)
                                Writer->DecrementNestingLevel();  // Closing <div> opened at "Using the Kronecker method"
                            if (Writer)
                                Writer->DecrementNestingLevel();
                            return false;
                        };
                        if (Writer)
                            Writer->DecrementNestingLevel();
                        for (size_t i_1 = 0; i_1 < MMults[i]; i_1++)
                            AddAnswer(&E);
                    }; break;
                    default:
                        if (Writer) {
                            Writer->BeginError();
                            Writer->Add("Can not find roots of remaining polynom:");
                            Writer->AddFormula(Mults[i].asNumeric(UnknownVar));
                            Writer->EndError();
                        };
                        AddMultiplicator(Mults[i], MMults[i]);
                        AllRootsFound = false;
                };
            };
            if (Writer)
                Writer->DecrementNestingLevel();
            break;
        };
    }
    return AllRootsFound;
}

void TPolynomialEquality::ClearSolution() {
    Result.Intervals.clear();
    Roots.clear();
    RootsMultiplicity.clear();
    Multiplicators.clear();
    MMultiplicity.clear();
    SquareMods.clear();
    SquareModsMultiplicity.clear();
    Degenerate = false;
    AllRootsFound = true;
    LinearMultiplier = TNumeric::create(1);
}

bool TPolynomialEquality::GetSolution(std::shared_ptr<THTMLWriter> Writer) {  /// \todo: should be const method
    vector<std::shared_ptr<TNumeric>> Coef1;                                  // коэффициенты в виде объектов TNumeric
    Coef1 = GetCoef();

    ClearSolution();

    TPolynom SourcePolynom;  // полином, прочитанный из условия задачи
    SourcePolynom = TPolynom(Coef1);
    size_t MajorPower = SourcePolynom.MajorPower();  // старшая степень с отличным от нуля коэффициентом

    if (MajorPower <= 1)
    // a * x^0 = 0
    {
        TLinearEquality E;
        E.UnknownVar = UnknownVar;
        E.SetLeftPartP(SourcePolynom);
        if (Writer)
            Writer->AddParagraph("Solving linear equality: %N", *E.Conditions);
        bool res = E.GetSolution(Writer);
        CopyAnswer(&E);
        return res;
    } else if (MajorPower == 2) {
        TSquareEquality E;
        E.UnknownVar = UnknownVar;
        E.SetLeftPartP(SourcePolynom);
        if (Writer)
            Writer->AddParagraph("Solving square equality: %N", *E.Conditions);
        if (Writer)
            Writer->IncrementNestingLevel();
        bool res = E.GetSolution(Writer);
        if (Writer)
            Writer->DecrementNestingLevel();
        CopyAnswer(&E);
        return res;
    } else {
        // проверяем, чтобы все коэффициенты были рациональными
        // заодно вычисляем НОК знаменателей всех коэффициентов
        int NOK;
        if (CheckRationalAndGetNOK(Writer, SourcePolynom, NOK) == false)
            return false;

        if (NOK != 1)
            AddMultiplicator(TPolynom(TNumeric(1) / NOK));  // регистрируем множитель

        // заполняем целочисленные коэффициенты
        vector<int> IntCoef;
        if (GetIntCoefs(Writer, NOK, SourcePolynom, IntCoef) == false)
            return false;

        // Теперь выносим общий множитель всех коэффициентов
        int NOD = factorOutCommonMultiplicator(Writer, IntCoef);
        if (NOD != 1)
            AddMultiplicator(TPolynom(TNumeric(NOD)));  // регистрируем множитель

        if (Writer)
            Writer->AddParagraph("After simplifying coefficients: %N", MakeEquality(TPolynom(IntCoef).asNumeric(UnknownVar), TNumeric(0)));

        // Рассматриваем случай x^k*P(x) = 0
        if (factorOutXk(Writer, IntCoef) == false)
            return false;

        // ТЕПЕРЬ ИЩЕМ СРЕДИ РАЦИОНАЛЬНЫХ КОРНЕЙ
        TPolynom PRemaining;
        if (SearchRationalRoots(Writer, IntCoef, PRemaining) == false)
            return false;

        if (AnalyzePRemaining(Writer, PRemaining) == false)
            return false;
    };
    SortRoots();
    PrintAnswer(Writer);
    return true;
}

void TPolynomialEquality::PrintAnswer(std::shared_ptr<THTMLWriter> Writer) {
    if (Roots.size() > 0) {
        if (Writer) {
            Writer->AddParagraph("Found roots:");
            Writer->IncrementNestingLevel();
            for (size_t i = 0; i < Roots.size(); i++)
                Writer->AddParagraph2("%n has multiplicity %d", MakeEquality(UnknownVar, *Roots[i]), RootsMultiplicity[i]);
            Writer->DecrementNestingLevel();
        }
    } else {
        if (AllRootsFound) {
            if (Writer)
                Writer->AddParagraph("No roots found.");
        } else {
            if (Writer)
                Writer->WriteError("Can not find all roots.");
        }
    }
    if (Multiplicators.size() > 0) {
        TNumeric Res;
        Res.operation = OperatorProd;
        for (size_t i = 0; i < Multiplicators.size(); i++) {
            TNumeric A = Multiplicators[i].asNumeric(UnknownVar);
            if (MMultiplicity[i] > 1)
                A = A ^ MMultiplicity[i];
            Res.OperandsPushback(A);
        }
        if (Writer) {
            Writer->AddParagraph("Factorial expansion: %N", Res);
        }
    } else {
        if (Writer)
            Writer->WriteError("Can not factorize polynom.");
    }
}

void TPolynomialEquality::SortRoots() {
    for (size_t i = 1; i < Roots.size(); i++)
        for (size_t j = 0; j < i; j++) {
            if (Roots[i]->Calculate() < Roots[j]->Calculate()) {
                std::swap(Roots[i], Roots[j]);

                size_t Mult = RootsMultiplicity[i];
                RootsMultiplicity[i] = RootsMultiplicity[j];
                RootsMultiplicity[j] = Mult;
            }
        }
}

//********************************************************************************************************
//********************************************************************************************************

TLinearEquality::TLinearEquality() : TPolynomialEquality(1) {
    CanRandomize = true;
    BuildPhrases();
}

TLinearEquality::TLinearEquality(const TPolynom& P) : TPolynomialEquality(P, false) {
    if (P.MajorPower() > 1)
        throw "TLinearEquality::TLinearEquality(TPolynom P): P.MajorPower() > 1";
}
void TLinearEquality::BuildPhrases() {
    if (MyTranslator.CheckDictionary(GetClassName()))
        return;
    MyTranslator.AddDictionary(GetClassName());
    MyTranslator.AddEng("Error: can't find coefficients");
    MyTranslator.AddRus("Ошибка: не могу прочитать коэффициенты");
    MyTranslator.AddEng("Major coefficient (linear) is zero. Graphic is parallel to X axis");
    MyTranslator.AddRus("Коэффициент при линейном члене равен нулю. График прямой параллелен оси X.");
    MyTranslator.AddEng("Solution is any number.");
    MyTranslator.AddRus("Решение - вся числовая ось");
    MyTranslator.AddEng("Free member is non-zerro. No solution.");
    MyTranslator.AddRus("Свободный член отличен от нуля. Нет решений");
    MyTranslator.AddEng("Only solution exists: ");
    MyTranslator.AddRus("Существует единственное решение");
    MyTranslator.AddEng("Solve linear equality");
    MyTranslator.AddRus("Решить линейное уравнение");
    MyTranslator.AddEng("linear equality");
    MyTranslator.AddRus("линейное уравнение");
}

vector<string> TLinearEquality::GetKeyWords() {
    if (MyTranslator.CheckDictionary(GetClassName() + "Keywords") == false) {
        MyTranslator.AddDictionary(GetClassName() + "Keywords");
        MyTranslator.AddEng("equality");
        MyTranslator.AddRus("уравнение");
        MyTranslator.AddEng("linear");
        MyTranslator.AddRus("линейное");
    }
    vector<string> Res;
    Res.push_back(MyTranslator.tr("equality"));
    Res.push_back(MyTranslator.tr("linear"));
    return Res;
}

TLinearEquality::~TLinearEquality() {
    // delete Conditions and delete Solution is called by ~TProblem, no need to call them here
}

TLinearEquality::TLinearEquality(const TLinearEquality& L) : TPolynomialEquality() {
    Assign(L);
}

string TLinearEquality::GetTask() {
    return MyTranslator.tr("Solve linear equality");
}

string TLinearEquality::GetShortTask() {
    return MyTranslator.tr("linear equality");
}

void TLinearEquality::Assign(const TLinearEquality& L) {
    Conditions = std::make_shared<TNumeric>(*L.Conditions);
}

void TLinearEquality::SetLeftPartP(const TPolynom& P, bool) {
    TPolynom P2 = P;
    if (P2.MajorPower() > 1)
        throw "TLinearEquality::SetLeftPart(const TPolynom &P): MajorPower > 1";
    while (P2.Coef.size() < 2)
        P2.Coef.push_back(TNumeric::create(0));  // требуем, чтобы все необходимые коэффициенты присутствовали
    TPolynomialEquality::SetLeftPartP(P2);
}

bool TLinearEquality::GetSolution(std::shared_ptr<THTMLWriter> Writer) {
    ClearSolution();
    TNumeric& a = *GetCoefP(1);
    TNumeric& b = *GetCoefP(0);
    TNumeric& c = *GetRightPartP();

    if (!(a.CanCalculate() && b.CanCalculate() && c.CanCalculate()))
        return false;
    if (a.Calculate() == 0) {
        if (Writer)
            Writer->AddParagraph("Major coefficient (linear) is zero. Graphic is parallel to X axis");
        Degenerate = true;
        Roots.clear();
        RootsMultiplicity.clear();
        if ((c - b).Calculate() == 0) {
            if (Writer)
                Writer->AddParagraph("Solution is any number.");
            if (Writer)
                Writer->AddFormula(MakeBelongsTo(UnknownVar, NumericAllReal));
            Result.Intervals.push_back(IntervalAllRealNumbers);
        } else {
            if (Writer)
                Writer->AddParagraph("Free member is non-zerro. No solution.");
            if (Writer)
                Writer->AddFormula(MakeBelongsTo(UnknownVar, EmptySet));
            Result.Intervals.clear();
        }
    } else {
        TNumeric X = (c - b) / a;
        TNumeric XSimplified = X.Simplify();
        if (Writer)
            Writer->AddParagraph("Only solution exists: ");
        if (Writer)
            Writer->AddFormula(MakeEquality(UnknownVar, MakeEquality(X, XSimplified)));
        AddRoot(XSimplified);
    };
    SortRoots();
    AllRootsFound = true;
    return true;
};

void TLinearEquality::Randomize(std::mt19937& rng) {
    std::uniform_int_distribution<int> dist(-20, 20);
    TNumeric A(dist(rng));
    TNumeric B(dist(rng));
    TNumeric C(dist(rng));
    TPolynom P(A, B);
    SetLeftPartP(P);
    SetRightPart(C);
}

//********************************************************************************************************
//********************************************************************************************************

TSquareEquality::TSquareEquality() : TPolynomialEquality(2) {
    CanRandomize = true;
    BuildPhrases();
}

TSquareEquality::TSquareEquality(TPolynom P) : TPolynomialEquality(P, false) {
    if (P.MajorPower() > 2)
        throw "TSquareEquality::TSquareEquality(TPolynom P): P.MajorPower() > 2";
    BuildPhrases();
}

TSquareEquality::~TSquareEquality() {
    // delete Conditions and delete Solution is called by ~TProblem, no need to call them here
}

void TSquareEquality::BuildPhrases() {
    if (MyTranslator.CheckDictionary(GetClassName()))
        return;
    MyTranslator.AddDictionary(GetClassName());
    MyTranslator.AddEng("Error: can't find coefficients");
    MyTranslator.AddRus("Ошибка: не могу прочитать коэффициенты");
    MyTranslator.AddEng("Coefficient at square term is zero, so solving the linear equality.");
    MyTranslator.AddRus("Коэффициент при квадрате равен нулю, следовательно уравнение линейное");
    MyTranslator.AddEng("Equality has one root of twice multiplicity.");
    MyTranslator.AddRus("Уравнение имеет один корень двойной кратности");
    MyTranslator.AddEng("Equality has two different roots.");
    MyTranslator.AddRus("Уравнение имеет два различных корня.");
    MyTranslator.AddEng("Finding discriminant: %N");
    MyTranslator.AddRus("Ищем дискриминант: %N");
    MyTranslator.AddEng("Discriminant is zero. So equality has one root of twice multiplicity.");
    MyTranslator.AddRus("Дискриминант равен нулю. Уравнение имеет один корень двойной кратности.");
    MyTranslator.AddEng("Discriminant is greater than zero. So equality has two different roots");
    MyTranslator.AddRus("Дискриминант больше нуля. Уравнение имеет два различных корня");
    MyTranslator.AddEng("Discriminant is less than zero. Equation has no roots.");
    MyTranslator.AddRus("Дискриминант меньше нуля. Уравнение не имеет корней.");
    MyTranslator.AddEng("Solve square equality");
    MyTranslator.AddRus("Решить квадратное уравение");
    MyTranslator.AddEng("square equality");
    MyTranslator.AddRus("квадратное уравнение");
}

vector<string> TSquareEquality::GetKeyWords() {
    if (MyTranslator.CheckDictionary(GetClassName() + "Keywords") == false) {
        MyTranslator.AddDictionary(GetClassName() + "Keywords");
        MyTranslator.AddEng("equality");
        MyTranslator.AddRus("уравнение");
        MyTranslator.AddEng("square");
        MyTranslator.AddRus("квадратное");
    }
    vector<string> Res;
    Res.push_back(MyTranslator.tr("equality"));
    Res.push_back(MyTranslator.tr("square"));
    return Res;
}

string TSquareEquality::GetTask() {
    return MyTranslator.tr("Solve square equality");
}

string TSquareEquality::GetShortTask() {
    return MyTranslator.tr("square equality");
}

void TSquareEquality::SetLeftPartP(const TPolynom& P, bool) {
    TPolynom P2 = P;
    if (P2.MajorPower() > 2)
        throw "TSquareEquality::SetLeftPartP(const TPolynom &P): MajorPower > 2";
    while (P2.Coef.size() < 3)
        P2.Coef.push_back(TNumeric::create(0));  // требуем, чтобы все необходимые коэффициенты присутствовали
    TPolynomialEquality::SetLeftPartP(P2);
}

bool TSquareEquality::GetSolution(std::shared_ptr<THTMLWriter> Writer) {
    ClearSolution();
    TNumeric& a = *GetCoefP(2);
    TNumeric& b = *GetCoefP(1);
    TNumeric& c = *GetCoefP(0);
    if (!(a.CanCalculate() && b.CanCalculate() && c.CanCalculate()))
        return false;

    Result.Intervals.clear();
    Roots.clear();
    RootsMultiplicity.clear();

    TNumeric X;
    if (a.Calculate() == 0) {
        // решаем линейное уравнение
        if (Writer)
            Writer->AddParagraph("Coefficient at square term is zero, so solving the linear equality.");

        TLinearEquality E;
        E.SetLeftPart(b, c);
        if (Writer)
            Writer->IncrementNestingLevel();
        E.GetSolution(Writer);
        if (Writer)
            Writer->DecrementNestingLevel();
        // копируем ответ
        CopyAnswer(&E);
    } else {
        if (c.Calculate() == 0) {
            // случай ax^2+bx = 0, возможно b = 0
            TNumeric X1("0");
            if (b.Calculate() == 0)
            // случай ax^2 = 0
            {
                if (Writer)
                    Writer->AddParagraph("Equality has one root of twice multiplicity.");
                if (Writer)
                    Writer->AddFormula(MakeEquality(UnknownVar, X1));
                Result.Intervals.push_back(TInterval(X1, X1, true, true));
                AddRoot(X1, 2);
            } else {
                // случай ax^2+bx = 0
                TNumeric X2(-b / a);
                TNumeric X2Simplified = X2.Simplify();
                if (Writer)
                    Writer->AddParagraph("Equality has two different roots.");
                if (Writer)
                    Writer->AddFormula(MakeEquality(X_1, X1));
                if (Writer)
                    Writer->AddFormula(MakeEquality(X_2, MakeEquality(X2, X2Simplified)));
                Result.Intervals.push_back(TInterval(X1, X1, true, true));
                Result.Intervals.push_back(TInterval(X2Simplified, X2Simplified, true, true));
                AddRoot(X1);
                AddRoot(X2Simplified);
            }

        } else {
            TNumeric D = (b ^ TNumeric(2)) - TNumeric(4) * a * c;
            TNumeric DSimplified = D.Simplify();
            if (Writer)
                Writer->AddParagraph("Finding discriminant: %N", MakeEquality(TNumeric("D"), MakeEquality(D, DSimplified)));

            double d = DSimplified.Calculate();

            if (d == 0) {
                if (Writer)
                    Writer->AddParagraph("Discriminant is zero. So equality has one root of twice multiplicity.");
                TNumeric X1 = (-b + DSimplified.sqrt()) / (TNumeric(2) * a);
                TNumeric X1Simplified = X1.Simplify();
                if (Writer)
                    Writer->AddFormula(MakeEquality(UnknownVar, MakeEquality(X1, X1Simplified)));
                AddRoot(X1Simplified, 2);
            } else {
                if (d > 0) {
                    if (Writer)
                        Writer->Add("Discriminant is greater than zero. So equality has two different roots");

                    TNumeric X1 = (-b + DSimplified.sqrt()) / (TNumeric(2) * a);
                    TNumeric X1Simplified = X1.Simplify();
                    if (Writer)
                        Writer->AddFormula(MakeEquality(X_1, MakeEquality(X1, X1Simplified)));
                    Result.Intervals.push_back(TInterval(X1Simplified, X1Simplified, true, true));

                    TNumeric X2 = (-b - DSimplified.sqrt()) / (TNumeric(2) * a);
                    TNumeric X2Simplified = X2.Simplify();
                    if (Writer)
                        Writer->AddFormula(MakeEquality(X_2, MakeEquality(X2, X2Simplified)));
                    Result.Intervals.push_back(TInterval(X2Simplified, X2Simplified, true, true));

                    AddRoot(X1Simplified);
                    AddRoot(X2Simplified);
                } else {
                    if (Writer)
                        Writer->AddParagraph("Discriminant is less than zero. Equation has no roots.");
                    TPolynom P;
                    P.Coef.push_back(TNumeric::create(std::move(c)));
                    P.Coef.push_back(TNumeric::create(std::move(b)));
                    P.Coef.push_back(TNumeric::create(std::move(a)));
                    AddMultiplicator(P);
                    Degenerate = false;  // вырожденным не является, просто нет корней
                }
            };
        };
    };
    SortRoots();
    AllRootsFound = true;
    return true;
}

void TSquareEquality::Randomize(std::mt19937& rng) {
    std::uniform_int_distribution<int> dist(-20, 20);
    TNumeric A(dist(rng));
    TNumeric B(dist(rng));
    TNumeric C(dist(rng));
    TPolynom P(A, B, C);
    SetLeftPartP(P);
}

//*****************************************************************************************************************************
//              НЕРАВЕНСТВА
//*****************************************************************************************************************************
TPolynomialInequality::TPolynomialInequality(size_t MaxPower, bool Less, bool Strict) : TPolynomConditions(MaxPower, true, OperatorLess) {
    SetType(Less, Strict);
    BuildPhrases();
}

TPolynomialInequality::TPolynomialInequality(const TPolynom& P, bool Less, bool Strict, bool AllCoef) : TPolynomConditions(P, true, OperatorLess, AllCoef) {
    SetType(Less, Strict);
    BuildPhrases();
}

void TPolynomialInequality::BuildPhrases() {
    if (MyTranslator.CheckDictionary(GetClassName()))
        return;
    MyTranslator.AddDictionary(GetClassName());
    MyTranslator.AddEng("Finding roots of polynom.");
    MyTranslator.AddRus("Ищем корни многочлена");
    MyTranslator.AddEng("Left side of inequality is degenerated.");
    MyTranslator.AddRus("Левая часть неравенства вырождена");
    MyTranslator.AddEng("Because inequality is strict it has no solution in this case.");
    MyTranslator.AddRus("Так как неравенство строгое, то решений нет");
    MyTranslator.AddEng("Because inequality is unstrict the solution is any number");
    MyTranslator.AddRus("Так как неравенство нестрогое, то решение - вся числовая ось");
    MyTranslator.AddEng("Can not find roots.");
    MyTranslator.AddRus("Не могу найти корни");
    MyTranslator.AddEng("Solve polynomial inequality of %d power");
    MyTranslator.AddRus("Решить полиномиальное неравенство %d степени");
    MyTranslator.AddEng("%d power");
    MyTranslator.AddRus("%d степени");
}

vector<string> TPolynomialInequality::GetKeyWords() {
    if (MyTranslator.CheckDictionary(GetClassName() + "Keywords") == false) {
        MyTranslator.AddDictionary(GetClassName() + "Keywords");
        MyTranslator.AddEng("inequality");
        MyTranslator.AddRus("неравенство");
        MyTranslator.AddEng("polynom");
        MyTranslator.AddRus("многочлен");
    }
    vector<string> Res;
    Res.push_back(MyTranslator.tr("inequality"));
    Res.push_back(MyTranslator.tr("polynom"));
    return Res;
}

TPolynomialInequality::~TPolynomialInequality() {}

bool TPolynomialInequality::SetType(bool Less, bool Strict) {
    Operation operation;
    if (Less && Strict)
        operation = OperatorLess;
    if (Less && !Strict)
        operation = OperatorLessOrEqual;
    if (!Less && Strict)
        operation = OperatorGreater;
    if (!Less && !Strict)
        operation = OperatorGreaterOrEqual;
    Conditions->operation = operation;
    this->Less = Less;
    this->Strict = Strict;
    return true;
}

string TPolynomialInequality::GetTask() {
    char Buf[128];
    sprintf(Buf, MyTranslator.tr("Solve polynomial inequality of %d power").c_str(), this->MaxPower);
    return string(Buf);
}

string TPolynomialInequality::GetShortTask() {
    char Buf[128];
    sprintf(Buf, MyTranslator.tr("%d power").c_str(), this->MaxPower);
    return string(Buf);
}

bool TPolynomialInequality::GetSolution(std::shared_ptr<THTMLWriter> Writer) {
    vector<PNumeric> Coef1;  // коэффициенты в виде объектов TNumeric
    Coef1 = GetCoef();

    Result.Intervals.clear();
    TPolynom P(std::move(Coef1));

    TPolynomialEquality E(P);
    if (Writer)
        Writer->AddParagraph("Finding roots of polynom.");
    if (Writer)
        Writer->IncrementNestingLevel();
    bool res = E.GetSolution(Writer);
    if (Writer)
        Writer->DecrementNestingLevel();
    if (res) {
        if (E.Degenerate) {
            if (Writer)
                Writer->AddParagraph("Left side of inequality is degenerated.");
            if (Strict) {
                if (Writer)
                    Writer->AddParagraph("Because inequality is strict it has no solution in this case.");
                Result.Intervals.clear();
            } else {
                if (Writer)
                    Writer->AddParagraph("Because inequality is unstrict the solution is any number");
                Result = E.Result;
            }
        } else {
            size_t MajorPower = P.MajorPower();

            int CurSign;  // текущий знак
            int LeaderTermSign;
            if (P.GetCoef(MajorPower)->Calculate() < 0)
                LeaderTermSign = -1;
            else
                LeaderTermSign = +1;
            if (MajorPower % 2 == 0)
                CurSign = LeaderTermSign;  // четная степень
            else
                CurSign = -LeaderTermSign;  // нечетная степень
            /*            {
                if(P.GetCoef(MajorPower).Calculate() < 0) CurSign = LeaderTermSign; //-x^3 > 0 при x = -inf
                else CurSign = -LeaderTermSign; //x^3 < 0 при x = -inf
            };*/

            PNumeric LeftX = TNumeric::create(NumericMinusInf);
            bool IncludeLeft = false;
            for (size_t i = 0; i <= E.Roots.size(); i++) {
                PNumeric RightX;
                bool IncludeRight;
                if (i < E.Roots.size()) {
                    RightX = E.Roots[i];
                    IncludeRight = !Strict;
                } else {
                    RightX = TNumeric::create(NumericPlusInf);
                    IncludeRight = false;
                };

                bool Include = false;  // итсина, если многочлен на текущем сегменте LeftX..RightX удовлетворяет неравенству
                if (Less && CurSign == -1)
                    Include = true;
                if (!Less && CurSign == 1)
                    Include = true;
                if (Include)
                    Result = Result + TInterval(*LeftX, *RightX, IncludeLeft, IncludeRight);

                // переходим к следующему участку
                if (i < E.Roots.size() && E.RootsMultiplicity[i] % 2 == 1)
                    CurSign = -CurSign;
                LeftX = RightX;
                IncludeLeft = IncludeRight;
            }
        }
    } else {
        if (Writer)
            Writer->WriteError("Can not find roots.");
        res = false;
    }
    if (Writer)
        Writer->AddFormula(MakeBelongsTo(UnknownVar, Result.asNumeric()));
    return res;
}

vector<std::shared_ptr<TNumeric>> TPolynomialInequality::GetTypes(std::shared_ptr<const TNumeric> N) {
    vector<std::shared_ptr<TNumeric>> Types;
    if (N != Conditions)
        return Types;
    Types.push_back(TPolynomialInequality(asPolynom(), false, false).Conditions);
    Types.push_back(TPolynomialInequality(asPolynom(), true, false).Conditions);
    Types.push_back(TPolynomialInequality(asPolynom(), false, true).Conditions);
    Types.push_back(TPolynomialInequality(asPolynom(), true, true).Conditions);
    return Types;
}

void TPolynomialInequality::SetType(std::shared_ptr<TNumeric> N, size_t Type) {
    if (N == Conditions)  // Проверяем, чтобы клик был по всему уравнению
    {
        switch (Type) {
            case 0:
                SetType(false, false);
                break;
            case 1:
                SetType(true, false);
                break;
            case 2:
                SetType(false, true);
                break;
            case 3:
                SetType(true, true);
                break;
            default:
                break;
        }
    }
}

void TPolynomialInequality::SaveToFile(ofstream& f) {
    f.write((char*)&Strict, sizeof(Strict));
    f.write((char*)&Less, sizeof(Less));
    TPolynomConditions::SaveToFile(f);
}

void TPolynomialInequality::LoadFromFile(ifstream& f) {
    f.read((char*)&Strict, sizeof(Strict));
    f.read((char*)&Less, sizeof(Less));
    TPolynomConditions::LoadFromFile(f);
}

//*****************************************************************************************************************************
//*****************************************************************************************************************************
TLinearInequality::TLinearInequality(bool Less, bool Strict) : TPolynomialInequality(1, Less, Strict) {
    CanRandomize = true;
    BuildPhrases();
}

TLinearInequality::TLinearInequality(const TLinearInequality* L, bool Less, bool Strict) : TPolynomialInequality(L->asPolynom(), Less, Strict) {
    BuildPhrases();
}

TLinearInequality::~TLinearInequality() {}
void TLinearInequality::BuildPhrases() {
    if (MyTranslator.CheckDictionary(GetClassName()))
        return;
    MyTranslator.AddDictionary(GetClassName());
    MyTranslator.AddEng("Error: can't find coefficients");
    MyTranslator.AddRus("Ошибка: не могу прочитать коэффициенты");
    MyTranslator.AddEng("Because coefficient in linear term is less than zero, the direction of inequality is reversed");
    MyTranslator.AddRus("Так как коэффициент при линейном члене отрицателен, то неравенство меняет знак.");
    MyTranslator.AddEng("No solution");
    MyTranslator.AddRus("Решений нет");
    MyTranslator.AddEng("Solve strict linear inequality");
    MyTranslator.AddRus("Решить строгое линейное неравенство");
    MyTranslator.AddEng("Solve unstrict linear inequality");
    MyTranslator.AddRus("Решить нестрогое линейное неравенство");
    MyTranslator.AddEng("Solve linear inequality");
    MyTranslator.AddRus("Решить линейное неравенство");
}
vector<string> TLinearInequality::GetKeyWords() {
    if (MyTranslator.CheckDictionary(GetClassName() + "Keywords") == false) {
        MyTranslator.AddDictionary(GetClassName() + "Keywords");
        MyTranslator.AddEng("inequality");
        MyTranslator.AddRus("неравенство");
        MyTranslator.AddEng("linear");
        MyTranslator.AddRus("линейное");
    }
    vector<string> Res;
    Res.push_back(MyTranslator.tr("inequality"));
    Res.push_back(MyTranslator.tr("linear"));
    return Res;
}

string TLinearInequality::GetTask() {
    if (Strict)
        return MyTranslator.tr("Solve strict linear inequality");
    else
        return MyTranslator.tr("Solve unstrict linear inequality");
}

string TLinearInequality::GetShortTask() {
    return MyTranslator.tr("Solve linear inequality");
}

bool TLinearInequality::GetSolution(std::shared_ptr<THTMLWriter> Writer) {
    TNumeric& a = *GetCoefP(1);
    TNumeric& b = *GetCoefP(0);
    TNumeric& c = *GetRightPartP();
    Result.Intervals.clear();

    if (!(a.CanCalculate() && b.CanCalculate() && c.CanCalculate()))
        return false;

    Result.Intervals.clear();

    if ((Less && a.Calculate() > 0) || (!Less && a.Calculate() < 0)) {
        TNumeric X = (c - b) / a;
        TNumeric XSimplified = X.Simplify();
        if (Writer)
            Writer->AddFormula(MakeBelongsTo(UnknownVar, MakeInterval(NumericMinusInf, XSimplified, false, !Strict)));
        Result.Intervals.push_back(TInterval(NumericMinusInf, XSimplified, false, !Strict));
    };
    if ((Less && a.Calculate() < 0) || (!Less && a.Calculate() > 0)) {
        if (Writer)
            Writer->AddParagraph("Because coefficient in linear term is less than zero, the direction of inequality is reversed");
        TNumeric X = (c - b) / a;
        TNumeric XSimplified = X.Simplify();
        if (Writer)
            Writer->AddFormula(MakeBelongsTo(UnknownVar, MakeInterval(XSimplified, NumericPlusInf, !Strict, false)));
        Result.Intervals.push_back(TInterval(XSimplified, NumericPlusInf, !Strict, false));
    };

    if (a.Calculate() == 0) {
        // прямая горизонтальна
        if ((Less && b.Calculate() < c.Calculate()) || (!Less && b.Calculate() > c.Calculate()) || (b.Calculate() == c.Calculate() && !Strict)) {
            if (Writer)
                Writer->AddFormula(MakeBelongsTo(UnknownVar, MakeInterval(NumericMinusInf, NumericPlusInf)));
            Result.Intervals.push_back(TInterval(NumericMinusInf, NumericPlusInf, false, false));
        } else {
            if (Writer)
                Writer->AddParagraph("No solution");
            if (Writer)
                Writer->AddFormula(MakeBelongsTo(UnknownVar, EmptySet));
            Result.Intervals.clear();
        }
    }
    return true;
}

void TLinearInequality::Randomize(std::mt19937& rng) {
    switch (rng() % 4) {
        case 0:
            SetType(true, true);
            break;
        case 1:
            SetType(false, true);
            break;
        case 2:
            SetType(true, false);
            break;
        case 3:
            SetType(false, false);
            break;
    }
    TLinearEquality E;
    E.Randomize(rng);
    SetLeftPartP(TPolynom(E.GetCoef()), true);
    SetRightPart(E.GetRightPart());
}

/*vector<TNumeric> TLinearInequality::GetTypes(std::shared_ptr<TNumeric> N)
{
vector<TNumeric> Types;
    if(N != Conditions) return Types;
    Types.push_back(TNumeric(*TLinearInequality(this, false, false).Conditions));
    Types.push_back(TNumeric(*TLinearInequality(this, true, false).Conditions));
    Types.push_back(TNumeric(*TLinearInequality(this, false, true).Conditions));
    Types.push_back(TNumeric(*TLinearInequality(this, true, true).Conditions));
    return Types;
}

void TLinearInequality::SetType(std::shared_ptr<TNumeric> N, size_t Type)
{
    if(N == Conditions)
    {
        switch(Type)
        {
            case 0: Less = false; Strict = false; Conditions->operation = OperatorGreaterOrEqual; break;
            case 1: Less = true;  Strict = false; Conditions->operation = OperatorLessOrEqual; break;
            case 2: Less = false; Strict = true;  Conditions->operation = OperatorGreater; break;
            case 3: Less = true;  Strict = true;  Conditions->operation = OperatorLess; break;
            default: break;
        }
    }
}*/

//**************************************************************************************
//**************************************************************************************
TSquareInequality::TSquareInequality(bool Less, bool Strict) : TPolynomialInequality(2, Less, Strict) {
    CanRandomize = true;
    BuildPhrases();
}

TSquareInequality::TSquareInequality(const TSquareInequality* L, bool Less, bool Strict) : TPolynomialInequality(L->asPolynom(), Less, Strict) {
    BuildPhrases();
}

TSquareInequality::~TSquareInequality() {}
void TSquareInequality::BuildPhrases() {
    if (MyTranslator.CheckDictionary(GetClassName()))
        return;
    MyTranslator.AddDictionary(GetClassName());
    MyTranslator.AddEng("Error: can't find coefficients");
    MyTranslator.AddRus("Ошибка: не могу прочитать коэффициенты");
    MyTranslator.AddEng("Major (square) term is zero so inequality is degenerated.");
    MyTranslator.AddRus("Коэффициент при квадратном члене равен нулю, неравенство вырождено");
    MyTranslator.AddEng("Linear term is zero so equality corresponds to horizontal line.");
    MyTranslator.AddRus("Коэффициент при линейном члене равен нулю, равенство задает горизонтальную линию");
    MyTranslator.AddEng("Free term is less than zero so line is below X axis.");
    MyTranslator.AddRus("Свободный член меньше нуля, прямая находится под осью X");
    MyTranslator.AddEng("Free term is zero so line coincides with X axis.");
    MyTranslator.AddRus("Свободный член равен нулю, прямая совпадает с осью X");
    MyTranslator.AddEng("Free term is greater than zero so line is above X axis.");
    MyTranslator.AddRus("Свободный член больше нуля, прямая над осью X");
    MyTranslator.AddEng("Linear term is greater than zero so only root exists.");
    MyTranslator.AddRus("Линейный член больше нуля, следовательно существует единственный корень");
    MyTranslator.AddEng("Linear term is less than zero so only root exists.");
    MyTranslator.AddRus("Линейный член меньше нуля, следовательно существует единственный корень");
    MyTranslator.AddEng("Finding roots of corresponding square equality.");
    MyTranslator.AddRus("Ищем корни соответствующего квадратного уравнения");
    MyTranslator.AddEng("Major (square) coefficient is less than zero so parabola looks down.");
    MyTranslator.AddRus("Коэффициент при квадратичном члене меньше нуля, ветви параболы направлены вниз.");
    MyTranslator.AddEng("Major (square) coefficient is greater than zero so parabola looks up.");
    MyTranslator.AddRus("Коэффициент при квадратичном члене больше нуля, ветви параболы направлены вверх.");
    MyTranslator.AddEng("Parabola intersects X axis at only one point.");
    MyTranslator.AddRus("Парабола пересекает ось X в единственной точке");
    MyTranslator.AddEng("Major (square) coefficient is less than zero so parabola looks down.");
    MyTranslator.AddRus("Коэффициент при квадратичном члене меньше нуля, ветви параболы направлены вниз.");
    MyTranslator.AddEng("Major (square) coefficient is greater than zero so parabola looks up.");
    MyTranslator.AddRus("Коэффициент при квадратичном члене больше нуля, ветви параболы направлены вверх.");
    MyTranslator.AddEng("Major (square) coefficient is less than zero so parabola looks down.");
    MyTranslator.AddRus("Коэффициент при квадратичном члене меньше нуля, ветви параболы направлены вниз.");
    MyTranslator.AddEng("Major (square) coefficient is greater than zero so parabola looks up.");
    MyTranslator.AddRus("Коэффициент при квадратичном члене больше нуля, ветви параболы направлены вверх.");
    MyTranslator.AddEng("Solve square inequality");
    MyTranslator.AddRus("Решить квадратное неравенство");
    MyTranslator.AddEng("square inequality");
    MyTranslator.AddRus("квадратное неравенство");
}

vector<string> TSquareInequality::GetKeyWords() {
    if (MyTranslator.CheckDictionary(GetClassName() + "Keywords") == false) {
        MyTranslator.AddDictionary(GetClassName() + "Keywords");
        MyTranslator.AddEng("inequality");
        MyTranslator.AddRus("неравенство");
        MyTranslator.AddEng("square");
        MyTranslator.AddRus("квадратное");
    }
    vector<string> Res;
    Res.push_back(MyTranslator.tr("inequality"));
    Res.push_back(MyTranslator.tr("square"));
    return Res;
}

string TSquareInequality::GetTask() {
    return MyTranslator.tr("Solve square inequality");
}

string TSquareInequality::GetShortTask() {
    return MyTranslator.tr("square inequality");
}

/*void TSquareInequality::SetCoef(const TNumeric& a, const TNumeric &b, const TNumeric &c)
{
    std::shared_ptr<TNumeric> a1 = Conditions->GetByRole(IDa);
    std::shared_ptr<TNumeric> b1 = Conditions->GetByRole(IDb);
    std::shared_ptr<TNumeric> c1 = Conditions->GetByRole(IDc);
    *(a1) = a;
    *(b1) = b;
    *(c1) = c;
}*/

bool TSquareInequality::GetSolution(std::shared_ptr<THTMLWriter> Writer) {
    TSquareEquality Eq;
    TNumeric& a = *GetCoefP(2);
    TNumeric& b = *GetCoefP(1);
    TNumeric& c = *GetCoefP(0);
    if (!(a.CanCalculate() && b.CanCalculate() && c.CanCalculate()))
        return false;

    if (!(a.CanCalculate() && b.CanCalculate() && c.CanCalculate()))
        return false;

    Result.Intervals.clear();

    if (a.Calculate() == 0) {
        // вырожденное квадратное уравнение
        if (Writer)
            Writer->AddParagraph("Major (square) term is zero so inequality is degenerated.");
        if (b.Calculate() == 0) {
            // корней нет, то есть прямая горизонтальна
            if (Writer)
                Writer->AddParagraph("Linear term is zero so equality corresponds to horizontal line.");
            if (c.Calculate() < 0) {
                // прямая ниже оси X
                if (Writer)
                    Writer->AddParagraph("Free term is less than zero so line is below X axis.");
                if (Less) {
                    if (Writer)
                        Writer->AddFormula(MakeBelongsTo(UnknownVar, NumericAllReal));
                    Result.Intervals.push_back(IntervalAllRealNumbers);
                } else {
                    if (Writer)
                        Writer->AddFormula(MakeBelongsTo(UnknownVar, EmptySet));
                    Result.Intervals.clear();
                }
            };
            if (c.Calculate() == 0) {
                if (Writer)
                    Writer->AddParagraph("Free term is zero so line coincides with X axis.");
                if (Strict) {
                    if (Writer)
                        Writer->AddFormula(MakeBelongsTo(UnknownVar, EmptySet));
                    Result.Intervals.clear();
                } else {
                    if (Writer)
                        Writer->AddFormula(MakeBelongsTo(UnknownVar, NumericAllReal));
                    Result.Intervals.push_back(IntervalAllRealNumbers);
                }
            }
            if (c.Calculate() > 0) {
                if (Writer)
                    Writer->AddParagraph("Free term is greater than zero so line is above X axis.");
                if (Less) {
                    if (Writer)
                        Writer->AddFormula(MakeBelongsTo(UnknownVar, EmptySet));
                    Result.Intervals.clear();
                } else {
                    if (Writer)
                        Writer->AddFormula(MakeBelongsTo(UnknownVar, NumericAllReal));
                    Result.Intervals.push_back(IntervalAllRealNumbers);
                }
            }
        };
        if (b.Calculate() > 0) {
            // должен быть один корень
            if (Writer)
                Writer->AddParagraph("Linear term is greater than zero so only root exists.");
            TNumeric X = TNumeric("-1") * c / b;
            TNumeric XSimplified = X.Simplify();
            if (Writer)
                Writer->AddFormula(MakeEquality(UnknownVar, XSimplified));
            if (Less) {
                if (Writer)
                    Writer->AddFormula(MakeInterval(NumericMinusInf, XSimplified, false, !Strict));
                Result.Intervals.push_back(TInterval(NumericMinusInf, XSimplified, false, !Strict));
            } else {
                if (Writer)
                    Writer->AddFormula(MakeInterval(XSimplified, NumericPlusInf, !Strict, false));
                Result.Intervals.push_back(TInterval(XSimplified, NumericMinusInf, !Strict, false));
            }
        };
        if (b.Calculate() < 0) {
            // должен быть один корень
            if (Writer)
                Writer->AddParagraph("Linear term is less than zero so only root exists.");
            TNumeric X = TNumeric("-1") * c / b;
            TNumeric XSimplified = X.Simplify();
            if (Writer)
                Writer->AddFormula(MakeEquality(UnknownVar, XSimplified));
            if (Less) {
                if (Writer)
                    Writer->AddFormula(MakeInterval(XSimplified, NumericPlusInf, !Strict, false));
                Result.Intervals.push_back(TInterval(XSimplified, NumericMinusInf, !Strict, false));
            } else {
                if (Writer)
                    Writer->AddFormula(MakeInterval(NumericMinusInf, XSimplified, false, !Strict));
                Result.Intervals.push_back(TInterval(NumericMinusInf, XSimplified, false, !Strict));
            }
        };
    } else {
        TSquareEquality Eq;
        Eq.SetLeftPart(a, b, c);
        TIntervalsSet Roots;

        if (Writer)
            Writer->AddParagraph("Finding roots of corresponding square equality.");
        if (Writer)
            Writer->IncrementNestingLevel();
        Eq.GetSolution(Writer);
        if (Writer)
            Writer->DecrementNestingLevel();
        Roots = Eq.Result;

        if (Roots.Intervals.size() == 0) {
            // корней нет
            if (a.Calculate() < 0) {
                if (Writer)
                    Writer->AddParagraph("Major (square) coefficient is less than zero so parabola looks down.");
                if (Less) {
                    if (Writer)
                        Writer->AddFormula(MakeBelongsTo(UnknownVar, NumericAllReal));
                    Result.Intervals.push_back(IntervalAllRealNumbers);
                } else {
                    if (Writer)
                        Writer->AddFormula(MakeBelongsTo(UnknownVar, EmptySet));
                    Result.Intervals.clear();
                }
            } else {
                if (Writer)
                    Writer->AddParagraph("Major (square) coefficient is greater than zero so parabola looks up.");
                if (Less) {
                    if (Writer)
                        Writer->AddFormula(MakeBelongsTo(UnknownVar, EmptySet));
                    Result.Intervals.clear();
                } else {
                    if (Writer)
                        Writer->AddFormula(MakeBelongsTo(UnknownVar, NumericAllReal));
                    Result.Intervals.push_back(IntervalAllRealNumbers);
                }
            };
        };
        if (Roots.Intervals.size() == 1) {
            // один корень
            if (Writer)
                Writer->AddParagraph("Parabola intersects X axis at only one point.");
            TNumeric X = Roots.Intervals[0].Left;

            if (a.Calculate() < 0) {
                if (Writer)
                    Writer->AddParagraph("Major (square) coefficient is less than zero so parabola looks down.");
                if (Less)
                    if (Strict) {
                        if (Writer)
                            Writer->AddFormula(
                                MakeBelongsTo(UnknownVar, MakeUnion(MakeInterval(NumericMinusInf, TNumeric(X), false, !Strict), MakeInterval(TNumeric(X), NumericPlusInf, false, false))));
                        Result.Intervals.push_back(TInterval(NumericMinusInf, X, false, !Strict));
                    } else {
                        if (Writer)
                            Writer->AddFormula(MakeBelongsTo(UnknownVar, NumericAllReal));
                        Result.Intervals.push_back(TInterval(NumericMinusInf, NumericPlusInf));
                    }
                else if (Strict) {
                    if (Writer)
                        Writer->AddFormula(MakeBelongsTo(UnknownVar, EmptySet));
                    Result.Intervals.clear();
                } else {
                    if (Writer)
                        Writer->AddFormula(MakeBelongsTo(UnknownVar, MakeInline(TNumeric("{"), X, TNumeric("}"))));
                    Result.Intervals.push_back(TInterval(X, X, !Strict, !Strict));
                }

            } else {
                if (Writer)
                    Writer->AddParagraph("Major (square) coefficient is greater than zero so parabola looks up.");
                if (Less)
                    if (Strict) {
                        if (Writer)
                            Writer->AddFormula(MakeBelongsTo(UnknownVar, EmptySet));
                        Result.Intervals.clear();
                    } else {
                        if (Writer)
                            Writer->AddFormula(MakeBelongsTo(UnknownVar, MakeInline(TNumeric("{"), X, TNumeric("}"))));
                        Result.Intervals.push_back(TInterval(X, X, !Strict, !Strict));
                    }
                else if (Strict) {
                    if (Writer)
                        Writer->AddFormula(MakeBelongsTo(UnknownVar, MakeUnion(MakeInterval(NumericMinusInf, TNumeric(X), false, !Strict), MakeInterval(TNumeric(X), NumericPlusInf, false, false))));
                    Result.Intervals.push_back(TInterval(NumericMinusInf, X, false, !Strict));
                    Result.Intervals.push_back(TInterval(X, NumericPlusInf, !Strict, false));
                } else {
                    if (Writer)
                        Writer->AddFormula(MakeBelongsTo(UnknownVar, NumericAllReal));
                    Result.Intervals.push_back(TInterval(NumericMinusInf, NumericPlusInf));
                }
            };
        };
        if (Roots.Intervals.size() == 2) {
            // два корня
            TNumeric X1 = Roots.Intervals[0].Left;
            TNumeric X2 = Roots.Intervals[1].Left;
            if (X1.Calculate() > X2.Calculate()) {
                TNumeric T = X1;
                X1 = X2;
                X2 = T;
            }

            if (a.Calculate() < 0) {
                if (Writer)
                    Writer->AddParagraph("Major (square) coefficient is less than zero so parabola looks down.");
            } else {
                if (Writer)
                    Writer->AddParagraph("Major (square) coefficient is greater than zero so parabola looks up.");
            }
            if ((Less && a.Calculate() < 0) || (!Less && a.Calculate() > 0)) {
                if (Writer)
                    Writer->AddFormula(MakeBelongsTo(UnknownVar, MakeUnion(MakeInterval(NumericMinusInf, TNumeric(X1), false, !Strict), MakeInterval(TNumeric(X2), NumericPlusInf, !Strict, false))));
                Result.Intervals.push_back(TInterval(NumericMinusInf, X1, false, !Strict));
                Result.Intervals.push_back(TInterval(X2, NumericMinusInf, !Strict, false));
            } else {
                if (Writer)
                    Writer->AddFormula(MakeBelongsTo(UnknownVar, MakeInterval(X1, X2, !Strict, !Strict)));
                Result.Intervals.push_back(TInterval(X1, X2, !Strict, !Strict));
            };
        };
    }
    return true;
}

void TSquareInequality::Randomize(std::mt19937& rng) {
    switch (rng() % 4) {
        case 0:
            SetType(true, true);
            break;
        case 1:
            SetType(false, true);
            break;
        case 2:
            SetType(true, false);
            break;
        case 3:
            SetType(false, false);
            break;
    }
    TSquareEquality E;
    E.Randomize(rng);
    SetLeftPartP(TPolynom(E.GetCoef()), true);
}

/*vector<TNumeric> TSquareInequality::GetTypes(std::shared_ptr<TNumeric> N)
{
vector<TNumeric> Types;
    if(N != Conditions) return Types;
    Types.push_back(TNumeric(*TSquareInequality(this, false, false).Conditions));
    Types.push_back(TNumeric(*TSquareInequality(this, true, false).Conditions));
    Types.push_back(TNumeric(*TSquareInequality(this, false, true).Conditions));
    Types.push_back(TNumeric(*TSquareInequality(this, true, true).Conditions));
    return Types;
}

void TSquareInequality::SetType(std::shared_ptr<TNumeric> N, size_t Type)
{
    if(N == Conditions)
    {
        switch(Type)
        {
            case 0: Less = false; Strict = false; Conditions->operation = OperatorGreaterOrEqual; break;
            case 1: Less = true;  Strict = false; Conditions->operation = OperatorLessOrEqual; break;
            case 2: Less = false; Strict = true;  Conditions->operation = OperatorGreater; break;
            case 3: Less = true;  Strict = true;  Conditions->operation = OperatorLess; break;
            default: break;
        }
    }
}*/
//*****************************************************************************************************************************
template<class TInequalityClass, const char* ClassName>
TSetOfInequalities<TInequalityClass, ClassName>::TSetOfInequalities() {
    this->Conditions = std::make_shared<TNumeric>();
    this->Conditions->operation = OperatorEqSet;
    this->Conditions->OperandsPushback(*(FirstInequality.Conditions));
    this->Conditions->OperandsPushback(*(SecondInequality.Conditions));
    // this->Solution = 0;
    CanRandomize = true;
    BuildPhrases();
}

template<class TInequalityClass, const char* ClassName>
TSetOfInequalities<TInequalityClass, ClassName>::~TSetOfInequalities() {}

template<class TInequalityClass, const char* ClassName>
string TSetOfInequalities<TInequalityClass, ClassName>::GetTask() {
    return MyTranslator.tr("Solve set of inequalities");
}

template<class TInequalityClass, const char* ClassName>
string TSetOfInequalities<TInequalityClass, ClassName>::GetShortTask() {
    return MyTranslator.tr("set of inequalities");
}

template<class TInequalityClass, const char* ClassName>
void TSetOfInequalities<TInequalityClass, ClassName>::BuildPhrases() {
    if (MyTranslator.CheckDictionary("TSetOfInequalities"))
        return;
    MyTranslator.AddDictionary("TSetOfInequalities");
    MyTranslator.AddEng("Solving first inequality");
    MyTranslator.AddRus("Решаем первое неравенство");
    MyTranslator.AddEng("Solving second inequality");
    MyTranslator.AddRus("Решаем второе неравенство");
    MyTranslator.AddEng("Common solution:");
    MyTranslator.AddRus("Общее решение");

    MyTranslator.AddEng("Solve set of inequalities");
    MyTranslator.AddRus("Решить совокупность неравенств");
    MyTranslator.AddEng("set of inequalities");
    MyTranslator.AddRus("совокупность неравенств");
}

template<class TInequalityClass, const char* ClassName>
vector<string> TSetOfInequalities<TInequalityClass, ClassName>::GetKeyWords() {
    if (MyTranslator.CheckDictionary("TSetOfInequalitiesKeywords") == false) {
        MyTranslator.AddDictionary("TSetOfInequalitiesKeywords");
        MyTranslator.AddEng("set");
        MyTranslator.AddRus("совокупность");
        MyTranslator.AddEng("inequality");
        MyTranslator.AddRus("неравенство");
    }
    vector<string> Res;
    Res.push_back(MyTranslator.tr("set"));
    Res.push_back(MyTranslator.tr("inequality"));
    return Res;
}

template<class TInequalityClass, const char* ClassName>
bool TSetOfInequalities<TInequalityClass, ClassName>::GetSolution(std::shared_ptr<THTMLWriter> Writer) {
    FirstInequality.Conditions = std::make_shared<TNumeric>(*(this->Conditions->operands[0]));
    if (Writer)
        Writer->AddParagraph("Solving first inequality");
    if (Writer)
        Writer->IncrementNestingLevel();
    bool res1 = this->FirstInequality.GetSolution(Writer);
    if (Writer)
        Writer->DecrementNestingLevel();
    if (res1 == false)
        return false;

    SecondInequality.Conditions = std::make_shared<TNumeric>(*(this->Conditions->operands[1]));
    if (Writer)
        Writer->AddParagraph("Solving second inequality");
    if (Writer)
        Writer->IncrementNestingLevel();
    bool res2 = this->SecondInequality.GetSolution(Writer);
    if (Writer)
        Writer->DecrementNestingLevel();
    if (res2 == false)
        return false;

    if (Writer)
        Writer->AddParagraph("Common solution:");
    TIntervalsSet Res;
    Res = this->FirstInequality.Result + this->SecondInequality.Result;
    if (Writer)
        Writer->AddFormula(MakeBelongsTo(TNumeric("x"), Res.asNumeric()));

    TInequality::Result = Res;
    return true;
}

template<class TInequalityClass, const char* ClassName>
vector<std::shared_ptr<TNumeric>> TSetOfInequalities<TInequalityClass, ClassName>::GetTypes(std::shared_ptr<const TNumeric> N) {
    vector<std::shared_ptr<TNumeric>> Res;
    if (*N == *this->Conditions->operands[0]) {
        FirstInequality.Conditions = std::make_shared<TNumeric>(*N);
        Res = FirstInequality.GetTypes(FirstInequality.Conditions);
    };
    if (*N == *this->Conditions->operands[1]) {
        SecondInequality.Conditions = std::make_shared<TNumeric>(*N);
        Res = SecondInequality.GetTypes(SecondInequality.Conditions);
    }
    return Res;
}

template<class TInequalityClass, const char* ClassName>
void TSetOfInequalities<TInequalityClass, ClassName>::SetType(std::shared_ptr<TNumeric> N, size_t Type) {
    if (*N == *Conditions->operands[0]) {
        FirstInequality.SetType(FirstInequality.Conditions, Type);
        Conditions->operands[0] = std::make_shared<TNumeric>(*FirstInequality.Conditions);
    };
    if (*N == *this->Conditions->operands[1]) {
        SecondInequality.SetType(SecondInequality.Conditions, Type);
        Conditions->operands[1] = std::make_shared<TNumeric>(*SecondInequality.Conditions);
    }
}

template<class TInequalityClass, const char* ClassName>
void TSetOfInequalities<TInequalityClass, ClassName>::Randomize(std::mt19937& rng) {
    if (FirstInequality.CanRandomize)
        FirstInequality.Randomize(rng);
    if (SecondInequality.CanRandomize)
        SecondInequality.Randomize(rng);
    this->Conditions->operation = OperatorEqSet;
    this->Conditions->OperandsClear();
    this->Conditions->OperandsPushback(*(FirstInequality.Conditions));
    this->Conditions->OperandsPushback(*(SecondInequality.Conditions));
}

template<class TInequalityClass, const char* ClassName>
void TSetOfInequalities<TInequalityClass, ClassName>::SaveToFile(ofstream& f) {
    this->Conditions->SaveToFile(f);
    /*        FirstInequality.SaveToFile(f);
            SecondInequality.SaveToFile(f);*/
}
template<class TInequalityClass, const char* ClassName>
void TSetOfInequalities<TInequalityClass, ClassName>::LoadFromFile(ifstream& f) {
    this->Conditions->LoadFromFile(f);
    /*        FirstInequality.LoadFromFile(f);
            SecondInequality.LoadFromFile(f);*/
}

template<class TInequalityClass, const char* ClassName>
TSystemOfInequalities<TInequalityClass, ClassName>::TSystemOfInequalities() : TSetOfInequalities<TInequalityClass, ClassName>() {
    this->Conditions->operation = OperatorEqSystem;
    BuildPhrases();
};

template<class TInequalityClass, const char* ClassName>
bool TSystemOfInequalities<TInequalityClass, ClassName>::GetSolution(std::shared_ptr<THTMLWriter> Writer) {
    this->FirstInequality.Conditions = std::make_shared<TNumeric>(*(this->Conditions->operands[0]));
    if (Writer)
        Writer->AddParagraph("Solving first inequality");
    if (Writer)
        Writer->IncrementNestingLevel();
    bool res1 = this->FirstInequality.GetSolution(Writer);
    if (Writer)
        Writer->DecrementNestingLevel();
    if (res1 == false)
        return false;

    this->SecondInequality.Conditions = std::make_shared<TNumeric>(*(this->Conditions->operands[1]));
    if (Writer)
        Writer->AddParagraph("Solving second inequality");
    if (Writer)
        Writer->IncrementNestingLevel();
    bool res2 = this->SecondInequality.GetSolution(Writer);
    if (Writer)
        Writer->DecrementNestingLevel();
    if (res2 == false)
        return false;

    if (Writer)
        Writer->AddParagraph("Common solution:");
    TIntervalsSet Res;
    Res = this->FirstInequality.Result * this->SecondInequality.Result;
    if (Writer)
        Writer->AddFormula(MakeBelongsTo(TNumeric("x"), Res.asNumeric()));

    ((TInequality*)this)->Result = Res;
    return true;
}

template<class TInequalityClass, const char* ClassName>
void TSystemOfInequalities<TInequalityClass, ClassName>::BuildPhrases() {
    if (MyTranslator.CheckDictionary("TSystemOfInequalities"))
        return;
    MyTranslator.AddDictionary("TSystemOfInequalities");

    MyTranslator.AddEng("Solve system of inequalities");
    MyTranslator.AddRus("Решить систему неравенств");
    MyTranslator.AddEng("system of inequalities");
    MyTranslator.AddRus("система неравенств");
}

template<class TInequalityClass, const char* ClassName>
vector<string> TSystemOfInequalities<TInequalityClass, ClassName>::GetKeyWords() {
    if (MyTranslator.CheckDictionary("TSystemOfInequalitiesKeywords") == false) {
        MyTranslator.AddDictionary("TSystemOfInequalitiesKeywords");
        MyTranslator.AddEng("system");
        MyTranslator.AddRus("система");
        MyTranslator.AddEng("inequality");
        MyTranslator.AddRus("неравенство");
    }
    vector<string> Res;
    Res.push_back(MyTranslator.tr("system"));
    Res.push_back(MyTranslator.tr("inequality"));
    return Res;
}
//********************************************************************************************************************** */
TPolynomDerivative::TPolynomDerivative(size_t MaxPower) : TPolynomConditions(MaxPower, false) {
    CanRandomize = true;
    BuildPhrases();
}
TPolynomDerivative::~TPolynomDerivative() {}
void TPolynomDerivative::BuildPhrases() {
    if (MyTranslator.CheckDictionary(GetClassName()))
        return;
    MyTranslator.AddDictionary(GetClassName());
    MyTranslator.AddEng("Find derivative of polynom of degree %d");
    MyTranslator.AddRus("Найдите производную многочлена степени %d");
    MyTranslator.AddEng("polynom of degree %d");
    MyTranslator.AddRus("многочлен степени %d");
}

vector<string> TPolynomDerivative::GetKeyWords() {
    if (MyTranslator.CheckDictionary(GetClassName() + "Keywords") == false) {
        MyTranslator.AddDictionary(GetClassName() + "Keywords");
        MyTranslator.AddEng("derivative");
        MyTranslator.AddRus("производная");
        MyTranslator.AddEng("polynom");
        MyTranslator.AddRus("многочлен");
    }
    vector<string> Res;
    Res.push_back(MyTranslator.tr("derivative"));
    Res.push_back(MyTranslator.tr("polynom"));
    return Res;
}

string TPolynomDerivative::GetTask() {
    char Buf[128];
    sprintf(Buf, MyTranslator.tr("Find derivative of polynom of degree %d").c_str(), MaxPower);
    return string(Buf);
}

string TPolynomDerivative::GetShortTask() {
    char Buf[128];
    sprintf(Buf, MyTranslator.tr("polynom of degree %d").c_str(), MaxPower);
    return string(Buf);
}

bool TPolynomDerivative::GetSolution(std::shared_ptr<THTMLWriter> Writer) {
    TPolynom P = asPolynom();
    if (Writer) {
        TPolynom D = P.Derivative();
        TPolynom DSimplified = D;
        for (size_t i = 0; i < DSimplified.Coef.size(); i++)
            DSimplified.Coef[i] = TNumeric::create(DSimplified.Coef[i]->Simplify());
        TNumeric dP;
        dP.operation = OperatorDeriv;
        dP.OperandsPushback(P.asNumeric());
        Writer->AddFormula(MakeEquality(dP, TNumeric(" ")));
        Writer->AddFormula(MakeEquality(D.asNumeric(), TNumeric(" ")));
        Writer->AddFormula(DSimplified.asNumeric());
    }
    return true;
}

void TPolynomDerivative::Randomize(std::mt19937& rng) {
    TPolynom P;
    P.Coef.assign(MaxPower + 1, nullptr);
    std::uniform_int_distribution<int> dist(-20, 20);
    for (size_t i = 0; i <= MaxPower; i++)
        P.Coef[i] = TNumeric::create(dist(rng));
    SetLeftPartP(P);
}

//*****************************************************************************************************************************

TRationalFunctionDerivative::TRationalFunctionDerivative(size_t MaxPowerNumerator, size_t MaxPowerDenominator) :
    TRationalFunctionConditions(OperatorDeriv, false, MaxPowerNumerator, MaxPowerDenominator) {
    CanRandomize = true;
    BuildPhrases();
}
TRationalFunctionDerivative::~TRationalFunctionDerivative() {}

void TRationalFunctionDerivative::BuildPhrases() {
    if (MyTranslator.CheckDictionary(GetClassName()))
        return;
    MyTranslator.AddDictionary(GetClassName());
    MyTranslator.AddEng("Finding derivative of fraction");
    MyTranslator.AddRus("Ищем производную дроби");
    MyTranslator.AddEng("After simplifying: ");
    MyTranslator.AddRus("После упрощения");
    MyTranslator.AddEng("Extracting polynomial part");
    MyTranslator.AddRus("Выделяем главную часть");
    MyTranslator.AddEng("Find derivative of rational function");
    MyTranslator.AddRus("Найти производную рациональной функции");
    MyTranslator.AddEng("rational function of %d/%d powers");
    MyTranslator.AddRus("производная рациональной функции степеней %d/%d");
}

vector<string> TRationalFunctionDerivative::GetKeyWords() {
    if (MyTranslator.CheckDictionary(GetClassName() + "Keywords") == false) {
        MyTranslator.AddDictionary(GetClassName() + "Keywords");
        MyTranslator.AddEng("derivative");
        MyTranslator.AddRus("производная");
        MyTranslator.AddEng("rational");
        MyTranslator.AddRus("рациональная");
    }
    vector<string> Res;
    Res.push_back(MyTranslator.tr("derivative"));
    Res.push_back(MyTranslator.tr("rational"));
    return Res;
}

string TRationalFunctionDerivative::GetTask() {
    return MyTranslator.tr("Find derivative of rational function");
}

string TRationalFunctionDerivative::GetShortTask() {
    char Buf[128];
    sprintf(Buf, MyTranslator.tr("rational function of %d/%d powers").c_str(), MaxPowerNumerator, MaxPowerDenominator);
    return Buf;
}

bool TRationalFunctionDerivative::GetSolution(std::shared_ptr<THTMLWriter> Writer) {
    TRationalFunction R;
    R.P.Coef.assign(MaxPowerNumerator + 1, nullptr);
    for (size_t i = 0; i <= MaxPowerNumerator; i++) {
        auto v = Conditions->GetByRoleConst(TPolynomConditions::RolePowerBegin + i);
        if (!v)
            return false;
        else
            *R.P.Coef[i] = std::move(*v);
    }
    R.Q.Coef.assign(MaxPowerDenominator + 1, nullptr);
    for (size_t i = 0; i <= MaxPowerDenominator; i++) {
        auto v = Conditions->GetByRoleConst(TPolynomConditions::RolePowerBegin + i + MaxPowerNumerator + 1);
        if (!v)
            return false;
        else
            *R.Q.Coef[i] = std::move(*v);
    }
    if (Writer) {
        TRationalFunction D = R.Derivative();
        TRationalFunction DSimplified = D;
        for (size_t i = 0; i < DSimplified.P.Coef.size(); i++)
            DSimplified.P.Coef[i] = TNumeric::create(DSimplified.P.Coef[i]->Simplify());
        for (size_t i = 0; i < DSimplified.Q.Coef.size(); i++)
            DSimplified.Q.Coef[i] = TNumeric::create(DSimplified.Q.Coef[i]->Simplify());
        TNumeric dP;
        TNumeric MainAndO;
        dP.operation = OperatorDeriv;
        dP.OperandsPushback(R.asNumeric());
        MainAndO = DSimplified.GetMainPartAndO();
        Writer->AddFormula(MakeEquality(dP, TNumeric(" ")));
        Writer->AddParagraph("Finding derivative of fraction");
        Writer->AddFormula(MakeEquality(D.asNumeric(), TNumeric(" ")));
        Writer->AddParagraph("After simplifying: ");
        Writer->AddFormula(MakeEquality(DSimplified.asNumeric(), TNumeric(" ")));
        Writer->AddParagraph("Extracting polynomial part");
        Writer->AddFormula(MainAndO);
    }
    return true;
}

void TRationalFunctionDerivative::SaveToFile(ofstream& f) {
    TRationalFunctionConditions::SaveToFile(f);
    TProblem::SaveToFile(f);
}

void TRationalFunctionDerivative::LoadFromFile(ifstream& f) {
    TRationalFunctionConditions::LoadFromFile(f);
    TProblem::LoadFromFile(f);
}

TRationalFunctionConditions::TRationalFunctionConditions(Operation operation, bool HaveRightPart, size_t MaxPowerNumerator, size_t MaxPowerDenominator) {
    this->operation = operation;
    this->MaxPowerNumerator = MaxPowerNumerator;
    this->MaxPowerDenominator = MaxPowerDenominator;
    UnknownVar = TNumeric("x");
    this->HaveRightPart = HaveRightPart;
    SetMaxPower(MaxPowerNumerator, MaxPowerDenominator);
}

/*void TRationalFunctionConditions::Assign(const TRationalFunctionConditions& R)
{
    HaveRightPart = R.HaveRightPart;
    MaxPowerNumerator = R.MaxPowerDenominator;
    MaxPowerDenominator = R.MaxPowerDenominator;
    operation = R.operation;
    UnknownVar = R.UnknownVar;
    *Conditions = *R.Conditions;
}

void TRationalFunctionConditions::operator=(const TRationalFunctionConditions& R)
{
    Assign(R);
}*/
TNumeric TRationalFunctionConditions::GetVarPower(size_t power) {
    if (power == 0)
        return TNumeric("1");
    else {
        if (power == 1)
            return (UnknownVar);
        else
            return (UnknownVar ^ TNumeric(static_cast<int>(power)));
    };
}

void TRationalFunctionConditions::SetMaxPower(size_t MaxPowerNumerator, size_t MaxPowerDenominator) {
    TNumeric Numerator;
    Numerator.operation = OperatorSum;
    for (size_t i = 0; i <= MaxPowerNumerator; i++) {
        TNumeric a(0);
        a.role = GetNumeratorCoefID(i);
        // a.SetEditableFlags(ConstAllowed);
        if (i == 0) {
            Numerator = a;
        } else {
            TNumeric Temp = a * (UnknownVar ^ TNumeric(static_cast<int>(i)));
            Numerator = Temp + Numerator;
        }
    }

    TNumeric Denominator;
    Denominator.operation = OperatorSum;
    for (size_t i = 0; i <= MaxPowerDenominator; i++) {
        TNumeric a(0);
        a.role = GetDenominatorCoefID(i);
        // a.SetEditableFlags(ConstAllowed);
        if (i == 0) {
            a.strval = "1";
            Denominator = a;
        } else {
            TNumeric Temp = a * (UnknownVar ^ TNumeric(static_cast<int>(i)));
            Denominator = Temp + Denominator;
        }
    }
    TNumeric Res;
    Res.operation = OperatorFrac;
    Res.OperandsPushback(Numerator);
    Res.OperandsPushback(Denominator);

    this->MaxPowerNumerator = MaxPowerNumerator;
    this->MaxPowerDenominator = MaxPowerDenominator;

    if (HaveRightPart) {
        TNumeric RightPart("0");
        // RightPart.SetEditableFlags(NoEditable);
        RightPart.role = RightPartID();
        Conditions = std::make_shared<TNumeric>();
        Conditions->operation = operation;
        Conditions->OperandsPushback(Res);
        Conditions->OperandsPushback(RightPart);
    } else {
        Conditions = std::make_shared<TNumeric>(Res);
    }
}

size_t TRationalFunctionConditions::GetNumeratorCoefID(size_t power) {
    return TPolynomConditions::RolePowerBegin + power;
}

size_t TRationalFunctionConditions::GetDenominatorCoefID(size_t power) {
    return TPolynomConditions::RolePowerBegin + MaxPowerNumerator + 1 + power;
}

TRationalFunctionConditions::~TRationalFunctionConditions() {}

void TRationalFunctionConditions::SetRightPart(std::shared_ptr<TNumeric> R) {
    if (HaveRightPart) {
        auto& RP = Conditions->GetByRole(RightPartID(), Conditions);
        RP = R;
        RP->role = RightPartID();
    } else {
    }
}

TNumeric& TRationalFunctionConditions::GetRightPartP() {
    return *Conditions->GetByRole(RightPartID(), Conditions);
}

TNumeric TRationalFunctionConditions::GetRightPart() const {
    return *Conditions->GetByRoleConst(RightPartID());
}

TPolynom TRationalFunctionConditions::GetNumeratorP() {
    vector<TNumeric> Coefs;
    Coefs.assign(MaxPowerNumerator + 1, TNumeric("0"));
    for (size_t i = 0; i <= MaxPowerNumerator; i++)
        Coefs[i] = *Conditions->GetByRoleConst(GetNumeratorCoefID(i));
    return TPolynom(std::move(Coefs));
}

TPolynom TRationalFunctionConditions::GetDenominatorP() {
    vector<TNumeric> Coefs;
    Coefs.assign(MaxPowerDenominator + 1, TNumeric("0"));
    for (size_t i = 0; i <= MaxPowerDenominator; i++)
        Coefs[i] = *Conditions->GetByRole(GetDenominatorCoefID(i), Conditions);
    return TPolynom(std::move(Coefs));
}

void TRationalFunctionConditions::SaveToFile(ofstream& f) {
    uint16_t MaxPowerNumerator = this->MaxPowerNumerator;
    uint16_t MaxPowerDenominator = this->MaxPowerDenominator;
    f.write((char*)&MaxPowerNumerator, sizeof(MaxPowerNumerator));
    f.write((char*)&MaxPowerDenominator, sizeof(MaxPowerDenominator));
    UnknownVar.SaveToFile(f);
}

void TRationalFunctionConditions::LoadFromFile(ifstream& f) {
    uint16_t MaxPowerNumerator;
    uint16_t MaxPowerDenominator;
    f.read((char*)&MaxPowerNumerator, sizeof(MaxPowerNumerator));
    f.read((char*)&MaxPowerDenominator, sizeof(MaxPowerDenominator));
    this->MaxPowerNumerator = MaxPowerNumerator;
    this->MaxPowerDenominator = MaxPowerDenominator;
    UnknownVar.LoadFromFile(f);
}

void TRationalFunctionConditions::SetNumerator(const TPolynom& P, bool AllCoef) {
    if (AllCoef)
        if (P.Coef.size() == 0)
            MaxPowerNumerator = 0;
        else
            MaxPowerNumerator = P.Coef.size() - 1;
    else
        MaxPowerNumerator = P.MajorPower();

    TNumeric Sum;
    for (size_t i = 0; i <= MaxPowerNumerator; i++) {
        PNumeric a = P.GetCoef(i);
        a->role = GetNumeratorCoefID(i);
        // a.SetEditableFlags(ConstAllowed);
        const TNumeric& Temp = GetVarPower(i);
        if (i == 0) {
            if (Temp.operation == OperatorConst && Temp.strval == "1")
                // случай специально для многочленов по x - вместо 0+a*x^0 делаем просто a
                Sum = *a;
            else
                Sum = (*a) * Temp;
        } else {
            Sum = (*a) * Temp + Sum;
        }
    }
    PNumeric N = GetNumerator();
    *N = Sum;
}

void TRationalFunctionConditions::SetDenominator(const TPolynom& P, bool AllCoef) {
    if (AllCoef)
        if (P.Coef.size() == 0)
            MaxPowerDenominator = 0;
        else
            MaxPowerDenominator = P.Coef.size() - 1;
    else
        MaxPowerDenominator = P.MajorPower();

    TNumeric Sum;
    for (size_t i = 0; i <= MaxPowerDenominator; i++) {
        PNumeric a = P.GetCoef(i);
        a->role = GetDenominatorCoefID(i);
        // a.SetEditableFlags(ConstAllowed);
        TNumeric Temp = GetVarPower(i);
        if (i == 0) {
            if (Temp.operation == OperatorConst && Temp.strval == "1")
                // случай специально для многочленов по x - вместо 0+a*x^0 делаем просто a
                Sum = *a;
            else
                Sum = (*a) * Temp;
        } else {
            Sum = (*a) * Temp + Sum;
        }
    }
    PNumeric D = GetDenominator();
    *D = Sum;
}

PNumeric TRationalFunctionConditions::GetNumerator() {
    if (HaveRightPart)
        return Conditions->operands[0]->at(0);
    else
        return Conditions->at(0);
}

PNumeric TRationalFunctionConditions::GetDenominator() {
    if (HaveRightPart)
        return Conditions->operands[0]->at(1);
    else
        return Conditions->at(1);
}

void TRationalFunctionConditions::Randomize(std::mt19937& rng) {
    TPolynom Numerator;
    Numerator.Coef.assign(MaxPowerNumerator + 1, nullptr);
    std::uniform_int_distribution<int> dist(-20, 20);
    for (size_t i = 0; i <= MaxPowerNumerator; i++)
        Numerator.Coef[i] = TNumeric::create(dist(rng));
    SetNumerator(Numerator);
    TPolynom Denominator;
    Denominator.Coef.assign(MaxPowerDenominator + 1, nullptr);
    for (size_t i = 0; i <= MaxPowerDenominator; i++)
        Denominator.Coef[i] = TNumeric::create(dist(rng));
    SetDenominator(Denominator);
}

template class TSetOfInequalities<TLinearInequality, TSetOfLinearInequalitiesStr>;
template class TSystemOfInequalities<TLinearInequality, TSystemOfLinearInequalitiesStr>;
template class TSystemOfInequalities<TSquareInequality, TSystemOfSquareInequalitiesStr>;
template class TSetOfInequalities<TSquareInequality, TSetOfSquareInequalitiesStr>;
template class TSetOfInequalities<TLinearInequality, TSystemOfLinearInequalitiesStr>;
template class TSetOfInequalities<TSquareInequality, TSystemOfSquareInequalitiesStr>;