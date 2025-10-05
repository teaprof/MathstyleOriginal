#include "tsystemofequations.h"

#include <iostream>
#include <sstream>

#include "Base/tphrases.h"
#include "mathomatic.h"
using namespace std;
// #define __DEBUG__

TSystemOfEquations::TSystemOfEquations(size_t VarsCount, size_t EqCount) : TProblem() {
    this->EqCount = EqCount;
    if (VarsCount > 10)
        throw "TSystemOfEquations:: VarsCount > 10";
    for (size_t i = 0; i < VarsCount; i++) {
        PNumeric V = TNumeric::create();
        char v[10];
        if (VarsCount <= 3) {
            strcpy(v, "x");
            v[0] = v[0] + i;
            *V = TNumeric(v);
        } else {
            *V = MakeSubscript(TNumeric("x"), TNumeric(static_cast<int>(i) + 1));
        };
        Variables.push_back(V);
    };

    Conditions = std::make_shared<TNumeric>();
    Conditions->operation = OperatorEqSystem;
    int IDCount = 0;
    for (size_t n = 0; n < EqCount; n++) {
        PNumeric LeftSide = TNumeric::create();
        vector<int> EqIDs;
        EqIDs.assign(VarsCount + 1, 0);
        for (size_t m = 0; m < VarsCount; m++) {
            TNumeric A(IDCount + 1);
            EqIDs[m] = IDCount;
            A.role = IDCount++;
            //A.SetEditableFlags(ConstAllowed);
            if (m == 0) {
                *LeftSide = A * (*Variables[m]);
            } else {
                *LeftSide = *LeftSide + A * (*Variables[m]);
            };
        }
        TNumeric FreeTerm(0);
        //FreeTerm.EditableFlags = ConstAllowed;
        EqIDs[VarsCount] = IDCount;
        FreeTerm.role = IDCount++;
        TNumeric Eq = MakeEquality(*LeftSide, FreeTerm);

        role.push_back(EqIDs);
        Conditions->OperandsPushback(Eq);
    };
    CanRandomize = true;
    BuildPhrases();
}

TSystemOfEquations::~TSystemOfEquations() {
    // nothing to do
}

TSystemOfEquations::TSystemOfEquations(const TSystemOfEquations& E) : TProblem(E) {
    Conditions = 0;
    Assign(E);
    BuildPhrases();
}

void TSystemOfEquations::BuildPhrases() {
    if (MyTranslator.CheckDictionary(GetClassName()))
        return;
    MyTranslator.AddDictionary(GetClassName());
    MyTranslator.AddEng("Can not read coefficients");
    MyTranslator.AddRus("Не могу прочитать коэффициенты");
    MyTranslator.AddEng("Equation above is always false.");
    MyTranslator.AddRus("Это уравнение никогда не удовлетворяется");
    MyTranslator.AddEng("Equation above is always true");
    MyTranslator.AddRus("Это уравнение всегда выполняется");
    MyTranslator.AddEng("From first equation of the system one can obtain");
    MyTranslator.AddRus("Из первого уравнение получаем");
    MyTranslator.AddEng("Substituting to system, we obtain");
    MyTranslator.AddRus("Подставляя в систему, получаем");
    MyTranslator.AddEng("Substituting to system, we obtain");
    MyTranslator.AddRus("Подставляя в систему, получаем");
    MyTranslator.AddEng("After simplifying ");
    MyTranslator.AddRus("После упрощения");
    MyTranslator.AddEng("Can not understand what can be done with first equation.");
    MyTranslator.AddRus("Не могу понять, что делать с первым уравнением");
    MyTranslator.AddEng("Final solution of the system:");
    MyTranslator.AddRus("Окончательное решение системы уравнений");
    MyTranslator.AddEng("The system has no solution");
    MyTranslator.AddRus("Система уравнений не имеет решения");
    MyTranslator.AddEng("Solve system of %d equations with %d unknown variables");
    MyTranslator.AddRus("Решить систему из %d уравнений с %d неизвестными");
    MyTranslator.AddEng("System of equations %d/%d");
    MyTranslator.AddRus("Система уравнений %d на %d");
}

