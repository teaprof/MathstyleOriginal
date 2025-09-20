#include<iostream>
//#include <typeinfo>
#include "ObjectsFactory.h"

using namespace std;

#define SignatureLen 7
char Signature[SignatureLen] = {0x1F, (char)0xAA, 0x34, 0x45, (char)0x90, (char)0xFF, 0x11};
void FillSignature(char *Signature)
//записываем размеры основных типов данных - на случай, если проект будет перекомпилирован на другую платформу. Если типы данных поменяются,
//то ранее сохранённые файлы будет открыть невозможно, но ошибки не будет
{
    Signature[SignatureLen-1] = sizeof(char);
    Signature[SignatureLen-2] = sizeof(int);
    Signature[SignatureLen-3] = sizeof(uint16_t);
    Signature[SignatureLen-4] = sizeof(bool);
}

TProblem* MakeCopy(TProblem* P)
//creates an object of the same type as P and returns pointer to it
{
#define CHECKTYPE(TYPE)  {TYPE Obj; if(strcmp(Buf, Obj.GetClassName().c_str()) == 0) { Problem = new TYPE(*((TYPE*)P)); };};
#define CHECKTYPE2(TYPE, STR)  if(strcmp(Buf, STR) == 0) { Problem = new TYPE(*((TYPE*)P)); };
    TProblem *Problem = 0;
    const char *Buf;
    string str = P->GetClassName();
    Buf = str.c_str();
    CHECKTYPE(TSimplifyProblem);
    CHECKTYPE(TLinearEquality);
    CHECKTYPE(TSquareEquality);
    CHECKTYPE(TLinearInequality);
    CHECKTYPE(TSquareInequality);
    CHECKTYPE(TRationalFunctionDerivative);
    CHECKTYPE(TPolynomDerivative);
    CHECKTYPE(TPolynomialInequality);
    CHECKTYPE(TPolynomialEquality);
    CHECKTYPE(TPowerEquality);
    CHECKTYPE(TRationalSumEquality);
    CHECKTYPE(TLinearTrigEquality);
    CHECKTYPE(THomogeneousTrigEquality);
    CHECKTYPE2(TSetOfLinearInequalities, TSetOfLinearInequalitiesStr);
    CHECKTYPE2(TSystemOfLinearInequalities, TSystemOfLinearInequalitiesStr);
    CHECKTYPE2(TSetOfSquareInequalities, TSetOfSquareInequalitiesStr);
    CHECKTYPE2(TSystemOfSquareInequalities, TSystemOfSquareInequalitiesStr);
    CHECKTYPE2(TSystemOfEquations, "TSystemOfEquations"); //SLAE
    CHECKTYPE(TDerivativeProblem);
    CHECKTYPE(TPolynomIntegralProblem);
    CHECKTYPE(TIntegralProblem);
    CHECKTYPE(TRationalIntegralProblem);
    CHECKTYPE(TMaclaurinSeriesProblem);
#undef CHECKTYPE
#undef CHECKTYPE2
    if(Problem == 0)
    {
        cerr<<"Can not make a copy of unknown object"<<endl;
        cerr<<"GetClassName() = "<<Buf<<endl;
        throw "Can not make a copy of unknown object";
        return 0;
    }
    return Problem;
}

TProblem* LoadFromFile(ifstream &f, char* ResultMsg, int ResultMsgLen)
{
#define BufLen 100
char Buf[BufLen];
char S[SignatureLen];
    FillSignature(Signature);
    f.read(S, SignatureLen);
    if(strncmp(Signature, S, SignatureLen) != 0)
    {
        strncpy(ResultMsg, "Incorrect file format", ResultMsgLen);
        return 0;
    };
    f.read(Buf, BufLen);
    Buf[BufLen-1] = 0;
    TProblem *Problem = 0;
    if(strcmp(Buf, "TSimplifyProblem") == 0)
    {
        Problem = new TSimplifyProblem;
    }
    if(strcmp(Buf, "TLinearEquality") == 0)
    {
        Problem = new TLinearEquality;
    };
    if(strcmp(Buf, "TSquareEquality") == 0)
    {
        Problem = new TSquareEquality;
    }
    if(strcmp(Buf, "TLinearInequality")== 0)
    {
        Problem = new TLinearInequality;
    }
    if(strcmp(Buf, "TSquareInequality") == 0)
    {
        Problem = new TSquareInequality;
    }
    if(strcmp(Buf, "TSetOfLinearInequalities") == 0)
    {
        Problem = new TSetOfLinearInequalities;
    }
    if(strcmp(Buf, "TSystemOfSquareInequalities") == 0)
    {
        Problem = new TSystemOfSquareInequalities;
    }
    if(strcmp(Buf, "TRationalFunctionDerivative") == 0)
    {
        Problem = new TRationalFunctionDerivative;
    }
    if(strcmp(Buf, "TPolynomDerivative") == 0)
    {
        Problem = new TPolynomDerivative;
    }
    if(strcmp(Buf, "TPolynomialInequality") == 0)
    {
        Problem = new TPolynomialInequality;
    }
    if(strcmp(Buf, "TPolynomialEquality") == 0)
    {
        Problem = new TPolynomialEquality;
    }
    if(strcmp(Buf, "TPowerEquality") == 0)
    {
        Problem = new TPowerEquality;
    }
    if(strcmp(Buf, "TRationalSumEquality") == 0)
    {
        Problem = new TRationalSumEquality;
    }
    if(strcmp(Buf, "TLinearTrigEquality") == 0)
    {
        Problem = new TLinearTrigEquality;
    }
    if(strcmp(Buf, "THomogeneousTrigEquality") == 0)
    {
        Problem = new THomogeneousTrigEquality;
    }
    if(strcmp(Buf, TSetOfSquareInequalitiesStr) == 0)
    {
        Problem = new TSetOfSquareInequalities;
    }
    if(strcmp(Buf, TSystemOfSquareInequalitiesStr) == 0)
    {
        Problem = new TSystemOfSquareInequalities;
    }
    if(strcmp(Buf, "TSystemOfEquations") == 0)
    {
        Problem = new TSystemOfEquations;
    }
    if(strcmp(Buf, "TDerivativeProblem") == 0)
    {
        Problem = new TDerivativeProblem;
    }
    if(strcmp(Buf, "TPolynomIntegralProblem") == 0)
    {
        Problem = new TPolynomIntegralProblem;

    }
    if(strcmp(Buf, "TRationalIntegralProblem") == 0)
    {
        Problem = new TRationalIntegralProblem;
    }
    if(strcmp(Buf, "TIntegralProblem") == 0)
    {
        Problem = new TIntegralProblem;
    }
    if(strcmp(Buf, "TMaclaurinSeriesProblem") == 0)
    {
        Problem = new TMaclaurinSeriesProblem;
    }
    if(Problem == 0)
    {
        strncpy(ResultMsg, "Incorrect file format", ResultMsgLen);
        return 0;
    }
    Problem->LoadFromFile(f);
    return Problem;
}

void SaveToFile(ofstream &f, TProblem *P)
{

#define BufLen 100
char Buf[BufLen];    
    strncpy(Buf, P->GetClassName().c_str(), 100);
    Buf[99] = 0;
    FillSignature(Signature);
    f.write(Signature, SignatureLen);
    f.write(Buf, BufLen);
    P->SaveToFile(f);
}

