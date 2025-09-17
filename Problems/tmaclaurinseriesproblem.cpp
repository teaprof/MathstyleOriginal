#include<iostream>
#include<sstream>
#include "Base/polynom.h"
#include "tmaclaurinseriesproblem.h"

//#define __DEBUG__

TPolynomSeries::TPolynomSeries()
{
    RemainderPower = -1;
}

TPolynomSeries::TPolynomSeries(const TPolynomSeries& P)
{
    this->RemainderPower = P.RemainderPower;
    this->Coefs = P.Coefs;
}

TPolynomSeries::TPolynomSeries(const TNumeric& Coef0)
{
    this->Coefs.push_back(Coef0);
    this->RemainderPower = -1;
}

TPolynomSeries::TPolynomSeries(const vector<TNumeric>& Coefs, int LastTerm)
{
    this->Coefs = Coefs;
    this->RemainderPower = LastTerm;
}

TPolynomSeries TPolynomSeries::operator+(const TPolynomSeries& P)
{
TPolynomSeries Res;
TPolynom P1(this->Coefs);
TPolynom P2(P.Coefs);
    Res.Coefs = (P1+P2).Coef;

int RR = -1;
    if(RemainderPower != -1) RR = RemainderPower;
    if(P.RemainderPower != -1)
        if(P.RemainderPower < RR || RR == -1) RR = P.RemainderPower;
    Res.Cut(RR);
    return Res;
}

TPolynomSeries TPolynomSeries::operator+(const TNumeric& N)
{
    if(RemainderPower == 0) return *this;
    TPolynomSeries Res(*this);
    if(Res.Coefs.size() > 0) Res.Coefs[0] = Res.Coefs[0] + N;
    return Res;
}

TPolynomSeries TPolynomSeries::operator-(const TNumeric& N)
{
    if(RemainderPower == 0) return *this;
    TPolynomSeries Res(*this);
    if(Res.Coefs.size() > 0) Res.Coefs[0] = Res.Coefs[0] - N;
    return Res;
}

TPolynomSeries TPolynomSeries::operator-(const TPolynomSeries& P)
{
TPolynomSeries Res;
TPolynom P1(this->Coefs);
TPolynom P2(P.Coefs);
    Res.Coefs = (P1-P2).Coef;

int RR = -1;
    if(RemainderPower != -1) RR = RemainderPower;
    if(P.RemainderPower != -1)
       if(P.RemainderPower < RR || RR == -1) RR = P.RemainderPower;
    Res.Cut(RR);
    return Res;
}



TPolynomSeries TPolynomSeries::operator*(const TPolynomSeries& P)
{
    //Сначала честно перемножаем полиномы
TPolynom This(this->Coefs);
TPolynom New(P.Coefs);
TPolynom Res = This*New;
//Теперь определяем показатель степени x при eps: eps*x^k
int ResRemainderPower  = -1;
    if(RemainderPower != -1)
        for(size_t i = 0; i < P.Coefs.size(); i++)
        {
            if(P.Coefs[i] != TNumeric(0))
            {
                int RR = RemainderPower + i;
                if(RR < ResRemainderPower || ResRemainderPower == -1)
                {
                    ResRemainderPower = RR;
                    break;
                };
            };
        };

    if(P.RemainderPower != -1)
        for(size_t i = 0; i < Coefs.size(); i++)
            if(Coefs[i] != TNumeric(0))
            {
                int RR = P.RemainderPower + i;
                if(RR < ResRemainderPower || ResRemainderPower == -1)
                {
                    ResRemainderPower = RR;
                    break;
            };
        };
    return TPolynomSeries(Res.Coef, ResRemainderPower) ;
}

TPolynomSeries TPolynomSeries::operator*(const TNumeric& N)
{
TPolynom This(this->Coefs);
    return TPolynomSeries((This*N).Coef, this->RemainderPower);
}

TPolynomSeries TPolynomSeries::operator=(const TPolynomSeries& P)
{
    this->Coefs = P.Coefs;
    this->RemainderPower = P.RemainderPower;
    return *this;
}