vector<string> TSystemOfEquations::GetKeyWords() {
    if (MyTranslator.CheckDictionary(GetClassName() + "Keywords") == false) {
        MyTranslator.AddDictionary(GetClassName() + "Keywords");
        MyTranslator.AddEng("system");
        MyTranslator.AddRus("система");
        MyTranslator.AddEng("linear");
        MyTranslator.AddRus("линеное");
        MyTranslator.AddEng("equality");
        MyTranslator.AddRus("уравнение");
        MyTranslator.AddEng("SLAE");
        MyTranslator.AddRus("СЛАУ");
    }
    vector<string> Res;
    Res.push_back(MyTranslator.tr("system"));
    Res.push_back(MyTranslator.tr("linear"));
    Res.push_back(MyTranslator.tr("equality"));
    Res.push_back(MyTranslator.tr("SLAE"));
    return Res;
}

string TSystemOfEquations::GetTask() {
    char Buf[128];
    sprintf(Buf, MyTranslator.tr("Solve system of %d equations with %d unknown variables").c_str(), EqCount, VarsCount());
    return string(Buf);
}

string TSystemOfEquations::GetShortTask() {
    char Buf[128];
    sprintf(Buf, MyTranslator.tr("System of equations %d/%d").c_str(), EqCount, VarsCount());
    return string(Buf);
}

void TSystemOfEquations::SaveToFile(ofstream& f) {
    uint16_t VarsCount = this->VarsCount();
    uint16_t EqCount = this->EqCount;

    f.write((char*)&SolutionExists, sizeof(SolutionExists));

    f.write((char*)&VarsCount, sizeof(VarsCount));
    f.write((char*)&EqCount, sizeof(EqCount));

    for (size_t i = 0; i < VarsCount; i++)
        Variables[i]->SaveToFile(f);

    uint16_t IDsize = role.size();
    f.write((char*)&IDsize, sizeof(IDsize));
    for (size_t i = 0; i < IDsize; i++) {
        uint16_t IDSize1 = role[i].size();
        f.write((char*)&IDSize1, sizeof(IDSize1));
        for (size_t j = 0; j < IDSize1; j++) {
            uint16_t role = this->role[i][j];
            f.write((char*)&role, sizeof(role));
        }
    };

    uint16_t CoefsSize = Coefs.size();
    f.write((char*)&CoefsSize, sizeof(CoefsSize));
    for (size_t i = 0; i < CoefsSize; i++) {
        CoefsSize = Coefs[i].size();
        f.write((char*)&CoefsSize, sizeof(CoefsSize));
        for (size_t j = 0; j < VarsCount; j++) {
            Coefs[i][j]->SaveToFile(f);
        }
    };

    uint16_t RightSideSize = RightSide.size();
    f.write((char*)&RightSideSize, sizeof(RightSideSize));
    for (size_t j = 0; j < RightSideSize; j++)
        RightSide[j]->SaveToFile(f);

    uint16_t AnswerSize = Answer.size();
    f.write((char*)&AnswerSize, sizeof(AnswerSize));
    for (size_t i = 0; i < AnswerSize; i++)
        Answer[i]->SaveToFile(f);

    TProblem::SaveToFile(f);
}

