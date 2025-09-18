#include <sstream>
#include <math.h>
#include "ttrigonometryproblems.h"
#include "TPolynomProblems.h"

TLinearTrigEquality::TLinearTrigEquality() : TProblem(), TEquality()
{
    Init(TNumeric("x"), TNumeric("n"));
    CanRandomize = true;
    BuildPhrases();
}

TLinearTrigEquality::TLinearTrigEquality(const TNumeric& UnknownVar, const TNumeric& CounterVar): TProblem(), TEquality()
{
    Init(UnknownVar, CounterVar);
    CanRandomize = true;
    BuildPhrases();
}

void TLinearTrigEquality::BuildPhrases()
{
    if(MyTranslator.CheckDictionary("TLinearTrigEquality"))return;
    MyTranslator.AddDictionary("TLinearTrigEquality");
    MyTranslator.AddEng("Can not find one or more coefficients.");
    MyTranslator.AddRus("Не могу найти коэффициенты");
    MyTranslator.AddEng("Equality is always true, so any number is solution.");
    MyTranslator.AddRus("Уравнение всегда выполняется, следовательно решением - вся числовая ось");
    MyTranslator.AddEng("Equality is always false so it has no solution.");
    MyTranslator.AddRus("Уравнение никогда не удовлетворяется, следовательно, решений нет");
    MyTranslator.AddEng("Solving: ");
    MyTranslator.AddRus("Решаем: ");
    MyTranslator.AddEng("Dividing left and right parts by coefficient at sine");
    MyTranslator.AddRus("Делим левую и правую части на коэффициент при синусе");
    MyTranslator.AddEng("Sine should be not greater than 1 in absolute value, no solution");
    MyTranslator.AddRus("Так как синус по модулю не должен превосходить 1, решений нет");
    MyTranslator.AddEng("After simplifying: ");
    MyTranslator.AddRus("После упрощения");
    MyTranslator.AddEng("Solving");
    MyTranslator.AddRus("Решаем");
    MyTranslator.AddEng("Dividing left and right parts by coefficient at sine");
    MyTranslator.AddRus("Делим левую и правую части на коэффициент при синусе");
    MyTranslator.AddEng("Cosine should be not greater than 1 in absolute value, no solution");
    MyTranslator.AddRus("Так как косинус по модулю не должен превосходить единицу, решений нет");
    MyTranslator.AddEng("After simplifying: ");
    MyTranslator.AddRus("После упрощений: ");
    MyTranslator.AddEng("Dividing left and right part by ");
    MyTranslator.AddRus("Делим левую и правую части на");
    MyTranslator.AddEng("After simplifying: %N");
    MyTranslator.AddRus("После упрощения: %N");
    MyTranslator.AddEng("Performing change of variables: %N");
    MyTranslator.AddRus("Выполняем замену переменных: %N");
    MyTranslator.AddEng("We should find at least one value for %n for which equality %N is satisfied.");
    MyTranslator.AddRus("Нужно найти хотя бы одно значение для %n, являющееся решением уравнения %N");
    MyTranslator.AddEng("Because of %n is greater than zero, the value of %n should be in interval %n");
    MyTranslator.AddRus("Так как %n больше нуля, то значение %n должно быть в интервале %n");
    MyTranslator.AddEng("Because of %n is less than zero, the value of %n should be in interval %N");
    MyTranslator.AddRus("Так как %n меньше нуля, то значение %n должно быть в интервале %n");
    MyTranslator.AddEng("Transforming to sine of sum: ");
    MyTranslator.AddRus("Преобразуем к синусу суммы");
    MyTranslator.AddEng("Designating %n");
    MyTranslator.AddRus("Обозначаем %n");
    MyTranslator.AddEng("Exactly one solution was expected.");
    MyTranslator.AddRus("Ожидалось одно единственное решение");
    MyTranslator.AddEng("General solution: %N ");
    MyTranslator.AddRus("Общее решение: %N");
    MyTranslator.AddEng("After simplifying: %N");
    MyTranslator.AddRus("После упрощений: %N");
    MyTranslator.AddEng("Solve linear trigonometric equality");
    MyTranslator.AddRus("Решить линейное тригонометрическое уравнение");
    MyTranslator.AddEng("linear");
    MyTranslator.AddRus("линейное");
    MyTranslator.AddEng("Dividing equation by %n");
    MyTranslator.AddRus("Делим уравнение на %n");
    MyTranslator.AddEng("Now we obtain the equation %N");
    MyTranslator.AddRus("Получаем уравнение %N");
    MyTranslator.AddEng("General solution is %N");
    MyTranslator.AddRus("Общее решение: %N");
    MyTranslator.AddEng("After simplifying %N where %n is integer");
    MyTranslator.AddRus("После упрощения получаем %N где %n - целое");
}