void TPolynomSeries::Cut(int RemainderPower)
{
    if(RemainderPower > this->RemainderPower && this->RemainderPower != -1)
        return;
    if(RemainderPower == -1)
        return;
    this->RemainderPower = RemainderPower;
    if(Coefs.size() > (size_t) RemainderPower + 1) Coefs.erase(Coefs.begin() + RemainderPower + 1, Coefs.end());
}

TPolynomSeries TPolynomSeries::Substitute(const TPolynomSeries& P, int LastTerm)
{
TPolynomSeries Res;
    TPolynomSeries Power(TNumeric(1));
#ifdef __DEBUG__
    std::cout<<GetNumeric("x").CodeBasic()<<endl;
#endif
    for(size_t i = 0; i < Coefs.size(); i++)
    {
        TPolynomSeries Term(Power*Coefs[i]);
#ifdef __DEBUG__
        std::cout<<Term.GetNumeric("x").CodeBasic()<<endl;
#endif
        Res = Res + Term;
        Power = Power * P;
#ifdef __DEBUG__
        std::cout<<Power.GetNumeric("x").CodeBasic()<<endl;
        std::cout<<Res.GetNumeric("x").CodeBasic()<<endl;
#endif
    }

    if(RemainderPower != -1)
    {
        int PMinorPower = 0;
        for(size_t i = 0; i < P.Coefs.size(); i++)
            if(P.Coefs[i] != TNumeric(0))
            {
                PMinorPower = i;
                break;
            }
        int RR = PMinorPower+RemainderPower;
        Res.Cut(RR);
    };

    Res.Cut(LastTerm);
    return Res;
}

TNumeric TPolynomSeries::GetNumeric(const string &var)
{
TPolynom This(Coefs);
TNumeric Res = This.GetSeriesNumeric(TNumeric(var));
    if(RemainderPower != -1)
    {
        TNumeric Remainder;
        //Remainder = MakeProd(MakePow(TNumeric(var), TNumeric(RemainderPower)), TNumeric("\\eps"));
        Remainder = MakeBigO(MakePow(TNumeric(var), TNumeric(RemainderPower)));
        Res = Res + Remainder;
    };
    return Res;
}

//***********************************************************************************

void TMaclaurinSeriesProblem::Assign(const TMaclaurinSeriesProblem& S)
{
    if(Conditions) delete Conditions;
    Conditions = new TNumeric;
    *this->Conditions = *S.Conditions;
    MaxPower = S.MaxPower;
    /*if(Solution) delete Solution;
    Solution = 0;
    if(S.Solution)
    {
        Solution = new TLines;
        *Solution = *S.Solution;
    }
    Series = S.Series;*/

    BuildPhrases();
}


TMaclaurinSeriesProblem::TMaclaurinSeriesProblem(size_t MaxPower)
{
    Conditions = new TNumeric;
    Conditions->Operator = OperatorPow;
    Conditions->OperandsPushback(TNumeric("x") + TNumeric(1));
    Conditions->OperandsPushback(MakeFrac(TNumeric("1"), TNumeric("2")));
    Conditions->SetEditableFlags(ConstAllowed | FunctionsAllowed);
    this->MaxPower = MaxPower;
    BuildPhrases();
}