void TSystemOfEquations::LoadFromFile(ifstream& f) {
    uint16_t VarsCount;
    uint16_t EqCount;
    f.read((char*)&SolutionExists, sizeof(SolutionExists));
    f.read((char*)&VarsCount, sizeof(VarsCount));
    f.read((char*)&EqCount, sizeof(EqCount));

    this->EqCount = EqCount;

    Variables.clear();
    for (size_t i = 0; i < VarsCount; i++) {
        TNumeric N;
        N.LoadFromFile(f);
        Variables.push_back(TNumeric::create(std::move(N)));
    }

    role.clear();
    uint16_t IDsize;
    f.read((char*)&IDsize, sizeof(IDsize));
    for (size_t i = 0; i < IDsize; i++) {
        uint16_t IDsize1;
        f.read((char*)&IDsize1, sizeof(IDsize));
        vector<int> ID1;
        for (size_t j = 0; j < IDsize1; j++) {
            uint16_t ID2;
            f.read((char*)&ID2, sizeof(ID2));
            ID1.push_back(ID2);
        }
        role.push_back(ID1);
    };

    Coefs.clear();
    uint16_t CoefsSize;
    f.read((char*)&CoefsSize, sizeof(CoefsSize));
    for (size_t i = 0; i < CoefsSize; i++) {
        uint16_t CoefsSize1;
        f.read((char*)&CoefsSize1, sizeof(CoefsSize1));
        vector<PNumeric> Coefs1;
        for (size_t j = 0; j < CoefsSize1; j++) {
            TNumeric N;
            N.LoadFromFile(f);
            Coefs1.push_back(TNumeric::create(std::move(N)));
        }
        Coefs.push_back(Coefs1);
    };

    RightSide.clear();
    uint16_t RightSideSize;
    f.read((char*)&RightSideSize, sizeof(RightSideSize));
    for (size_t j = 0; j < RightSideSize; j++) {
        TNumeric N;
        N.LoadFromFile(f);
        RightSide.push_back(TNumeric::create(std::move(N)));
    }

    Answer.clear();
    uint16_t AnswerSize;
    f.read((char*)&AnswerSize, sizeof(AnswerSize));
    for (size_t i = 0; i < AnswerSize; i++) {
        TNumeric N;
        N.LoadFromFile(f);
        Answer.push_back(TNumeric::create(std::move(N)));
    };

    TProblem::LoadFromFile(f);
}

void TSystemOfEquations::Assign(const TSystemOfEquations& E) {
    this->Variables = E.Variables;
    if (Conditions == 0)
        Conditions = std::make_shared<TNumeric>();
    *Conditions = *E.Conditions;
    this->Coefs = E.Coefs;
    this->RightSide = E.RightSide;
    this->role = E.role;
    this->Answer = E.Answer;
    this->EqCount = E.EqCount;
}

void TSystemOfEquations::operator=(const TSystemOfEquations& E) {
    Assign(E);
}

bool TSystemOfEquations::ReadData() {
    Coefs.clear();
    RightSide.clear();
    EqCount = role.size();
    for (size_t n = 0; n < EqCount; n++) {
        vector<PNumeric> Coefs1;
        Coefs1.assign(VarsCount(), nullptr);
        for (size_t m = 0; m < VarsCount(); m++)
            Coefs1[m] = Conditions->GetByRole(role[n][m], Conditions);
        Coefs.push_back(Coefs1);
        PNumeric N = Conditions->GetByRole(role[n][VarsCount()], Conditions);
        RightSide.push_back(N);
    }
    return true;
}

// Вместо переменной Variable в выражении Equation подставляет выражение Expression
TNumeric NSubstitute(const TNumeric& Equation, const TNumeric& Variable, const TNumeric& Expression) {
    if (Equation.IsEqual(Variable)) {
        return Expression;
    } else {
        TNumeric Res = Equation;
        for (size_t i = 0; i < Res.operands.size(); i++)
            *Res.operands[i] = NSubstitute(*Res.operands[i], Variable, Expression);
        return Res;
    }
}

TNumeric Designate(const TNumeric& Equation, const vector<TNumeric>& Vars, const vector<string> Names) {
    TNumeric Res = Equation;
    for (size_t i = 0; i < Vars.size(); i++)
        Res = NSubstitute(Res, Vars[i], TNumeric(Names[i]));
    return Res;
}

TNumeric Designate(const TNumeric& Equation, const vector<string> Names, const vector<TNumeric> Vars) {
    TNumeric Res = Equation;
    for (size_t i = 0; i < Vars.size(); i++)
        Res = NSubstitute(Res, TNumeric(Names[i]), Vars[i]);
    return Res;
}

