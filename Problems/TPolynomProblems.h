#ifndef TPOLYNOMDERIVATIVE_H
#define TPOLYNOMDERIVATIVE_H

#include "tproblem.h"
#include "Base/polynom.h"

extern TNumeric X_1;
extern TNumeric X_2;
extern TNumeric EmptySet;
extern TNumeric NumericAllReal;
extern TInterval IntervalAllRealNumbers;
extern TNumeric NumPhi;

class TPolynomConditions:  public TProblem
{
        int RightPartID; //вычисляется как MaxPower+1
        bool HaveRightPart; //истина, если необходимо добавлять в Conditions правую часть
    public:
        TNumeric UnknownVar; //обозначение неизвестной переменной
        size_t MaxPower;
        TPolynomConditions(size_t MaxPower = 8, bool HaveRightPart = true, int operation = OperatorEqual);
        //Если AllCoef = true, то будут учитываться все коэфициенты, если false - то нулевые коэфициенты старше MajorPower будут отброшены
        TPolynomConditions(const TPolynom& P, bool HaveRightPart = true, int operation = OperatorEqual, bool AllCoef = true);
        ~TPolynomConditions();

        vector<TNumeric> GetCoef() const;
        TPolynom GetP() const; //rename to getPolynom
        PNumeric GetCoefP(size_t power);
        const PNumeric GetCoef(size_t power) const;
        PNumeric GetRightPartP();
        TNumeric GetRightPart() const;


        //Если AllCoef = true, то будут учитываться все коэфициенты, если false - то нулевые коэфициенты старше MajorPower будут отброшены
        virtual void SetLeftPartP(const TPolynom &P, bool AllCoef = true);
        //sets left part to ax+b
        virtual void SetLeftPart(const TNumeric &a, const TNumeric &b);
        //sets left part to ax^2+bx+c
        virtual void SetLeftPart(const TNumeric &a, const TNumeric &b, const TNumeric& c);
        //устанавливает максимальную степень многочлена MaxPower
        virtual void SetMaxPower(size_t MaxPower, int operation);

        void SetRightPart(const TNumeric& N);

        //меняет Conditions, если они есть
        void SetUnknownVar(TNumeric UnknownVar);

        virtual void SaveToFile(ofstream &f);
        virtual void LoadFromFile(ifstream &f);

        //возвращает UnknownVar^power
        virtual TNumeric GetVarPower(size_t power);

        void Randomize(std::mt19937& rng);
};

class TPolynomialEquality : public TPolynomConditions, public TEquality
{
        bool CheckRationalAndGetNOK(std::shared_ptr<THTMLWriter> Writer, const TPolynom &P, int& NOK); //проверяет, чтобы все коэффициенты в многочлене были рациональными и вычисляет НОК знаменателей
        bool GetIntCoefs(std::shared_ptr<THTMLWriter> Writer, int NOK, const TPolynom& P, vector<int> &IntCoefs); //умножает все коэффициенты многочлена на NOK, после чего они должны стать целыми, и записывает их в IntCoefs
        int TakeOutCommonMultiplicator(std::shared_ptr<THTMLWriter> Writer, vector<int>& Coefs); //выносит общий множитель
        bool TakeOutXk(std::shared_ptr<THTMLWriter> Writer, vector<int>& Coefs); //выносит общий множитель вида x^k
        bool SearchRationalRoots(std::shared_ptr<THTMLWriter> Writer, vector<int>& Coefs, TPolynom& PRemaining); //ищет корни среди рациональных корней
        //PRemaining - оставшийся многочлен после выноса рациональных корней

        bool AnalyzePRemaining(std::shared_ptr<THTMLWriter> Writer, const TPolynom& PRemaining); //пытается что-то еще сделать с оставшимся многочленом

    public:
        //Эти данные относятся к решению, их в файл можно не сохранять
        bool AllRootsFound; //истина, если найдены все корни
        bool Degenerate; //истина, если многочлен вырожденный (0x^n+0x^(n-1)+...+0x+a = b). В этом случае решений или нет (a!=b), или решение - любое число.
        //Эти поля имеют смысл только если Degenerate == false
        TNumeric LinearMultiplier;
        vector<TNumeric> Roots; //корни (соответствуют множителям в разложении многочлена вида x-x_0
        vector<size_t> RootsMultiplicity; //кратность корней
        vector<TPolynom> SquareMods; //квадратичные остатки (содержащие пару комплексных сопряженных корней) (множители вида ax^2+bx+c)
        vector<size_t> SquareModsMultiplicity; //кратность

