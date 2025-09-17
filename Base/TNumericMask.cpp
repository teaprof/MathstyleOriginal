#include "TNumericMask.h"

#include <algorithm>
#include <iostream>
using namespace std;

#define __DEBUG__

TNumericMask::TNumericMask()
{

}

bool NextCount(vector<size_t> &Counter, size_t MaxCount)
{
    size_t i = 0;
    do
    {
        Counter[i]++;
        if(Counter[i] > MaxCount)
        {
            Counter[i] = 0;
            i++;
        } else break;
    }while(i < Counter.size());
    return (i < Counter.size());
}

bool TNumericMask::CheckTemplateConstant(const TNumeric& Template, const TNumeric& NConst, std::map<std::string, TNumeric> &Coefs)
{
//    N = N.Simplify();
    if(Template.K == "x")
    {
        if(NConst.Operator == OperatorConst && NConst.K == "x") return true;
        else return false;
    } else {
        if(Template.IsVariable()) //любая переменная, но не x
        {
            if(NConst.DependsOn("x")) return false;
            else {
                //cout<<K<<" = "<<N.CodeBasic()<<endl;
                //проверяем, чтобы не было противоречий с уже записанными константами
                map<string, TNumeric>::iterator j = Coefs.find(Template.K);
                if(j == Coefs.end())
                    Coefs.insert(pair<string, TNumeric>(Template.K, NConst));
                else
                    if(j->second != NConst) return false;
                return true;
            };
        } else {
            //Число
            if(NConst.Operator == OperatorConst && NConst.K == Template.K) return true;
            else return false;
        };
    };
}


struct TPairsIndex
{
    int Index, Pairs;
};

bool PairsIndexCompare(const TPairsIndex& I, const TPairsIndex& J)
{
    return I.Pairs > J.Pairs;
}
void PrintVV(vector< vector<size_t> > VV)
{
    cout<<"------------------------"<<endl;
    for(size_t i = 0; i < VV.size(); i++)
    {
        for(size_t j = 0; j < VV[i].size(); j++)
            cout<<VV[i][j]<<"\t";
        cout<<endl;
    }
}

void DisperseSort(vector< vector<size_t> > &SubSetsVector)
{
//    PrintVV(SubSetsVector);
vector<size_t> Sorted;
vector<TPairsIndex> Pairs;
//подсчитываем количество пар одинаковых чисел
    TPairsIndex PairsIndex;
    PairsIndex.Index = 0;
    PairsIndex.Pairs = 0;
    Pairs.assign(SubSetsVector.size(), PairsIndex);
    for(size_t i = 0; i < SubSetsVector.size(); i++)
    {
        Pairs[i].Index = i;
        Sorted = SubSetsVector[i];
        sort(Sorted.begin(), Sorted.end());
        for(size_t j = 1; j < Sorted.size(); j++)
            if(Sorted[j-1] == Sorted[j])
                Pairs[i].Pairs++;
    }
    sort(Pairs.begin(), Pairs.end(), PairsIndexCompare);
vector< vector<size_t> > SubSetsVectorTemp = SubSetsVector;
    for(size_t i = 0; i < SubSetsVectorTemp.size(); i++)
    {
        SubSetsVector[i] = SubSetsVectorTemp[Pairs[i].Index];
    }
//    PrintVV(SubSetsVector);
}