// выражает переменную Var из выражения Equation
bool Express(TNumeric Equation, TNumeric Var, TNumeric* Res) {
    bool Ok;
    string ErrorStr;
    map<string, TNumeric> Map;
    size_t StartID = 0;
    Equation.EliminateUnimplementedFunctions(StartID, Map);
    Var.EliminateUnimplementedFunctions(StartID, Map);
    math_clear_all();
    string Cmd1 = Equation.CodeBasic();
    string Cmd2 = string("solve 1 for ") + Var.CodeBasic();
#ifdef __DEBUG__
    cout << Cmd1 << endl;
    cout << Cmd2 << endl;
#endif
    string str1 = math_process(Cmd1.c_str(), &Ok, &ErrorStr);
#ifdef __DEBUG__
    cout << "Ok = " << Ok << "; ErrorStr = " << ErrorStr << "; answer = " << str1 << endl;
#endif
    if (Ok == false)
        return false;
    string str2 = math_process(Cmd2.c_str(), &Ok, &ErrorStr);
#ifdef __DEBUG__
    cout << "Ok = " << Ok << "; ErrorStr = " << ErrorStr << "; answer = " << str2 << endl;
#endif
    if (Ok == false)
        return false;
    math_clear_all();
    // Если до сюда дошли, то в str2 должна быть строка вида x = y
    // Находим всё, что после знака =
    if (str2.find("=") == string::npos)
        return false;
    str2 = str2.substr(str2.find("=") + 1, string::npos);        
    Res->AssignV(str2.c_str());
    Res->RestoreUnimplementedFunctions(Map);
#ifdef __DEBUG__
    cout << str2 << endl;
    cout << Res->CodeBasic() << endl;
#endif
    return true;
}

// Вместо переменной VariableName в выражении Equation подставляет выражение Expression
TNumeric NSubstitute(const TNumeric& Equation, const string& VariableName, const TNumeric& Expression) {
    if (Equation.operation == OperatorConst) {
        if (Equation.strval == VariableName) {
            return Expression;
        } else
            return Equation;
    } else {
        TNumeric Res = Equation;
        for (size_t i = 0; i < Res.operands.size(); i++)
            *Res.operands[i] = NSubstitute(*Res.operands[i], VariableName, Expression);
        return Res;
    }
}

void TSystemOfEquations::Substitute(size_t VariableNumber, const TNumeric& Expression) {
    for (size_t n = 0; n < EqCount; n++) {
        TNumeric NewEquation = NSubstitute(*Conditions->operands[n], *Variables[VariableNumber], Expression);
#ifdef __DEBUG__
        cout << NewEquation.CodeBasic() << endl;
#endif
        NewEquation = *NewEquation.Simplify();
        // теперь осталось найти все коэффициенты при оставшихся неизвестных
        if (NewEquation.operation != OperatorEqual)
            throw "TSystemOfEquations::Substitute: Unexpected result. NewEquation.operation != OperatorEqual";

#ifdef __DEBUG__
        cout << NewEquation.CodeBasic() << endl;
#endif

        std::shared_ptr<TNumeric> LeftSide = NewEquation.operands[0];
        std::shared_ptr<TNumeric> RightSide = NewEquation.operands[1];
        // вычисляем свободный член в левой части
        std::shared_ptr<TNumeric> LeftSideFreeTerm = std::make_shared<TNumeric>(*LeftSide);  /// \todo: deep copy instad of make_shared
        for (size_t m1 = 0; m1 < VarsCount(); m1++)
            *LeftSideFreeTerm = NSubstitute(*LeftSideFreeTerm, *Variables[m1], TNumeric(0));

        // переносим свободный член в правую часть
        *LeftSide = *LeftSide - *LeftSideFreeTerm;
        *LeftSide = *LeftSide->Simplify();
        *RightSide = *RightSide - *LeftSideFreeTerm;
        *RightSide = *RightSide->Simplify();
        RightSide->role = role[n][VarsCount()];
#ifdef __DEBUG__
        cout << LeftSide.CodeBasic() << endl;
        cout << RightSide.CodeBasic() << endl;
#endif

        vector<PNumeric> NewCoefs;
        NewCoefs.assign(VarsCount() - 1, nullptr);
        TNumeric NewLeftSide;
        size_t m3 = 0;  // количество уже добавленных слагаемых в NewLeftSide
        for (size_t m = 0; m < VarsCount(); m++) {
            // ищем коэффициент при переменной номер m
            // коэфициент получится, если в левую часть вместо всех переменных кроме её подставить 0, а в переменную номер m -
            // единицу
            std::shared_ptr<TNumeric> Coef = std::make_shared<TNumeric>(*LeftSide);
            for (size_t m1 = 0; m1 < VarsCount(); m1++)
                if (m == m1)
                    *Coef = NSubstitute(*Coef, *Variables[m1], TNumeric(1));
                else
                    *Coef = NSubstitute(*Coef, *Variables[m1], TNumeric(0));
            *Coef = *Coef->Simplify();
            Coef->role = role[n][m];
            if (m != VariableNumber) { // todo: continue if m == VariableNumber
                // добавляем новый член в левую часть
                if (m3 == 0) {
                    NewLeftSide = *Coef * (*Variables[m]);
                } else {
                    NewLeftSide = NewLeftSide + *Coef * (*Variables[m]);
                }
                m3++;
            };
#ifdef __DEBUG__
            cout << Coef.CodeBasic() << endl;
#endif
            // обновляем коэффициент при очередном члене
            if (m < VariableNumber) {
                NewCoefs[m] = Coef;
            };
            if (m > VariableNumber) {
                NewCoefs[m - 1] = Coef;
            }
        }
        *Conditions->operands[n] = MakeEquality(NewLeftSide, *RightSide);
        Coefs[n] = NewCoefs;
        role[n].erase(role[n].begin() + VariableNumber);
    }

    // исключаем переменную VariableNumber
    Variables.erase(Variables.begin() + VariableNumber);
}