vector<string> TLinearTrigEquality::GetKeyWords()
{
    if(MyTranslator.CheckDictionary(GetClassName()+"Keywords") == false)
    {
        MyTranslator.AddDictionary(GetClassName()+"Keywords");
        MyTranslator.AddEng("equality");
        MyTranslator.AddRus("уравнение");
        MyTranslator.AddEng("trigonometric");
        MyTranslator.AddRus("тригонометрическое");
        MyTranslator.AddEng("linear");
        MyTranslator.AddRus("линейное");
    }
vector<string> Res;
    Res.push_back(MyTranslator.tr("equality"));
    Res.push_back(MyTranslator.tr("trigonometric"));
    Res.push_back(MyTranslator.tr("linear"));

    return Res;
}


void TLinearTrigEquality::Init(const TNumeric& UnknownVar, const TNumeric& CounterVar)
{
TNumeric a(1);
TNumeric b(1);
TNumeric c(0);
    a.ID = IDa; b.ID = IDb; c.ID = IDc;
    a.SetEditableFlags(ConstAllowed);
    b.SetEditableFlags(ConstAllowed);
    c.SetEditableFlags(ConstAllowed);
    Conditions = std::make_shared<TNumeric>(MakeEquality(a*MakeCos(UnknownVar)+b*MakeSin(UnknownVar), c));
    this->UnknownVar = UnknownVar;
    this->CounterVar = CounterVar;
}

TLinearTrigEquality::~TLinearTrigEquality()
{
}

void TLinearTrigEquality::SetCoef(const TNumeric& a, const TNumeric& b, const TNumeric& c)
{
    TNumeric& A = Conditions->GetByID(IDa).value();
    A = a; A.ClearID(); A.ID = IDa; A.EditableFlags = ConstAllowed;
    TNumeric& B = Conditions->GetByID(IDb).value();
    B = b; B.ClearID(); B.ID = IDb; B.EditableFlags = ConstAllowed;
    TNumeric& C = Conditions->GetByID(IDc).value();
    C = c; C.ClearID(); C.ID = IDc; C.EditableFlags = ConstAllowed;
}


string TLinearTrigEquality::GetTask()
{
    return MyTranslator.tr("Solve linear trigonometric equality");
}

string TLinearTrigEquality::GetShortTask()
{
    return MyTranslator.tr("linear");
}