bool TNumericMask::CheckTemplateCommunative(const TNumeric& Template, const TNumeric& NConst, std::map<std::string, TNumeric> &Coefs)
//Operator должен быть коммутативным
{
#ifdef __DEBUG__
    string str;
    for(size_t i = 0; i < (size_t)DebugNTabs; i++)
        str = str + "  ";
    cout<<str<<"Template: "<<Template.CodeBasic()<<endl;
    cout<<str<<"Value: "<<NConst.CodeBasic()<<endl;
#endif
TNumeric N = NConst;
TNumeric AdditionalTerm;
    switch(Template.Operator)
    {
        case OperatorSum: AdditionalTerm = TNumeric("0"); break;
        case OperatorProd: AdditionalTerm = TNumeric("1"); break;
        default: return false;
    }

    if(Template.Operator != N.Operator)
    {
        TNumeric NewN;
        if(Template.Operator == OperatorProd && N.IsEqual(TNumeric("0")))
//        if(Template.Operator == OperatorProd && N == TNumeric("0"))
        {
            //особый случай: если N = 0, то нужно N преобразовать к виду N*f(x), где f(x) - зависящая от x часть Template
            NewN.Operator = OperatorProd;
            NewN.OperandsPushback(TNumeric("0"));
            for(size_t i = 0; i < Template.Operands.size(); i++)
                if(Template.Operands[i].DependsOn("x")) NewN.OperandsPushback(Template.Operands[i]);
        } else {
            NewN.Operator = Template.Operator;
            NewN.OperandsPushback(AdditionalTerm);
            NewN.OperandsPushback(N);
        }
        N = NewN;        

    };

    vector<size_t> SubSets;
    SubSets.assign(N.Operands.size(), 0);
/*    vector< vector<size_t> > SubSetsVector;
    size_t SubSetsVectorElementsCount = Template.Operands.size();
    for(size_t i = 1; i < N.Operands.size(); i++)
        SubSetsVectorElementsCount *= Template.Operands.size();
    SubSetsVector.assign(SubSetsVectorElementsCount, SubSets);
    size_t Count1 = 0;
    do
    {
        SubSetsVector[Count1++] = SubSets;
    }while(NextCount(SubSets, Template.Operands.size() - 1));
    if(SubSetsVector.size() > 4)
        DisperseSort(SubSetsVector);*/

//    for(size_t SubCount = 0; SubCount < SubSetsVector.size(); SubCount++)
    //Счетчик по всем SubSets[i]
    do
    {
    //    SubSets = SubSetsVector[SubCount];
        std::map<std::string, TNumeric> TempCoefs;
        bool AllSuccess = true; //Все операнды подогнать под шаблон удалось
        for(size_t j = 0; j < Template.Operands.size(); j++)
        {
            TNumeric SubNumeric;
            SubNumeric.Operator = N.Operator;
            for(size_t i = 0; i < SubSets.size(); i++)
                if(SubSets[i] == j)
                    SubNumeric.OperandsPushback(N.Operands[i]);
            if(SubNumeric.Operands.size() == 0)
                SubNumeric.OperandsPushback(AdditionalTerm);
            if(SubNumeric.Operands.size() == 1) //исключаем случай вида SumNumeric = prod(x) или sum(x) - произведение и сумма одного члена
            {
                //SubNumeric = SubNumeric.Operands[0] не работает
                TNumeric Temp = SubNumeric.Operands[0];
                SubNumeric = Temp;
            };
            if(Match(Template.Operands[j], SubNumeric, TempCoefs) == false)
            {
                //операнд подогнать под шаблон не удалось, переходим к следующему перераспределению операндов
                SubNumeric = SubNumeric.Simplify();
                if(Match(Template.Operands[j], SubNumeric, TempCoefs) == false)
                {
/*#ifdef __DEBUG__
                cout<<SubNumeric.CodeBasic()<<endl;
                if(SubNumeric.Operands.size() > 0)
                    cout<<SubNumeric.Operands[1].CodeBasic()<<endl;
#endif*/
                    AllSuccess = false;
                    break;
                };
            }
        };
        if(AllSuccess)
        {
            if(AcceptCoefs(Coefs, TempCoefs))return true;
            //else return false;
        }
        if(NextCount(SubSets, Template.Operands.size() - 1) == false)
            break;
    } while(true);
    return false;
}


TNumeric TNumericMask::ToProd(const TNumeric& N)
//преобразует к произведению
{
    TNumeric Res;
    Res.Operator = OperatorProd;
    if(N.Operator == OperatorFrac)
    {
        Res.OperandsPushback(ToProd(N.Operands[0]));
        Res.OperandsPushback(MakePow(ToProd(N.Operands[1]), TNumeric("-1")));
    } else
    if(N.Operator == OperatorProd)
    {
        for(size_t i = 0; i < N.Operands.size(); i++)
            Res.OperandsPushback(ToProd(N.Operands[i]));
    } else Res = N;
    //todo: сделать преобразование x^k = x*x*...*x (на случай шаблона x^k*x^2 )
    //remark: x^k*x^2 - плохой шаблон, такие шаблоны надо исключить

    //Преобразуем a*(b*c) к виду a*b*c
    if(Res.Operator == OperatorProd)
    {
        size_t i = 0;
        while(i<Res.Operands.size())
        {
            if(Res.Operands[i].Operator == OperatorProd)
            {
                for(size_t j = 0; j < Res.Operands[i].Operands.size(); j++)
                {
                    Res.Operands.insert(Res.Operands.begin()+i, Res.Operands[i].Operands[j]);
                    i++;
                };
                Res.Operands.erase(Res.Operands.begin() +  i);
            } else i++;
        };
    };
    return Res;
}