void TSystemOfEquations::ExcludeSimilarEquations() {
    for (size_t i = 0; i < Conditions->operands.size(); i++) {
        for (size_t j = i + 1; j < Conditions->operands.size();)
            if (Conditions->operands[i] == Conditions->operands[j])
                RemoveEquation(j);
            else
                j++;
    };
}

void TSystemOfEquations::RemoveEquation(size_t EqNumber) {
    role.erase(role.begin() + EqNumber);
    Conditions->operands.erase(Conditions->operands.begin() + EqNumber);
    EqCount--;
}

bool TSystemOfEquations::CheckAlwaysTrue(int EqNumber) {
    for (size_t m = 0; m < VarsCount(); m++)
        if (*Coefs[EqNumber][m] != TNumeric(0))
            return false;
    if (*RightSide[EqNumber] != TNumeric(0))
        return false;
    return true;
}

bool TSystemOfEquations::CheckAlwaysFalse(int EqNumber) {
    bool LeftZero = true;
    bool RightZero = true;
    for (size_t m = 0; m < VarsCount(); m++)
        if (*Coefs[EqNumber][m] != TNumeric(0)) {
            LeftZero = false;
            break;
        }
    if (*RightSide[EqNumber] != TNumeric(0))
        RightZero = false;
    if (!LeftZero)
        // в левой части не все коэффициенты равны нулю
        return false;
    return !RightZero;
}