bool TLinearTrigEquality::GetSolution(std::shared_ptr<THTMLWriter> Writer)
{
    if(Conditions == 0) return false;
TNumeric a = *(Conditions->GetByID(IDa));
TNumeric b = *(Conditions->GetByID(IDb));
TNumeric c = *(Conditions->GetByID(IDc));
    TrigRoots.clear();
    Result.Intervals.clear();
    if(!a.CanCalculate() || !b.CanCalculate() || !c.CanCalculate())
    {
        if(Writer)Writer->WriteError("Can not find one or more coefficients.");
        return false;
    }
    if(a == 0 && b == 0)
    //0 cosx + 0 sinx = c
    {
        if(c == 0)
        {
            if(Writer)Writer->AddParagraph("Equality is always true, so any number is solution.");
            Result.Intervals.push_back(IntervalAllRealNumbers);
        } else {
            if(Writer)Writer->AddParagraph("Equality is always false so it has no solution.");
            Result.Intervals.push_back(EmptySet);
        }
        if(Writer)Writer->AddFormula(MakeBelongsTo(UnknownVar, Result.GetNumeric()));
        return true;
    };
    if(a == 0)
    // b sinx = c
    {
        if(Writer)
        {
            Writer->BeginParagraph();
            Writer->Add("Solving: ");
            Writer->AddFormula(MakeEquality(b*MakeSin(UnknownVar), c));
            Writer->EndParagraph();
        }

        TNumeric cb = (c/b).Simplify();
        if(b.Calculate() != 1)
        {
            if(Writer)Writer->AddParagraph("Dividing left and right parts by coefficient at sine");
            if(Writer)Writer->AddFormula(MakeEquality(MakeSin(UnknownVar), cb));
        };
        double cbval = cb.Calculate();
        if(fabs(cbval) > 1)
        {
            if(Writer)Writer->AddParagraph("Sine should be not greater than 1 in absolute value, no solution");
            Result.Intervals.push_back(EmptySet);
        } else {
            TNumeric X;
            TNumeric N;
            N = TNumeric(MakeBelongsTo(CounterVar, TNumeric("Z"))); //во всех случаях n - целое
            if(cbval == 1)
            {
                X = NumPi2 + TNumeric(2)*NumPi*CounterVar;
            } else
            if(cbval == -1)
            {
                X = -NumPi2 + TNumeric(2)*NumPi*CounterVar;
            } else
            if(cbval == 0)
            {
                X = NumPi*CounterVar;
            } else
            {
                X = MakeProd(MakePow(TNumeric(-1), CounterVar), MakeArcsin(cb)) + NumPi*CounterVar;
            }
            TrigRoots.push_back(X);

            if(Writer)Writer->AddParagraph("After simplifying: ");
            X = X.Simplify().Unfactor();
            if(Writer)Writer->AddFormula(MakeEquality(UnknownVar, X));

            if(Writer)Writer->AddFormula(N);
        };
//        Result.Intervals.push_back(); //todo: непонятно, что делать в этом случае, когда вместо интервала бесконечная серия интервалов с параметром n
    } else
    if(b == 0)
    //a cos x = c
    {
        if(Writer)
        {
            Writer->BeginParagraph();
            Writer->Add("Solving");
            Writer->AddFormula(MakeEquality(a*MakeCos(UnknownVar), c));
            Writer->EndParagraph();
        }

        TNumeric ca = (c/a).Simplify();
        if(a.Calculate() != 1)
        {
            if(Writer)Writer->Add("Dividing left and right parts by coefficient at sine");
            if(Writer)Writer->AddFormula(MakeEquality(MakeCos(UnknownVar), ca));
        };
        double caval = ca.Calculate();
        if(fabs(caval) > 1)
        {
            if(Writer)Writer->AddParagraph("Cosine should be not greater than 1 in absolute value, no solution");
            Result.Intervals.push_back(EmptySet);
        } else {
            TNumeric X;
            TNumeric N;
            N = TNumeric(MakeBelongsTo(CounterVar, TNumeric("Z"))); //во всех случаях n - целое
            if(caval == 1)
            {
                X = TNumeric(2)*TNumeric("\\pi")*CounterVar;
            } else
            if(caval == -1)
            {
                X = TNumeric("\\pi") + TNumeric(2)*TNumeric("\\pi")*CounterVar;
            } else
            if(caval == 0)
            {
                X = MakeFrac(TNumeric("\\pi"), TNumeric(2)) + TNumeric("\\pi")*CounterVar;
            } else
            {
                X = MakePlusMinus(MakeArccos(ca)) + TNumeric(2)*TNumeric("\\pi")*CounterVar;
            }
            TrigRoots.push_back(X);
            if(Writer)Writer->AddFormula(MakeEquality(UnknownVar, X));

            if(Writer)Writer->AddParagraph("After simplifying: ");
            X = X.Simplify().Unfactor();
            if(Writer)Writer->AddFormula(MakeEquality(UnknownVar, X));

            if(Writer)Writer->AddFormula(N); //"n принадлежит Z"
        };
    } else {
    //a cos x + b sin x = c
        if(c == 0)
        {
            //a cos x + b sin x = 0
            //a != 0 (случай a == 0 разобран выше)
            if(Writer)Writer->AddParagraph("Dividing equation by %n", MakeCos(UnknownVar));
            if(Writer)Writer->AddFormula(MakeEquality(a+b*MakeTg(UnknownVar), TNumeric("0")));
            //a + b tg x = 0
            //tg x = -a/b
            TNumeric AB = -a/b;
            AB = AB.Simplify();
            if(Writer)Writer->AddParagraph("Now we obtain the equation %N", MakeEquality(MakeTg(UnknownVar), AB));

            CounterVar = TNumeric("k");
            TNumeric X = MakeArctg(AB) + NumPi*CounterVar;
            if(Writer)
            {
                Writer->AddParagraph("General solution is %N", MakeEquality(UnknownVar, X));
                X = X.Simplify();
                Writer->AddParagraph("After simplifying %N where %n is integer", MakeEquality(UnknownVar, X), CounterVar);
            }
            TrigRoots.push_back(X);
        } else {
            //a cos x + b sin x = c
            TNumeric Psi("\\psi");
            TNumeric D = MakeSqrt((a^TNumeric(2))+(b^TNumeric(2)));
            if(Writer)
            {
                Writer->BeginParagraph();
                Writer->Add("Dividing left and right part by ");
                Writer->AddFormula(D);
                Writer->EndParagraph();
            };
            a = (a / D);
            b = (b / D);
            c = (c / D);
            if(Writer)Writer->AddFormula(MakeEquality(a*MakeCos(UnknownVar)+b*MakeSin(UnknownVar), c));
            a = a.Simplify();
            b = b.Simplify();
            c = c.Simplify();
            if(Writer)
            {
                Writer->AddParagraph("After simplifying: %N", MakeEquality(a*MakeCos(UnknownVar)+b*MakeSin(UnknownVar), c));

                Writer->AddParagraph("Performing change of variables: %N", MakeSystemOfEquations(MakeEquality(MakeSin(Psi), a), MakeEquality(MakeCos(Psi), b)));

               Writer->AddParagraph("We should find at least one value for %n for which equality %N is satisfied.",
               TNumeric("\\psi"), MakeEquality(MakeSin(Psi)*MakeCos(UnknownVar)+MakeCos(Psi)*MakeSin(UnknownVar), c));
           };
           TNumeric PhiVal;
           if(b.Calculate() > 0)
           {
               if(Writer)
               {
                   Writer->AddParagraph("Because of %n is greater than zero, the value of %n should be in interval %n",
                   MakeCos(TNumeric("\\psi")),
                   TNumeric("\\psi"),
                   MakeInterval(-NumPi2, NumPi2));
               }
               PhiVal = MakeArcsin(a);
               PhiVal = PhiVal.Simplify();
           } else {
               TIntervalsSet I;
               I.Intervals.push_back(MakeInterval(NumPi2, NumPi));
               I.Intervals.push_back(MakeInterval(-NumPi, -NumPi2));
               if(Writer)
                   Writer->AddParagraph("Because of %n is less than zero, the value of %n should be in interval %N", MakeCos(TNumeric("\\psi")),
                               TNumeric("\\psi"), I.GetNumeric());
               PhiVal = MakeArcsin(a);
               PhiVal = PhiVal.Simplify();
               if(a.Calculate()>0) PhiVal = PhiVal - NumPi;
               else PhiVal = PhiVal + NumPi;
           }
           if(Writer)Writer->AddFormula(MakeEquality(Psi, PhiVal));
           if(Writer)
           {
               Writer->BeginParagraph();
               Writer->Add("Transforming to sine of sum: ");
               Writer->AddFormula(MakeEquality(MakeSin(UnknownVar + Psi), c));
               Writer->EndParagraph();
           };
               //sin((*Phrases)[Phi+x) = cos((*Phrases)[Phi)*sin(x) + sin((*Phrases)[Phi)*cos(x)
           TLinearTrigEquality E(TNumeric("t"), TNumeric("n"));
           E.SetCoef(TNumeric("0"), TNumeric("1"), c); //0 cos x + 1 sin x = c
           if(Writer)
               Writer->AddParagraph("Designating %n", MakeEquality(TNumeric("t"), UnknownVar+Psi));
            if(E.GetSolution(Writer))
                //в этом случае всегда решение будет и оно будет единственным
            {
                if(E.TrigRoots.size() != 1)
                {
                    if(Writer)Writer->WriteError("Exactly one solution was expected.");
                    return false;
                } else {
                    TNumeric X = E.TrigRoots[0] - PhiVal;

                    X = X.Simplify().Unfactor();
                    if(Writer)
                    {
                        Writer->AddParagraph("General solution: %N ", MakeEquality(UnknownVar, X));
                        Writer->AddParagraph("After simplifying: %N",  MakeEquality(UnknownVar, X));
                    };


                    TrigRoots.push_back(X);
                }
            } else {
                return false;
            }
        };
    }

    return true;
}