TMaclaurinSeriesProblem::~TMaclaurinSeriesProblem()
{

}
void TMaclaurinSeriesProblem::BuildPhrases()
{
    if(MyTranslator.CheckDictionary(GetClassName()))return;
    MyTranslator.AddDictionary(GetClassName());
    MyTranslator.AddEng("Transofming %N");
    MyTranslator.AddRus("Преобразуем %N");
    MyTranslator.AddEng("Can not find Maclaurin polynomial for %N, because argument is not in vicinity of zero.");
    MyTranslator.AddRus("Не могу разложить по формуле Маклорена %N, так как аргумент функции должен попадать окрестность нуля");
    MyTranslator.AddEng("The expression under ln is zero when %n = 0. Can not find Maclaurin polynomial.");
    MyTranslator.AddRus("Выражение под знаком ln равно нулю, когда %n = 0. Не могу разложить по формуле Маклорена.");
    MyTranslator.AddEng("Can not find Maclaurin polynomial for %N, because base of power function should be non-zero when %n in vicinity of zero");
    MyTranslator.AddRus("Не могу разложить по формуле Маклорена %N, так как основание степенной функции должно быть отличным от нуля когда %n в окрестности нуля");
    MyTranslator.AddEng("Can not find Maclaurin polynomial for %N, because base of power function should be positive if power is fractional.");
    MyTranslator.AddRus("Не могу разложить по формуле Маклорена %N, потому что основание степенной функции должно быть положительным, когда показатель дробный.");
    MyTranslator.AddEng("Answer: %N");
    MyTranslator.AddRus("Ответ: %N");
    MyTranslator.AddEng("Can not find Maclaurin polynomial.");
    MyTranslator.AddRus("Не могу разложить по формуле Маклорена");
    MyTranslator.AddEng("Find the Maclaurin polynomial of degree %d");
    MyTranslator.AddRus("Разложить по формуле Маклорена до %d степени включительно");
    MyTranslator.AddEng("Maclaurin polynomial of degree %d");
    MyTranslator.AddRus("Маклорена формула степени %d");
}

vector<string> TMaclaurinSeriesProblem::GetKeyWords()
{
    if(MyTranslator.CheckDictionary(GetClassName()+"Keywords") == false)
    {
        MyTranslator.AddDictionary(GetClassName()+"Keywords");
        MyTranslator.AddEng("Maclaurin");
        MyTranslator.AddRus("Маклорен");
        MyTranslator.AddEng("polynom");
        MyTranslator.AddRus("многочлен");
    }
vector<string> Res;
    Res.push_back(MyTranslator.tr("Maclaurin"));
    Res.push_back(MyTranslator.tr("polynom"));
    return Res;
}


TMaclaurinSeriesProblem::TMaclaurinSeriesProblem(const TMaclaurinSeriesProblem& D) : TProblem(D)
{
    this->Assign(D);
}

void TMaclaurinSeriesProblem::operator=(const TMaclaurinSeriesProblem& D)
{
    Assign(D);
};

string TMaclaurinSeriesProblem::GetTask()
{
    char Buf[128];
    sprintf(Buf, MyTranslator.tr("Find the Maclaurin polynomial of degree %d").c_str(), MaxPower);
    return string(Buf);
}

string TMaclaurinSeriesProblem::GetShortTask()
{
    char Buf[128];
    sprintf(Buf, MyTranslator.tr("Maclaurin polynomial of degree %d").c_str(), MaxPower);
    return string(Buf);
}