bool TSystemOfEquations::GetSystemSolution(std::shared_ptr<THTMLWriter> Writer) {
    ExcludeSimilarEquations();
    if (ReadData() == false)
        if (Writer)
            Writer->WriteError("Can not read coefficients");

    for (size_t n = 0; n < EqCount; n++) {
        if (CheckAlwaysFalse(n)) {
            if (Writer)
                Writer->addParagraph("Equation above is always false.", static_cast<int>(n) + 1);
            SolutionExists = false;
            return true;
        }
    }

    bool PrintedForExcluding = false;
    vector<int> EquationsToExclude;
    for (size_t n = 0; n < EqCount; n++) {
        if (CheckAlwaysTrue(n)) {
            if (!PrintedForExcluding) {
                PrintedForExcluding = true;
                // if(Writer)Writer->WriteRectangleElement(Conditions); // todo: why Conditions is rectangle element?
            }
            if (Writer)
                Writer->addParagraph("Equation above is always true", static_cast<int>(n) + 1);
            EquationsToExclude.push_back(n);
        };
    }
    for (size_t i = 0; i < EquationsToExclude.size(); i++) {
        size_t CurrentNumber = EquationsToExclude[i] - i;  // необходимо сделать сдвиг, так как при удалении в i-ый раз уже i-1 уравнений удалено и все номера сместились
        RemoveEquation(CurrentNumber);
    }

    if (EqCount == 0) {
        // больше не осталось уравнений для решения.
        Answer.assign(VarsCount(), nullptr);
        TNumeric C;
        C = MakeSubscript(TNumeric("C"), TNumeric(1));
        for (size_t i = 0; i < VarsCount(); i++) {
            *C.operands[1] = TNumeric(static_cast<int>(i) + 1);
            Answer[i] = TNumeric::create(std::move(C));
            if (Writer)
                Writer->AddFormula(MakeEquality(TNumeric(*Variables[i]), *Answer[i]));
        };
        SolutionExists = true;
        return true;
    };
    if (EqCount > 0) {
        TNumeric ExcludedVar;
        bool Excluded = false;
        for (size_t i = 0; i < Variables.size(); i++) {
            // выражаем переменную номер i из первого уравнения
            if (Express(*Conditions->operands[0], *Variables[i], &ExcludedVar)) {
                // успешно выразили
                // исключаем первое уравнение из системы и выраженную переменную
                TSystemOfEquations STemp = *this;
                STemp.RemoveEquation(0);
                STemp.Substitute(i, ExcludedVar);
                // сначала проверяем, приведёт ли выбранный путь к решению всей системы
                vector<TRectangleElement*> Lines2;

                TSystemOfEquations S = STemp;
                if (STemp.GetSystemSolution(0)) {
                    // да, приведёт; можно теперь выписывать решение
                    if (Writer)
                        if (S.EqCount > 0) {
                            Writer->BeginParagraph();
                            Writer->Add("From first equation of the system one can obtain");
                            Writer->AddFormula(MakeEquality(*Variables[i], ExcludedVar));
                            Writer->Add("Substituting to system, we obtain");
                            Writer->AddFormula(*S.Conditions);
                            Writer->EndParagraph();
                        }

                    // AddLine(Lines, Ntabs, new TLine(new TText((*Phrases)[PhFrom]),  new TNumeric(Conditions->operands[0]), new
                    // TText((*Phrases)[PhExpressing]),  new TNumeric(Variables[i]))); AddLine(Lines, Ntabs, new TLine(new
                    // TNumeric(MakeEquality(Variables[i], ExcludedVar))));
                    S.GetSystemSolution(Writer);

                    /*                    if(S.EqCount > 0)
                                        {
                                            if(Writer)
                                            {
                                                Writer->BeginParagraph();
                                                Writer->Add("From first equation of the system one can obtain");
                                                Writer->AddInlineFormula(MakeEquality(Variables[i], ExcludedVar));
                                                Writer->Add("Substituting to system, we obtain");
                                                Writer->NewLine();
                                                Writer->AddInlineFormula(*S.Conditions);
                                                Writer->EndParagraph();
                                            }
                                        } else {
                                            //S.GetSystemSolution(&Lines2, Ntabs+1);
                                            SolutionExists = true;
                                            Answer.clear();
                                            Answer.push_back(ExcludedVar);
                                        }*/

                    SolutionExists = S.SolutionExists;
                    if (SolutionExists) {
                        // копируем решение из только что решённой подсистемы
                        Answer = S.Answer;
                        // добавляем в него исключённую переменную
                        TNumeric Value = ExcludedVar;
                        for (size_t j = 0; j < S.Variables.size(); j++)
                            Value = NSubstitute(Value, *S.Variables[j], *Answer[j]);
                        PNumeric ValueSimplified = Value.Simplify();
                        // AddLine(Lines, Ntabs, new TNumeric(MakeEquality(TNumeric(Variables[i]), Value)));
                        if (*ValueSimplified != Value) {
                            if (Writer) {
                                Writer->BeginParagraph();
                                Writer->Add("After simplifying ");
                                Writer->AddFormula(MakeEquality(TNumeric(*Variables[i]), *ValueSimplified));
                                Writer->EndParagraph();
                            };
                        }
                        Answer.insert(Answer.begin() + i, ValueSimplified);
                    }

                    Excluded = true;
                    break;
                } else {
                    for (size_t i = 0; i < Lines2.size(); i++)
                        delete Lines2[i];
                }

            } else {
                // не удалось выразить
            }
        };
        if (Excluded == false) {
            if (Writer)
                Writer->WriteError("Can not understand what can be done with first equation.");
            return false;
        } else
            return true;
    } else {
        // EqCount = 1
        if (VarsCount() == 1)
            return true;
        else
            return false;
    }
    return true;
}