void TLinearTrigEquality::Randomize(std::mt19937& rng)
{
    std::uniform_int_distribution<int> dist(-10, 10);
    SetCoef(TNumeric(dist(rng)), TNumeric(dist(rng)), TNumeric(dist(rng)));
}

//**************************************************************************************************
THomogeneousTrigEquality::THomogeneousTrigEquality(size_t MaxPower): TPolynomConditions(), TEquality()
{
    this->UnknownVar = TNumeric("\\phi");
    this->CounterVar = TNumeric("n");
    SetMaxPower(MaxPower, OperatorEqual);
    CanRandomize = true;
    BuildPhrases();
}

THomogeneousTrigEquality::THomogeneousTrigEquality(size_t MaxPower, string UnknownVar, string CounterVar) : TPolynomConditions(), TEquality()
{
    this->UnknownVar = TNumeric(UnknownVar);
    this->CounterVar = TNumeric(CounterVar);
    SetMaxPower(MaxPower, OperatorEqual);
    CanRandomize = true;
    BuildPhrases();
}

void THomogeneousTrigEquality::BuildPhrases()
{
    if(MyTranslator.CheckDictionary("THomogeneousTrigEquality"))return;
    MyTranslator.AddDictionary("THomogeneousTrigEquality");
    MyTranslator.AddEng("Degenerated case: 0 = 0, consequently %N");
    MyTranslator.AddRus("Вырожденный случай: 0 = 0. Следовательно %N");
    MyTranslator.AddEng("Checking partial solution: %N");
    MyTranslator.AddRus("Проверяем частное решение: %N");
    MyTranslator.AddEng("Substituting %n one can obtain 0=0. Consequently, %N is a solution");
    MyTranslator.AddRus("Подставляя %n получаем 0 = 0. Следовательно, %N является решением.");
    MyTranslator.AddEng("Factoring out common multiplier ");
    MyTranslator.AddRus("Выносим общий множитель.");
    MyTranslator.AddEng("One can obtain: %N");
    MyTranslator.AddRus("Получаем: %N");
    MyTranslator.AddEng("%n is not solution. Dividing left and right sides by %n");
    MyTranslator.AddRus("%n не является решением. Делим левую и правую части на %n");
    MyTranslator.AddEng("The solution of %N is %N");
    MyTranslator.AddRus("Решением %N является %N");
    MyTranslator.AddEng("After simplifying %N");
    MyTranslator.AddRus("После упрощения %N");
    MyTranslator.AddEng("Common solution: ");
    MyTranslator.AddRus("Общее решение: ");
    MyTranslator.AddEng("Solve homogeneous trigonometric equation of %d power");
    MyTranslator.AddRus("Решить однородное тригонометрическое уравнение %d степени");
    MyTranslator.AddEng("homogeneous of %d power");
    MyTranslator.AddRus("однородное %d степени");
}