TPolynomSeries GetMaclaurin(int Operator, size_t LastTerm)
{
    TPolynomSeries Res(TNumeric(0));
    switch(Operator)
    {
        case OperatorSin:
        {
            Res.Coefs.assign(LastTerm + 1, TNumeric(0));
            for(size_t i = 1; i <= LastTerm; i+=2)
            {
                int factorial = 1;
                for(size_t j = 1; j <= i; j++)
                    factorial *= j;
                if(i % 4 == 3) factorial *= -1;
                Res.Coefs[i] = (TNumeric(1)/TNumeric(factorial)).Simplify();
            }
            Res.RemainderPower = LastTerm;
            break;
        };
        case OperatorCos:
        {
            Res.Coefs.assign(LastTerm + 1, TNumeric(0));
            for(size_t i = 0; i <= LastTerm; i+=2)
            {
                int factorial = 1;
                for(size_t j = 1; j <= i; j++)
                    factorial *= j;
                if(i % 4 == 2) factorial *= -1;
                Res.Coefs[i] = (TNumeric(1)/TNumeric(factorial)).Simplify();
            }
            Res.RemainderPower = LastTerm;
            break;
        };
        case OperatorSh:
        {
            Res.Coefs.assign(LastTerm + 1, TNumeric(0));
            for(size_t i = 1; i <= LastTerm; i+=2)
            {
                int factorial = 1;
                for(size_t j = 1; j <= i; j++)
                    factorial *= j;
                Res.Coefs[i] = (TNumeric(1)/TNumeric(factorial)).Simplify();
            }
            Res.RemainderPower = LastTerm;
            break;
        };
        case OperatorCh:
        {
            Res.Coefs.assign(LastTerm + 1, TNumeric(0));
            for(size_t i = 0; i <= LastTerm; i+=2)
            {
                int factorial = 1;
                for(size_t j = 1; j <= i; j++)
                    factorial *= j;
                Res.Coefs[i] = (TNumeric(1)/TNumeric(factorial)).Simplify();
            }
            Res.RemainderPower = LastTerm;
            break;
    };
        case OperatorExp:
        {
            Res.Coefs.assign(LastTerm + 1, TNumeric(0));
            for(size_t i = 0; i <= LastTerm; i++)
            {
                int factorial = 1;
                for(size_t j = 1; j <= i; j++)
                    factorial *= j;
                Res.Coefs[i] = (TNumeric(1)/TNumeric(factorial)).Simplify();
            }
            Res.RemainderPower = LastTerm;
            break;
        };
        break;
        case OperatorTg:
        case OperatorCtg:
        case OperatorArcsin:
        case OperatorArccos:
        case OperatorArctg:        
        {
            TNumeric TestFunc;
            TestFunc.Operator = Operator;
            TestFunc.OperandsPushback(TNumeric("x"));
            Res.Coefs.assign(LastTerm + 1, TNumeric(0));
            for(size_t i = 0; i <= LastTerm; i++)
            {
                int factorial = 1;
                for(size_t j = 1; j <= i; j++)
                    factorial *= j;
                TNumeric C = TestFunc.Substitute("x", TNumeric(0))/TNumeric(factorial);
                #ifdef __DEBUG__
                    std::cout<<C.CodeBasic()<<endl;
                #endif
                C = C.Simplify();
                #ifdef __DEBUG__
                    std::cout<<C.CodeBasic()<<endl;
                #endif
                Res.Coefs[i] = C;
                TestFunc = TestFunc.Derivative("x").Simplify();
                #ifdef __DEBUG__
                    std::cout<<TestFunc.CodeBasic()<<endl;
                #endif
            }
            Res.RemainderPower = LastTerm;
        }
        break;
        case OperatorLn:
        //выдает разложение ln(1+x)
        {
            Res.Coefs.assign(LastTerm + 1, TNumeric(0));
            for(size_t i = 1; i <= LastTerm; i++)
            {
                TNumeric C(TNumeric(1)/TNumeric(i));
                if(i % 2 == 0) C = C*TNumeric(-1);
                Res.Coefs[i] = C.Simplify();
            }
            Res.RemainderPower = LastTerm;
        }
        break;
        case OperatorLog:
        //выдает разложение log(1+x)
        {
            Res = GetMaclaurin(OperatorLn, LastTerm);
            Res = Res * (TNumeric(1)/MakeLn(TNumeric(10)));
        };
        case OperatorSqrt:     //returns series for sqrt(1+x)
        case OperatorFrac:     //returns series for 1/(1+x)
        {
            Res.Coefs.assign(LastTerm+1, TNumeric("0"));
            TNumeric Pow;
            if(Operator == OperatorFrac) Pow = TNumeric(-1);
            if(Operator == OperatorSqrt) Pow = TNumeric(1/2);
            for(size_t i = 0; i <= LastTerm; i++)
            {
                TNumeric Coef("1"); //Coef = C_i^Pow
                for(size_t k = 1; k <= i; k++)
                    Coef = Coef*(Pow - TNumeric(k) +TNumeric(1))/TNumeric(k);
                Coef = Coef.Simplify();
                Res.Coefs[i] = Coef;
            }
            Res.RemainderPower = LastTerm;
        }
    }


    Res.RemainderPower = LastTerm; //на всякий случай тут продублируем присвоение    
    return TPolynomSeries(Res);
}