        vector<TPolynom> Multiplicators; //множители, на которые разложился многочлен (todo: в порядке возрастания их степени); кратность записывается в MMultiplicity
        vector<size_t> MMultiplicity; //кратность множителей

        void SortRoots(); //сортирует Roots совместно с RootsMultiplicity

        TPolynomialEquality(size_t MaxPower = 6);
        TPolynomialEquality(const TPolynom& P, bool AllCoef = true);
        ~TPolynomialEquality();
        virtual string GetTask();
        virtual string GetShortTask();
        virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer);
        bool GetSolution(std::shared_ptr<THTMLWriter> Writer, TPolynom P);
        virtual string GetClassName() { return "TPolynomialEquality";};
        virtual void BuildPhrases();

        void PrintAnswer(std::shared_ptr<THTMLWriter> Writer);

        void ClearSolution();
        void AddRoot(const TNumeric& N, size_t Multiplicity = 1);
        void AddMultiplicator(const TPolynom& P, size_t Multiplicity = 1);
        void CopyAnswer(const TPolynomialEquality* Eq);
        void AddAnswer(const TPolynomialEquality* Eq);
        virtual vector<string> GetKeyWords();


};

class TLinearEquality : public TPolynomialEquality
//ax + b = c
{
    void Assign(const TLinearEquality& L);
public:
    TLinearEquality();
    ~TLinearEquality();
    explicit TLinearEquality(const TPolynom& P);
    explicit TLinearEquality(const TLinearEquality& L);
    void operator=(const TLinearEquality& L) {Assign(L);}

    std::vector<std::shared_ptr<TNumeric>> getEditables() {
        return {};
    }


    virtual void SetLeftPartP(const TPolynom &P, bool unused_flag = false);
    virtual string GetTask();
    virtual string GetShortTask();
    virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer); //возвращает непосредственно само решение без дублирования условия
    virtual void BuildPhrases();

    virtual string GetClassName() { return "TLinearEquality";};
    virtual vector<string> GetKeyWords();

    void Randomize(std::mt19937& rng);
};

class TSquareEquality : public TPolynomialEquality
        //ax^2 + bx + c = 0
{
public:
    TSquareEquality();
    TSquareEquality(TPolynom P); //полином должен быть квадратным, иначе будет исключение
    ~TSquareEquality();
    virtual void SetLeftPartP(const TPolynom &P, bool unused_flag = false);
    virtual string GetTask();
    virtual string GetShortTask();
    virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer);
    virtual void BuildPhrases();

    virtual string GetClassName() { return "TSquareEquality";};
    virtual vector<string> GetKeyWords();

    void Randomize(std::mt19937& rng);
};


//*****************************************************************************************************************************
//              НЕРАВЕНСТВА
//*****************************************************************************************************************************

class TPolynomialInequality : public TPolynomConditions, public TInequality
{

    public:
        bool Less; //Depends on conditions: true, if ax^2 + bx + c < 0; false if ax^2 + bx + c > 0
        bool Strict; //true if inequality is strict
        bool SetType(bool Less, bool Strict);

        TPolynomialInequality(size_t MaxPower = 6, bool Less = true, bool Strict = true);
        TPolynomialInequality(const TPolynom& P, bool Less = true, bool Strict = true, bool AllCoef = true);
        ~TPolynomialInequality();
        virtual string GetTask();
        virtual string GetShortTask();
        virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer);

        virtual std::vector<std::shared_ptr<TNumeric>> GetTypes(std::shared_ptr<const TNumeric> N); //выдаёт все возможные типы задачи
        virtual void SetType(std::shared_ptr<TNumeric> N, size_t Type);

        virtual string GetClassName() { return "TPolynomialInequality";}
        virtual void BuildPhrases();

        virtual void SaveToFile(ofstream &f);
        virtual void LoadFromFile(ifstream &f);
        virtual vector<string> GetKeyWords();

};


class TLinearInequality : public TPolynomialInequality
{
    static const int IDa = 1;
    static const int IDb = 2;
    static const int IDc = 3;
public:
        TLinearInequality(bool Less = true, bool Strict = true);
        TLinearInequality(const TLinearInequality *L, bool Less = true, bool Strict = true); //коэффициенты будут такие же, как в L, а знак неравенства будет определятся флагами Less и Strict
        ~TLinearInequality();
        virtual string GetTask();
        virtual string GetShortTask();
        bool GetSolution(std::shared_ptr<THTMLWriter> Writer);
        virtual string GetClassName() { return "TLinearInequality";};
        virtual void BuildPhrases();
        virtual vector<string> GetKeyWords();

        void Randomize(std::mt19937& rng);
};