bool TSystemOfEquations::GetSolution(std::shared_ptr<THTMLWriter> Writer) {
    if (GetSystemSolution(Writer)) {
        if (Writer) {
            if (SolutionExists) {
                Writer->BeginParagraph();
                Writer->Add("Final solution of the system:");
                for (size_t i = 0; i < VarsCount(); i++)
                    Writer->AddFormula(MakeEquality(*Variables[i], *Answer[i]));
                Writer->EndParagraph();
            } else {
                Writer->addParagraph("The system has no solution");
            }
        };
        return true;
    };
    return false;
}

void TSystemOfEquations::BeginAddEquations() {
    Conditions = std::make_shared<TNumeric>();
    Conditions->operation = OperatorEqSystem;
    MaxID = 0;
    role.clear();
    Coefs.clear();
    RightSide.clear();
    Answer.clear();
}

void TSystemOfEquations::EndAddEquations() {}

bool TSystemOfEquations::AddEquation(const TNumeric& N) {
    if (N.operation != OperatorEqual)
        return false;
    TNumeric Eq = *N.operands[0] - *N.operands[1];  // все переносим в левую часть
    vector<TNumeric> Coefs;
    vector<int> IDs;
    TNumeric LeftPart;
    LeftPart.operation = OperatorSum;
    Coefs.assign(Variables.size() + 1, TNumeric(0));
    IDs.assign(Variables.size() + 1, 0);
    // инициализируем коэфициент перед свободным членом
    TNumeric FreeTerm = Eq;
    for (size_t i = 0; i < Variables.size(); i++)
        FreeTerm = FreeTerm.Substitute(Variables[i]->strval, TNumeric(0));
    FreeTerm = *FreeTerm.Simplify();
    // инициализируем коэфициенты перед неизвестными
    for (size_t i = 0; i < Variables.size(); i++) {
        if (Variables[i]->operation != OperatorConst)
            throw "TSystemOfEquations::AddEquation: All Variables[i].operation must be OperatorConst";
        TNumeric Eq1 = Eq;
        Eq1 = Eq1.Substitute(Variables[i]->strval, TNumeric(1));
        for (size_t j = 0; j < Variables.size(); j++) {
            if (j != i)
                Eq1 = Eq1.Substitute(Variables[j]->strval, TNumeric(0));
        };
        Eq1 = Eq1 - FreeTerm;
        Eq1 = *Eq1.Simplify();
        Eq1.role = MaxID++;
        Coefs[i] = Eq1;
        IDs[i] = Eq1.role;
        LeftPart.OperandsPushback(Eq1 * (*Variables[i]));
    };
    FreeTerm.role = MaxID++;
    Coefs[Variables.size()] = -FreeTerm;  // правая часть со знаком "минус"
    Coefs[Variables.size()].role = FreeTerm.role;
    IDs[Variables.size()] = FreeTerm.role;
    role.push_back(IDs);

    Conditions->OperandsPushback(MakeEquality(LeftPart, Coefs[Variables.size()]));
    return true;
}

void TSystemOfEquations::Randomize(std::mt19937& rng) {
    std::uniform_int_distribution<int> dist(-10, 10);
    EqCount = role.size();
    for (size_t n = 0; n < EqCount; n++) {
        for (size_t m = 0; m <= VarsCount(); m++) {
            int CurID = role[n][m];
            PNumeric N = Conditions->GetByRole(CurID, Conditions);
            *N = TNumeric(dist(rng));
            N->role = CurID;
            //N->SetEditableFlags(ConstAllowed);
        };
    }
}