bool TMaclaurinSeriesProblem::ExpandIntoSeries(THTMLWriter *Writer, TPolynomSeries *Res, string var, const TNumeric& N, int LastTerm)
{
    switch(N.Operator)
    {
        case OperatorSum:
            *Res = TPolynomSeries(TNumeric(0));
            for(size_t i = 0; i < N.Operands.size(); i++)
            {
                TPolynomSeries Term;
                if(ExpandIntoSeries(Writer, &Term, var, N.Operands[i], LastTerm))
                    *Res = *Res + Term;
                else return false;
            };
            break;
        case OperatorMinus:
            *Res = TPolynomSeries(TNumeric(0));
            for(size_t i = 0; i < N.Operands.size(); i++)
            {
                TPolynomSeries Term;
                if(ExpandIntoSeries(Writer, &Term, var, N.Operands[i], LastTerm))
                {                    
                    if(i == 0) *Res = Term;
                    else *Res = *Res - Term;
                }
                else return false;
            };
        break;
        case OperatorProd:
            *Res = TPolynomSeries(TNumeric(1));
            for(size_t i = 0; i < N.Operands.size(); i++)
            {
                TPolynomSeries Term;
                if(ExpandIntoSeries(Writer, &Term, var, N.Operands[i], LastTerm))
                    *Res = *Res * Term;
                else return false;
            };
        break;
        case OperatorFrac:
            {
                if(N.Operands[1].DependsOn(var.c_str()))
                    //знаменатель зависит от переменной
                {
                    TNumeric N2 = MakeProd(N.Operands[0], MakePow(N.Operands[1], TNumeric(-1)));
                    //N2 = N2.Simplify(); //если вызвать Simplify, то N2 опять станет дробью
                    if(Writer)
                        Writer->AddParagraph("Transofming %N", MakeEquality(N, N2));
                    return ExpandIntoSeries(Writer, Res, var, N2, LastTerm);
                } else {
                    //знаменатель - константа
                    if(ExpandIntoSeries(Writer, Res, var, N.Operands[0], LastTerm) == false) return false;
                    *Res = *Res * (TNumeric(1)/N.Operands[1]);
                }
            }
        break;
        case OperatorSin:
        case OperatorCos:
        case OperatorTg:
        case OperatorArcsin:
        case OperatorArccos:
        case OperatorArctg:
        case OperatorExp:
        case OperatorSqrt:
        case OperatorSh:
        case OperatorCh:
        {
            TPolynomSeries Arg;
            TNumeric Arg0 = N.Operands[0].Substitute(var, TNumeric(0)).Simplify();
            if(Arg0 == 0)
            {
                if(ExpandIntoSeries(Writer, &Arg, var, N.Operands[0], LastTerm))
                {
                    *Res = GetMaclaurin(N.Operator, LastTerm).Substitute(Arg, LastTerm);
                } else return false;
            } else {
                //Используем формулы суммы
                //sin(f(x)) = sin(f(x)-f(0)+f(0)) = sin t * cos f(0) + cos t * sin f(0)
                if(ExpandIntoSeries(Writer, &Arg, var, N.Operands[0], LastTerm))
                {
                    TPolynomSeries t = Arg - Arg0;
                    if(Writer)
                    {
                        Writer->AddFormula(MakeEquality(TNumeric("t"), N.Operands[0]-TNumeric(1)));
                        Writer->AddFormula(MakeEquality(TNumeric("t"), t.GetNumeric(var)));
                    }
                    switch(N.Operator)
                    {
                        case OperatorSin:
                        //sin(a+b) = sin a cos b + cos a sin b
                            *Res = GetMaclaurin(OperatorSin, LastTerm).Substitute(t, LastTerm) * MakeCos(Arg0) +  GetMaclaurin(OperatorCos, LastTerm).Substitute(t, LastTerm) * MakeSin(Arg0);
                        break;
                        case OperatorCos:
                        //cos(a+b) = cos a cos b - sin a sin b
                            *Res = GetMaclaurin(OperatorCos, LastTerm).Substitute(t, LastTerm) * MakeCos(Arg0) -  GetMaclaurin(OperatorSin, LastTerm).Substitute(t, LastTerm) * MakeSin(Arg0);
                        break;
                        case OperatorTg:
                        //todo
                        //tg(a+b) = (tg(a) + tg(b))/(1-tg a tg b)                            
                            {
                                TPolynomSeries tg_t = GetMaclaurin(OperatorTg, LastTerm).Substitute(t, LastTerm); //tg(t)
                                TPolynomSeries Nominator = tg_t + MakeTg(Arg0);
                                TPolynomSeries Denominator = TPolynomSeries(TNumeric(1)) - tg_t * MakeTg(Arg0);
                                TPolynomSeries Denominator1 =  GetMaclaurin(OperatorFrac, LastTerm).Substitute(Denominator, LastTerm);
                                *Res = Nominator*Denominator1;
                                Res->Cut(LastTerm);
                            };
                        break;
                        case OperatorExp:
                        //exp(a+b) = exp(a) exp(b)
                            *Res = GetMaclaurin(OperatorExp, LastTerm).Substitute(t, LastTerm) * MakeExp(Arg0);
                        break;
                        default:
                            if(Writer)
                            {
                                Writer->BeginError();
                                Writer->AddParagraph("Can not find Maclaurin polynomial for %N, because argument is not in vicinity of zero.", N);
                                Writer->EndError();
                            };
                            return false;
                    };
                } else return false;
            }
        };
        break;
        case OperatorLn:
        {
            //ln f(x) = ln (1 + (f(x)/f(0) - 1)) + ln f(0)
            // t = f(x)/f(0) - 1
            TPolynomSeries f;
            if(ExpandIntoSeries(Writer, &f, var, N.Operands[0], LastTerm))
            {
                TNumeric f0 = N.Operands[0].Substitute(var, TNumeric(0)).Simplify();
                if(f0 == TNumeric(0))
                {
                    if(Writer)
                        Writer->AddParagraph("The expression under ln is zero when %n = 0. Can not find Maclaurin polynomial.", TNumeric(var));
                    return false;
                } else {
                    TPolynomSeries t =  f*(TNumeric(1)/f0) + (-TNumeric(1));
                    TPolynomSeries Ln = GetMaclaurin(OperatorLn, LastTerm);
                    *Res = Ln.Substitute(t, LastTerm) + MakeLn(f0);
                    if(Writer)
                    {
                        Writer->AddFormula(MakeEquality(TNumeric("t"), t.GetNumeric(var)));
                        Writer->AddFormula(MakeEquality(MakeLn(TNumeric("t")+TNumeric(1)), Ln.GetNumeric("t")));
                        Writer->AddFormula(MakeEquality(N, Res->GetNumeric(var)));
                    }
                }
            } else return false;
        };
        break;
        case OperatorLog:
        {
            TNumeric N2 = MakeFrac(MakeLn(N.Operands[0]), MakeLn(N.Operands[1]));
            N2 = N2.Simplify();
            if(Writer)
                Writer->AddParagraph("Transforming %N", MakeEquality(N, N2));
            return ExpandIntoSeries(Writer, Res, var, N2, LastTerm);
        }
        break;
        case OperatorLg:
        {
            TNumeric N2 = MakeFrac(MakeLn(N.Operands[0]), MakeLn(TNumeric(10)));
            return ExpandIntoSeries(Writer, Res, var, N2, LastTerm);
        };
        break;
        case OperatorPow:
        {
            TNumeric Base = N.Operands[0];
            TNumeric Pow = N.Operands[1];
            if(Pow.DependsOn(var.c_str()))
            {
                //g(x)^f(x) = exp(f(x)*ln(g(x)))
                TNumeric Arg = Pow*MakeLn(Base);
                TPolynomSeries ArgSer;
                if(Writer)
                    Writer->AddParagraph("Transforming %N ", MakeEquality(N, MakeExp(Arg)));
                if(ExpandIntoSeries(Writer, &ArgSer, var, Arg, LastTerm))
                {
                    if(Writer)
                        Writer->AddFormula(MakeEquality(Arg, ArgSer.GetNumeric(var)));
                    *Res = GetMaclaurin(OperatorExp, LastTerm);
                    *Res = Res->Substitute(ArgSer, LastTerm);
                } else return false;
            } else {
                if(Base.DependsOn(var.c_str()) == false)
                    //ни основание, ни показатель не зависят от переменной => константа
                {
                    *Res = MakePow(Base, Pow).Simplify();
                    return true;
                }
                int P;
                bool Integer = Pow.IsInteger(&P);
                if(Integer == false || P < 0)
                {
                    TNumeric Arg0 = N.Operands[0].Substitute(var, TNumeric("0")).Simplify();
                    if(Arg0 == TNumeric(0))
                    {
                        if(Writer)
                        {
                            Writer->BeginError();
                            Writer->AddParagraph("Can not find Maclaurin polynomial for %N, because base of power function should be non-zero when %n in vicinity of zero", N, TNumeric(var));
                            Writer->EndError();
                        }
                        return false;
                    } else {
                        if(Integer == false && Arg0.Calculate() < 0)
                        {
                            if(Writer)
                            {
                                Writer->BeginError();
                                Writer->AddParagraph("Can not find Maclaurin polynomial for %N, because base of power function should be positive if power is fractional.", N);
                                Writer->EndError();
                            }
                            return false;
                        };
                        TNumeric t = (N.Operands[0]/Arg0 - TNumeric(1)).Simplify();
                        TPolynomSeries tser;
                        if(!ExpandIntoSeries(Writer, &tser, var, t, LastTerm)) return false;
                        if(Arg0 != TNumeric(1))
                        {
                            if(Writer)
                            {
                                Writer->AddFormula(MakeEquality(TNumeric("t"), t));
                                Writer->AddFormula(MakeEquality(t, tser.GetNumeric(var)));
                            }
                        };
                        Res->Coefs.assign(LastTerm+1, TNumeric("0"));
                        for(size_t i = 0; i <= (size_t)LastTerm; i++)
                        {
                            TNumeric Coef("1"); //Coef = C_i^Pow
                            for(size_t k = 1; k <= i; k++)
                                Coef = Coef*(Pow - TNumeric(k) +TNumeric(1))/TNumeric(k);
                            Coef = Coef.Simplify();
                            Res->Coefs[i] = Coef;
                        }
//                        std::cout<<Res->GetNumeric("x").CodeBasic()<<endl;
                        Res->RemainderPower = LastTerm;
                        *Res = Res->Substitute(tser, LastTerm)*MakePow(Arg0, Pow);
                    }
                } else {
                    if(P == 0)
                    {
                        *Res = TPolynomSeries(TNumeric("1"));
                    };
                    //случай P<0 разобран выше
                    if(P > 0)
                    {
                        TPolynomSeries BSer;
                        if(ExpandIntoSeries(Writer, &BSer, var, Base, LastTerm))
                        {
                            *Res = BSer;
                            for(size_t k = 1; k < (size_t)P; k++)
                                *Res = (*Res) * BSer;
                            Res->Cut(LastTerm);
                        } else return false;
                    };
                };
            };
        };
        break;

         case OperatorConst:
            if(N.K == var)
            {
                Res->Coefs.clear();
                Res->Coefs.push_back(TNumeric("0"));
                Res->Coefs.push_back(TNumeric("1"));
                Res->RemainderPower = -1;
            } else {
                Res->Coefs.clear();
                Res->Coefs.push_back(N);
                Res->RemainderPower = -1;
            }
        break;
    }
    return true;
}

bool TMaclaurinSeriesProblem::GetSolution(THTMLWriter* Writer)
{
    TPolynomSeries Ser;
    if(ExpandIntoSeries(Writer, &Ser, "x", *this->Conditions, MaxPower))
    {
        TNumeric N = Ser.GetNumeric("x");
        if(Writer)
        {
            Writer->AddParagraph("Answer: %N", N);
        };
    } else {
        if(Writer)
            Writer->WriteError("Can not find Maclaurin polynomial.");
    }
    return true;
}

void TMaclaurinSeriesProblem::SaveToFile(ofstream &f)
{
    f.write((char*)&MaxPower, sizeof(MaxPower));
    TProblem::SaveToFile(f);
}

void TMaclaurinSeriesProblem::LoadFromFile(ifstream &f)
{
    f.read((char*)&MaxPower, sizeof(MaxPower));
    if(MaxPower > 10)
        throw "TMaclaurinSeriesProblem::LoadFromFile: MaxPower is too large. Invalid file format?";
    TProblem::LoadFromFile(f);
}



//TNumeric XDeriv = Conditions->Derivative();
//AddLine(Lines, Ntabs, new TNumeric(XDeriv));