class TSquareInequality : public TPolynomialInequality
{
public:
    TSquareInequality(bool Less = true, bool Strict = true);
    ~TSquareInequality();
    TSquareInequality(const TSquareInequality *L, bool Less = true, bool Strict = true); //коэффициенты будут такие же, как в L, а знак неравенства будет определятся флагами Less и Strict
    virtual string GetTask();
    virtual string GetShortTask();
    bool GetSolution(std::shared_ptr<THTMLWriter> Writer);
    virtual string GetClassName() { return "TSquareInequality";};
    virtual void BuildPhrases();
    virtual vector<string> GetKeyWords();

    void Randomize(std::mt19937& rng);
};


//*****************************************************************************************************************************
//              СИСТЕМЫ И СОВОКУПНОСТИ УРАВНЕНИЙ И НЕРАВЕНСТВ
//*****************************************************************************************************************************


template <class TInequalityClass, const char* ClassName>
class TSetOfInequalities : public TProblem, public TInequality
{
public:
    TInequalityClass FirstInequality, SecondInequality;
    TSetOfInequalities();
    ~TSetOfInequalities();
    virtual string GetTask();
    virtual string GetShortTask();
    virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer);
    virtual vector<std::shared_ptr<TNumeric>> GetTypes(std::shared_ptr<const TNumeric> N); //выдаёт все возможные типы задачи, когда кликается по объекту N
    virtual void SetType(std::shared_ptr<TNumeric> N, size_t Type);
    virtual void SaveToFile(ofstream &f)
    {
        this->Conditions->WriteToFile(f);
/*        FirstInequality.SaveToFile(f);
        SecondInequality.SaveToFile(f);*/
    }
    virtual void LoadFromFile(ifstream &f)
    {
        this->Conditions->LoadFromFile(f);
/*        FirstInequality.LoadFromFile(f);
        SecondInequality.LoadFromFile(f);*/
    }
    virtual void BuildPhrases();
    virtual string GetClassName() { return ClassName;};
    virtual vector<string> GetKeyWords();

    void Randomize(std::mt19937& rng);
};

template <class TInequalityClass, const char* ClassName>
class TSystemOfInequalities : public TSetOfInequalities<TInequalityClass, ClassName>
{
//    TInequality FirstInequality, SecondInequality;
public:
    TSystemOfInequalities();
    ~TSystemOfInequalities() {};
    virtual string GetTask() {return MyTranslator.tr("Solve system of inequalities");};
    virtual string GetShortTask() {return MyTranslator.tr("system of inequalities");};
    virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer);
    virtual string GetClassName() { return ClassName;};    
    virtual void BuildPhrases();
    virtual vector<string> GetKeyWords();

//    void Randomize(std::mt19937& rng);
};



template <class TInequalityClass, const char* ClassName>
TSetOfInequalities<TInequalityClass, ClassName>::TSetOfInequalities()
{
    this->Conditions = std::make_shared<TNumeric>();
    this->Conditions->operation = OperatorEqSet;
    this->Conditions->OperandsPushback(*(FirstInequality.Conditions));
    this->Conditions->OperandsPushback(*(SecondInequality.Conditions));
    //this->Solution = 0;
    CanRandomize = true;
    BuildPhrases();
}

template <class TInequalityClass, const char* ClassName>
TSetOfInequalities<TInequalityClass, ClassName>::~TSetOfInequalities()
{    
}

template <class TInequalityClass, const char* ClassName>
string TSetOfInequalities<TInequalityClass, ClassName>::GetTask()
{
    return MyTranslator.tr("Solve set of inequalities");
}

template <class TInequalityClass, const char* ClassName>
string TSetOfInequalities<TInequalityClass, ClassName>::GetShortTask()
{
    return MyTranslator.tr("set of inequalities");
}