THomogeneousTrigEquality::~THomogeneousTrigEquality()
{

}
string THomogeneousTrigEquality::GetTask()
{
    char Buf[128];
    sprintf(Buf, MyTranslator.tr("Solve homogeneous trigonometric equation of %d power").c_str(), MaxPower);
    return string(Buf);
}

string THomogeneousTrigEquality::GetShortTask()
{
    char Buf[128];
    sprintf(Buf, MyTranslator.tr("homogeneous of %d power").c_str(), MaxPower);
    return string(Buf);
}

TNumeric THomogeneousTrigEquality::GetVarPower(size_t power)
{
    //return TPolynomConditions::GetVarPower(power);
size_t sinpower = MaxPower - power;
    if(sinpower == 0) return MakeCos(UnknownVar)^TNumeric(power);
    else
    {
        if(power == 0) return MakeSin(UnknownVar)^TNumeric(sinpower);
        else return ((MakeSin(UnknownVar))^TNumeric(sinpower))*((MakeCos(UnknownVar))^TNumeric(power));
    };
}

bool THomogeneousTrigEquality::GetSolution(std::shared_ptr<THTMLWriter> Writer)
{
/*    AddLine(Lines, Ntabs, new TText("Решаем однородное тригонометрическое уравнение: "));
    AddLine(Lines, Ntabs, new TNumeric(*Conditions));*/
TPolynom P(this->GetCoef());

    Result.Intervals.clear();
    TrigRoots.clear();
    //вырожденный случай: 0 = 0
    if(P.IsZero())
    {
        Result.Intervals.push_back(IntervalAllRealNumbers);

        if(Writer)
            Writer->AddParagraph("Degenerated case: 0 = 0, consequently %N", MakeBelongsTo(UnknownVar, Result.GetNumeric()));
        return true;
    } else {
        size_t MaxFreeCosPower = 0; //сколько раз за скобки можно вынести Cos
        for(size_t i = 0; i < P.Coef.size()-1; i++)
        {
            TNumeric C = P.Coef[i];
            if(C == 0) MaxFreeCosPower++;
            else break;
        };
        size_t HasSin = false; //если присутствует sin не в нулевой степени
        for(size_t i = 0; i+1 < P.Coef.size(); i++)
        {
            TNumeric C = P.Coef[i];
            if(C != 0)
            {
                HasSin = true;
                break;
            }
        }
        if(HasSin)
        {
            if(Writer)
                Writer->AddParagraph("Checking partial solution: %N", MakeEquality(MakeCos(TNumeric("\\phi")), TNumeric("0")));
        };

        if(MaxFreeCosPower > 0)
        //можно вынести cos^MaxCosPower
        {
            if(HasSin)
            {
                if(Writer)
                    Writer->AddParagraph("Substituting %n one can obtain 0=0. Consequently, %N is a solution" ,
                            MakeEquality(MakeCos(TNumeric("\\phi")), TNumeric("0")), MakeEquality(MakeCos(TNumeric("\\phi")), TNumeric("0")));
            };

            TLinearTrigEquality CosX(UnknownVar, TNumeric("n")); //cos x = 0
            CosX.SetCoef(TNumeric("1"), TNumeric("0"), TNumeric("0"));
            //AddLine(Lines, Ntabs+1, new TLine(new TText("Решаем "), new TNumeric(*CosX.Conditions)));
            if(!CosX.GetSolution(Writer))return false;
            TrigRoots.insert(TrigRoots.begin(), CosX.TrigRoots.begin(), CosX.TrigRoots.end());
            Result.Intervals = CosX.Result.Intervals;

            if(Writer && HasSin)
            {
                Writer->BeginParagraph();
                Writer->Add("Factoring out common multiplier ");
                if(MaxFreeCosPower == 1)
                    Writer->AddFormula(MakeCos(UnknownVar));
                else
                    Writer->AddFormula(MakePow(MakeCos(UnknownVar), TNumeric(MaxFreeCosPower)));
                Writer->EndParagraph();
            }

            TPolynom P1 = P;
            P1.Coef.erase(P1.Coef.begin(), P1.Coef.begin()+MaxFreeCosPower);

            //После вынесения общего множителя cos^MaxCosPower
            if(P1.MajorPower() == 0)
            //частный случай a sin^0 x = 0
            {
                //ничего не делаем, так как a!=0 (мы уже проверяли, что уравнение не вырожденное)
/*                TLinearTrigEquality E(UnknownVar, TNumeric("n"));
                E.SetCoef(P1.GetCoef(1), P1.GetCoef(0), TNumeric(0));
                if(Writer)
                    Writer->AddParagraph("One can obtain: %N", *E.Conditions);
                if(Writer)Writer->IncrementNestingLevel();
                bool res = E.GetSolution(Writer);
                if(Writer)Writer->DecrementNestingLevel();
                if(res)
                {
                    TrigRoots.insert(TrigRoots.begin(), E.TrigRoots.begin(), E.TrigRoots.end());
                    Result.Intervals = E.Result.Intervals;
                }else {
                    return false;
                }*/
            } else {
                THomogeneousTrigEquality HE(MaxPower-MaxFreeCosPower);
                HE.SetLeftPartP(P1);
                if(Writer)
                    Writer->AddParagraph("One can obtain: %N", *HE.Conditions);
                if(Writer)Writer->IncrementNestingLevel();
                bool res = HE.GetSolution(Writer);
                if(Writer)Writer->DecrementNestingLevel();
                if(res)
                {
                    TrigRoots.insert(TrigRoots.begin(), HE.TrigRoots.begin(), HE.TrigRoots.end());
                    Result.Intervals = HE.Result.Intervals;
                } else {
                    return false;
                };
            }
        } else {
            if(Writer)
                Writer->AddParagraph("%n is not solution. Dividing left and right sides by %n", MakeEquality(MakeCos(TNumeric("\\phi")), TNumeric("0")),
                            MakeEquality(MakeCos(TNumeric("\\phi")), TNumeric("0")));

            //Переворачиваем P задом на перед в P1
            TPolynom P1(P);
            for(size_t i = 0; i <= P1.Coef.size()/2; i++)
            {
                TNumeric Temp = P1.Coef[i];
                P1.Coef[i] = P1.Coef[P1.Coef.size() - 1 - i];
                P1.Coef[P1.Coef.size() - 1 - i] = Temp;
            }
            TPolynomialEquality Tang(P1);
            Tang.SetUnknownVar(MakeTg(UnknownVar));
            //AddLine(Lines, Ntabs+1, new TLine(new TText("Решаем "), new TNumeric(*Tang.Conditions)));
            if(Writer)Writer->IncrementNestingLevel();
            bool res = Tang.GetSolution(Writer);
            if(Writer)Writer->DecrementNestingLevel();
            if(res)
            {
                for(size_t i = 0; i < Tang.Roots.size(); i++)
                {
                    TNumeric Tg = Tang.Roots[i];
                    TNumeric X = MakeArctg(Tg) + MakeProd(TNumeric("\\pi"), CounterVar);
                    X = X.Simplify();
                    TrigRoots.push_back(X);
                    Result.Intervals.push_back(TInterval(X));
                    if(Writer)
                    {
                        Writer->AddParagraph("The solution of %N is %N", MakeEquality(MakeTg(UnknownVar), Tg), MakeEquality(UnknownVar, X));
                        Writer->AddParagraph("After simplifying %N", MakeEquality(UnknownVar, X));
                    }
                };
            } else {
                return false;
            };
        }
    }
    TIntervalsSet I;
    if(Result.Intervals.size()>0)
    {
        I = Result.Intervals[0];
        for(size_t i = 1; i < Result.Intervals.size(); i++)
            I = I + Result.Intervals[i];
    };

    if(Writer)Writer->BeginParagraph();
    if(Writer)Writer->Add("Common solution: ");
    if(I.Intervals.size() == 1 && I.Intervals[0] == IntervalAllRealNumbers)
    {
        if(Writer)Writer->AddFormula(MakeBelongsTo(UnknownVar, IntervalAllRealNumbers.GetNumeric()));
    }
    else
    {
        if(TrigRoots.size() == 0)
        {
            if(Writer)Writer->AddFormula(MakeBelongsTo(UnknownVar, EmptySet));
        }
        else
        {
            TIntervalsSet I2;
            for(size_t i = 0; i < TrigRoots.size(); i++)
                I2 = I2 + TInterval(TrigRoots[i]);
            if(Writer)Writer->AddFormula(MakeBelongsTo(UnknownVar, I2.GetNumeric()));
        };
    };
    if(Writer)Writer->EndParagraph();
    return true;
}


vector<string> THomogeneousTrigEquality::GetKeyWords()
{
    if(MyTranslator.CheckDictionary(GetClassName()+"Keywords") == false)
    {
        MyTranslator.AddDictionary(GetClassName()+"Keywords");
        MyTranslator.AddEng("equality");
        MyTranslator.AddRus("уравнение");
        MyTranslator.AddEng("trigonometric");
        MyTranslator.AddRus("тригонометрическое");
        MyTranslator.AddEng("homogeneous");
        MyTranslator.AddRus("однородное");
    }
vector<string> Res;
    Res.push_back(MyTranslator.tr("equality"));
    Res.push_back(MyTranslator.tr("homogeneous"));
    Res.push_back(MyTranslator.tr("trigonometric"));
    return Res;
}

void THomogeneousTrigEquality::Randomize(std::mt19937& rng)
{
    TPolynomConditions::Randomize(rng);
}