TNumeric TNumericMask::Expand(const TNumeric& N)
//раскрывает скобки
{
TNumeric N2 = N;
    for(size_t i = 0; i < N2.Operands.size(); i++)
        N2.Operands[i] = Expand(N2.Operands[i]);
    if(N2.Operator == OperatorProd)
    {
        for(size_t i = 0; i < N2.Operands.size(); i++)
            if(N2.Operands[i].Operator == OperatorSum || N2.Operands[i].Operator == OperatorMinus)
            {
                TNumeric Res;
                Res.Operator = N2.Operands[i].Operator;
                for(size_t j = 0; j < N2.Operands[i].Operands.size(); j++)
                {
                    TNumeric Term = N2.Operands[i].Operands[j];
                    for(size_t k = 0; k < N2.Operands.size(); k++)
                    {
                        if(k<i) Term = N2.Operands[k]*Term;
                        if(k>i) Term = Term*N2.Operands[k];
                    };
                    Res.OperandsPushback(Expand(Term));
                };
                Res = ToSum(Res);
                return Res;
            };
        return N2;
    } else return N2;
};

TNumeric TNumericMask::ToSum(const TNumeric& N)
//преобразует к сумме
{
    TNumeric Res;
    Res.Operator = OperatorSum;
    if(N.Operator == OperatorMinus)
    {
        Res.OperandsPushback(ToSum(N.Operands[0]));
        for(size_t i = 1; i < N.Operands.size(); i++)
            Res.OperandsPushback(MakeProd(TNumeric("-1"), ToSum(N.Operands[i])));
    } else
    if(N.Operator == OperatorSum)
    {
        for(size_t i = 0; i < N.Operands.size(); i++)
            Res.OperandsPushback(ToSum(N.Operands[i]));
    } else {
        if(N.Operator == OperatorProd)
            //раскрываем скобки
        {
            Res = Expand(N);
        } else {
            if(N.Operator == OperatorPow)
                //если степень целая, то раскрываем степень
            {
                const TNumeric* Power = &N.Operands[1];
                int d;
                if(Power->IsInteger(&d) && d >= 0)
                {
                    switch(d)
                    {
                        case 0:
                            Res = TNumeric("1"); //выражение имеет вид a^0
                            break;
                        case 1:
                            Res = ToSum(N.Operands[0]);
                            break;
                        default:
                            Res = N;

/*                          Раскрываем скобки - так делать не надо
//если раскрывать степень, то будет очень много слагаемых в случаях, например, (x^2+x+1)^2 -- CheckTemplateCommutative будет очень медленной
                             Res.Operator = OperatorProd;
                            for(size_t i = 0; i < d; i++)
                                Res.OperandsPushback(N.Operands[0]);
                            Res = Expand(Res);*/
                    };
                } else Res = N;
            } else Res = N;
        };
    };


    //Преобразуем a+(b+c) к виду a+b+c
    if(Res.Operator == OperatorSum)
    {
        size_t i = 0;
        while(i<Res.Operands.size())
        {
            if(Res.Operands[i].Operator == OperatorSum)
            {
                for(size_t j = 0; j < Res.Operands[i].Operands.size(); j++)
                {
                    Res.Operands.insert(Res.Operands.begin()+i, Res.Operands[i].Operands[j]);
                    i++;
                };
                Res.Operands.erase(Res.Operands.begin() +  i);
            } else i++;
        };
    };

#ifdef __DEBUG__
    return TNumeric(Res);
#else
    return Res;
#endif
}