template <class TInequalityClass, const char* ClassName>
void TSetOfInequalities<TInequalityClass, ClassName>::BuildPhrases()
{
    if(MyTranslator.CheckDictionary("TSetOfInequalities"))return;
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

template <class TInequalityClass, const char* ClassName>
vector<string> TSetOfInequalities<TInequalityClass, ClassName>::GetKeyWords()
{
    if(MyTranslator.CheckDictionary("TSetOfInequalitiesKeywords") == false)
    {
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



template <class TInequalityClass, const char* ClassName>
bool TSetOfInequalities<TInequalityClass, ClassName>::GetSolution(std::shared_ptr<THTMLWriter> Writer)
{

    FirstInequality.Conditions = std::make_shared<TNumeric>(*(this->Conditions->operands[0]));
    if(Writer)Writer->AddParagraph("Solving first inequality");
    if(Writer)Writer->IncrementNestingLevel();
    bool res1 = this->FirstInequality.GetSolution(Writer);
    if(Writer)Writer->DecrementNestingLevel();
    if(res1 == false) return false;

    SecondInequality.Conditions = std::make_shared<TNumeric>(*(this->Conditions->operands[1]));
    if(Writer)Writer->AddParagraph("Solving second inequality");
    if(Writer)Writer->IncrementNestingLevel();
    bool res2 =  this->SecondInequality.GetSolution(Writer);
    if(Writer)Writer->DecrementNestingLevel();
    if(res2 == false) return false;

    if(Writer)Writer->AddParagraph("Common solution:");
    TIntervalsSet Res;
    Res = this->FirstInequality.Result + this->SecondInequality.Result;    
    if(Writer)Writer->AddFormula(MakeBelongsTo(TNumeric("x"), Res.GetNumeric()));

    TInequality::Result = Res;
    return true;
}


template <class TInequalityClass, const char* ClassName>
vector<std::shared_ptr<TNumeric>> TSetOfInequalities<TInequalityClass, ClassName>::GetTypes(std::shared_ptr<const TNumeric> N)
{
vector<std::shared_ptr<TNumeric>> Res;
    if(*N == *this->Conditions->operands[0])
    {
        FirstInequality.Conditions = std::make_shared<TNumeric>(*N);
        Res = FirstInequality.GetTypes(FirstInequality.Conditions);
    };
    if(*N == *this->Conditions->operands[1])
    {
        SecondInequality.Conditions = std::make_shared<TNumeric>(*N);
        Res = SecondInequality.GetTypes(SecondInequality.Conditions);
    }
    return Res;
}

template <class TInequalityClass, const char* ClassName>
void TSetOfInequalities<TInequalityClass, ClassName>::SetType(std::shared_ptr<TNumeric> N, size_t Type)
{
    if(*N == *Conditions->operands[0])
    {
        FirstInequality.SetType(FirstInequality.Conditions, Type);
        Conditions->operands[0] = std::make_shared<TNumeric>(*FirstInequality.Conditions);
    };
    if(*N == *this->Conditions->operands[1])
    {
        SecondInequality.SetType(SecondInequality.Conditions, Type);
        Conditions->operands[1] = std::make_shared<TNumeric>(*SecondInequality.Conditions);
    }
}

template <class TInequalityClass, const char* ClassName>
void TSetOfInequalities<TInequalityClass, ClassName>::Randomize(std::mt19937& rng)
{
    if(FirstInequality.CanRandomize)
        FirstInequality.Randomize(rng);
    if(SecondInequality.CanRandomize)
        SecondInequality.Randomize(rng);
    this->Conditions->operation = OperatorEqSet;
    this->Conditions->OperandsClear();
    this->Conditions->OperandsPushback(*(FirstInequality.Conditions));
    this->Conditions->OperandsPushback(*(SecondInequality.Conditions));
}


template <class TInequalityClass, const char* ClassName>
TSystemOfInequalities<TInequalityClass, ClassName>::TSystemOfInequalities() : TSetOfInequalities<TInequalityClass, ClassName>()
{
    this->Conditions->operation = OperatorEqSystem;
    BuildPhrases();
};

template <class TInequalityClass, const char* ClassName>
bool TSystemOfInequalities<TInequalityClass, ClassName>::GetSolution(std::shared_ptr<THTMLWriter> Writer)
{
    this->FirstInequality.Conditions = std::make_shared<TNumeric>(*(this->Conditions->operands[0]));
    if(Writer)Writer->AddParagraph("Solving first inequality");
    if(Writer)Writer->IncrementNestingLevel();
    bool res1 = this->FirstInequality.GetSolution(Writer);
    if(Writer)Writer->DecrementNestingLevel();
    if(res1 == false) return false;



    this->SecondInequality.Conditions = std::make_shared<TNumeric>(*(this->Conditions->operands[1]));
    if(Writer)Writer->AddParagraph("Solving second inequality");
    if(Writer)Writer->IncrementNestingLevel();
    bool res2 = this->SecondInequality.GetSolution(Writer);
    if(Writer)Writer->DecrementNestingLevel();
    if(res2 == false) return false;

    if(Writer)Writer->AddParagraph("Common solution:");
    TIntervalsSet Res;
    Res = this->FirstInequality.Result*this->SecondInequality.Result;
    if(Writer)Writer->AddFormula(MakeBelongsTo(TNumeric("x"), Res.GetNumeric()));


    ((TInequality*)this)->Result = Res;
    return true;
}

template <class TInequalityClass, const char* ClassName>
void TSystemOfInequalities<TInequalityClass, ClassName>::BuildPhrases()
{
    if(MyTranslator.CheckDictionary("TSystemOfInequalities"))return;
    MyTranslator.AddDictionary("TSystemOfInequalities");

    MyTranslator.AddEng("Solve system of inequalities");
    MyTranslator.AddRus("Решить систему неравенств");
    MyTranslator.AddEng("system of inequalities");
    MyTranslator.AddRus("система неравенств");
}



template <class TInequalityClass, const char* ClassName>
vector<string> TSystemOfInequalities<TInequalityClass, ClassName>::GetKeyWords()
{
    if(MyTranslator.CheckDictionary("TSystemOfInequalitiesKeywords") == false)
    {
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


extern const char TSetOfLinearInequalitiesStr[];
extern const char TSystemOfLinearInequalitiesStr[];
extern const char TSetOfSquareInequalitiesStr[];
extern const char TSystemOfSquareInequalitiesStr[];
typedef TSetOfInequalities<TLinearInequality, TSetOfLinearInequalitiesStr> TSetOfLinearInequalities;
typedef TSystemOfInequalities<TLinearInequality, TSystemOfLinearInequalitiesStr> TSystemOfLinearInequalities;
typedef TSetOfInequalities<TSquareInequality, TSetOfSquareInequalitiesStr> TSetOfSquareInequalities;
typedef TSystemOfInequalities<TSquareInequality, TSystemOfSquareInequalitiesStr> TSystemOfSquareInequalities;

//*****************************************************************************************************************************
//              ПРОИЗВОДНЫЕ
//*****************************************************************************************************************************
class TPolynomDerivative : public TPolynomConditions
{
public:
    TPolynomDerivative(size_t MaxPower = 8);
    ~TPolynomDerivative();

    virtual string GetTask();
    virtual string GetShortTask();
    virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer);

    virtual string GetClassName() { return "TPolynomDerivative";};
    virtual void BuildPhrases();
    virtual vector<string> GetKeyWords();

    void Randomize(std::mt19937& rng);
};

class TRationalFunctionConditions : public TProblem
{
    int RightPartID() const {return MaxPowerDenominator + MaxPowerNominator + 2; };
    bool HaveRightPart; //истина, если необходимо добавлять в Conditions правую часть
    //void Assign(const TRationalFunctionConditions& R);
    TNumeric GetVarPower(size_t power); //возвращает UnknownVar^power
public:
    TNumeric UnknownVar; //обозначение неизвестной переменной
    int operation;
    size_t MaxPowerNominator, MaxPowerDenominator;
    TRationalFunctionConditions(int operation, bool HaveRightPart = true, size_t MaxPowerNominator = 4, size_t MaxPowerDenominator = 4);
    ~TRationalFunctionConditions();

    //void operation=(const TRationalFunctionConditions& R);

    TNumeric *GetNominator();
    TNumeric *GetDenominator();

    TPolynom GetNominatorP();
    TPolynom GetDenominatorP();

    TNumeric& GetRightPartP();
    TNumeric GetRightPart() const;
    void SetRightPart(const TNumeric& R);

    void SetNominator(const TPolynom &P, bool AllCoef = true); //Если AllCoef = true, то будут учитываться все коэфициенты, если false - то нулевые коэфициенты старше MajorPower будут отброшены
    void SetDenominator(const TPolynom &P, bool AllCoef = true); //Если AllCoef = true, то будут учитываться все коэфициенты, если false - то нулевые коэфициенты старше MajorPower будут отброшены

    size_t GetNominatorCoefID(size_t power);
    size_t GetDenominatorCoefID(size_t power);

    void SetMaxPower(size_t MaxPowerNominator, size_t MaxPowerDenominator);
    //virtual vector<string> GetKeyWords();


    virtual void SaveToFile(ofstream &f);
    virtual void LoadFromFile(ifstream &f);

    void Randomize(std::mt19937& rng);
};


class TRationalFunctionDerivative : public TRationalFunctionConditions
{
public:
    TRationalFunctionDerivative(size_t MaxPowerNominator = 8, size_t MaxPowerDenominator = 8);
    ~TRationalFunctionDerivative();

    virtual string GetTask();
    virtual string GetShortTask();
    virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer);
    virtual string GetClassName() { return "TRationalFunctionDerivative";};
    virtual void BuildPhrases();

    virtual void SaveToFile(ofstream &f);
    virtual void LoadFromFile(ifstream &f);    
    virtual vector<string> GetKeyWords();
};


#endif // TPOLYNOMDERIVATIVE_H