TNumeric TNumericMask::ToPow(const TNumeric& NConst)
{
    TNumeric N = NConst;
    if(N.Operator == OperatorFrac)
     //1/f(x) => f(x)^(-1)
    {
        TNumeric A = N.Operands[0];
        TNumeric B = N.Operands[1];
        if(A == TNumeric("1"))
        {
             N = B^TNumeric("-1");
        } else {
            N = (B/A)^TNumeric("-1");
        }
    } else {
    //f(x)^k * g(x)^k => (f(x)*g(x))^k
        N = ToProd(NConst);
        if(N.Operator == OperatorProd)
        {
            for(size_t i = 0; i < N.Operands.size(); i++) N.Operands[i] = ToPow(N.Operands[i]);
            //сравниваем показатели степени
            TNumeric Power;
            bool AllPower = true;
            for(size_t i = 0; i < N.Operands.size(); i++)
            {
                if(N.Operands[i].Operator != OperatorPow)
                {
                    AllPower = false;
                    break;
                    }
                if(i == 0) Power = N.Operands[i].Operands[1];
                else
                    if(Power != N.Operands[i].Operands[1])
                    {
                        AllPower = false;
                        break;
                    }
            }
            if(AllPower)
            {
                TNumeric Base;
                Base.Operator = OperatorProd;
                for(size_t i = 0; i < N.Operands.size(); i++)
                    Base.OperandsPushback(N.Operands[i].Operands[0]);
                N = Base^Power;
            }
        };
    }
    return N;
};

bool TNumericMask::CheckTemplateProd(const TNumeric& Template, const TNumeric &N, std::map<std::string, TNumeric> &Coefs)
{
    TNumeric N2 = ToProd(N); //Преобразуем деление в умножение
    return CheckTemplateCommunative(Template, N2, Coefs);
}

bool TNumericMask::CheckTemplatePow(const TNumeric& Template, const TNumeric& NConst, std::map<std::string, TNumeric> &Coefs)
{
    std::map<std::string, TNumeric> TempCoefs;
    if(NConst.Operator == OperatorPow)
    {
        if(Match(Template.Operands[0], NConst.Operands[0], TempCoefs) && Match(Template.Operands[1], NConst.Operands[1], TempCoefs))
        {
            if(AcceptCoefs(Coefs, TempCoefs)) return true;
            else return false;
        }
        else return false;
    } else {
        TNumeric N = ToPow(NConst);
        if(N.Operator == OperatorPow && Match(Template.Operands[0], N.Operands[0], TempCoefs) && Match(Template.Operands[1], N.Operands[1], TempCoefs))
        {
            if(AcceptCoefs(Coefs, TempCoefs)) return true;
        }
        TempCoefs.clear();
        std::map<std::string, TNumeric> NewCoefs;
        if(N == TNumeric(1))
        {
            if(Match(Template.Operands[1], TNumeric("0"), TempCoefs))
                //случай: 1 попадает под шаблон f(x)^0
                if(AcceptCoefs(Coefs, NewCoefs)) return true;
            TempCoefs.clear();
            if(Match(Template.Operands[0], TNumeric("1"), TempCoefs))
                //случай: 1 попадает под шаблон 1^f(x)
                if(AcceptCoefs(Coefs, NewCoefs)) return true;
        } else {
            //случай: f попадает под шаблон f^1
            TempCoefs.clear();
            if(Match(Template.Operands[0], NConst, TempCoefs))
                if(Match(Template.Operands[1], TNumeric("1"), TempCoefs))
                    if(AcceptCoefs(Coefs, TempCoefs))return true;

            //случай: f*f*f = f^3
            if(NConst.Operator == OperatorProd)
            {
                bool AllEquals = true;
                for(size_t i = 1; i < NConst.Operands.size(); i++)
                    if(NConst.Operands[0] != NConst.Operands[1])
                    {
                        AllEquals = false;
                        break;
                    }
                if(AllEquals)
                {
                    TNumeric TempN;
                    TempN.Operator = OperatorPow;
                    TempN.OperandsClear();
                    TempN.OperandsPushback(NConst.Operands[0]);
                    TempN.OperandsPushback(TNumeric(NConst.Operands.size()));
                    if(CheckTemplatePow(Template, TempN, TempCoefs))
                    {
                        if(AcceptCoefs(Coefs, TempCoefs)) return true;
                        else return false;
                    };
                }
                return false;
             }
            return false;
        };
    };
    return false;
}

bool TNumericMask::CheckTemplateFrac(const TNumeric& Template, const TNumeric& N, std::map<std::string, TNumeric> &Coefs)
{
TNumeric Template2 = ToProd(Template);
    return Match(Template2, N, Coefs);
}

bool TNumericMask::CheckTemplateSum(const TNumeric& Template, const TNumeric& N, std::map<std::string, TNumeric> &Coefs)
{
TNumeric N2 = ToSum(N);
    return CheckTemplateCommunative(Template, N2, Coefs);
}

bool TNumericMask::CheckTemplateMinus(const TNumeric& Template, const TNumeric& N, std::map<std::string, TNumeric> &Coefs)
{
    Q_UNUSED(Template);
    TNumeric Template2 = ToSum(Template2);
    return Match(Template2, N, Coefs);
}

bool TNumericMask::AcceptCoefs(std::map<std::string, TNumeric> &Coefs, std::map<std::string, TNumeric> &NewCoefs)
{
    map<string,TNumeric>::iterator i;
    for( i = NewCoefs.begin() ; i != NewCoefs.end(); i++ )
    {
        map<string,TNumeric>::iterator j;
        j = Coefs.find(i->first);
        if(j == Coefs.end())
        {
            Coefs.insert(pair<string, TNumeric>(i->first, i->second));
        }
        else
        {
            if(j->second != i->second) return false;
        }
    }
    return true;
}


bool TNumericMask::Match(const TNumeric& Template, const TNumeric& N, std::map<std::string, TNumeric> &Coefs)
{
    DebugNTabs++;
    bool res = false;
#ifdef __DEBUG__
    string str;
    for(size_t i = 0; i < (size_t)DebugNTabs; i++)str = str + "  ";
    cout<<str<<"Matching: "<<Template.CodeBasic()<<" = "<<N.CodeBasic()<<endl;
#endif
    switch(Template.Operator)
    {
        case OperatorConst: res =  CheckTemplateConstant(Template, N, Coefs); break;
        case OperatorPow: res = CheckTemplatePow(Template, N, Coefs); break;
        case OperatorProd: res = CheckTemplateProd(Template, N, Coefs); break;
        case OperatorFrac: res = CheckTemplateFrac(Template, N, Coefs); break;
        case OperatorSum: res = CheckTemplateSum(Template, N, Coefs); break;
        case OperatorMinus: res = CheckTemplateMinus(Template, N, Coefs); break;
        case OperatorSin:
            if(N.Operator == Template.Operator)
                res = Match(Template.Operands[0], N.Operands[0], Coefs);
        break;
        default: res = false; break;
    }
#ifdef __DEBUG__
    cout<<str<<"Matching: "<<Template.CodeBasic()<<" = "<<N.CodeBasic()<<" ";
    if(res) cout<<"Matches Ok"<<endl;
    else cout<<"Does not match"<<endl;
    for(std::map<string, TNumeric>::iterator i = Coefs.begin(); i != Coefs.end(); i++)
    {
        cout<<str<<" #"<<i->first<<" = "<<i->second.CodeBasic()<<endl;
    };
#endif

    DebugNTabs--;
    return res;
}

vector<string> TNumericMask::FindCoefsNames(const TNumeric& Template)
{
vector<string> Res;
    if(Template.Operator == OperatorConst)
    {
        if(Template.K != "x" && Template.IsVariable())
            if(find(Res.begin(), Res.end(), Template.K) == Res.end())
                Res.push_back(Template.K);

    } else {
        for(size_t i = 0; i < Template.Operands.size(); i++)
        {
            vector<string> Temp = FindCoefsNames(Template.Operands[i]);
            for(size_t j = 0; j < Temp.size(); j++)
                if(find(Res.begin(), Res.end(), Temp[j]) == Res.end())
                    Res.push_back(Temp[j]);

        }
    }
    return Res;
}

bool TNumericMask::CheckAllCoefsFound(const TNumeric& Template)
{
vector<string> CoefsNames = FindCoefsNames(Template);
    for(size_t i = 0; i < CoefsNames.size(); i++)
    {
        if(Coefs.find(CoefsNames[i]) == Coefs.end())
            return false;
        else
        {
#ifdef __DEBUG__
            cout<<CoefsNames[i]<<" = "<<Coefs[CoefsNames[i]].CodeBasic()<<endl;
#endif
        }
    }
    return true;
}

bool TNumericMask::Match(const TNumeric& Template, const TNumeric& N)
{
    Coefs.clear();
#ifdef __DEBUG__
    cout<<"NEW MATCH BEGIN: "<<Template.CodeBasic()<<" == "<<N.CodeBasic()<<endl;
#endif
    DebugNTabs = 0;
    bool res, res1;
    res = Match(Template, N, Coefs);
    res1 = CheckAllCoefsFound(Template);
    return res & res1;
}

