#ifndef TPOLYNOMDERIVATIVE_H
#define TPOLYNOMDERIVATIVE_H

#include "Base/polynom.h"
#include "tproblem.h"

extern TNumeric X_1;
extern TNumeric X_2;
extern TNumeric EmptySet;
extern TNumeric NumericAllReal;
extern TInterval IntervalAllRealNumbers;
extern TNumeric NumPhi;

class TPolynomConditions : public TProblem {
    /// истина, если необходимо добавлять в Conditions правую часть
    bool HaveRightPart;  /// \todo: remove?
  public:
    static constexpr int RightPartID = 100000;
    static constexpr int RolePowerBegin = 100;

    /// обозначение неизвестной переменной
    TNumeric UnknownVar;
    size_t MaxPower;
    TPolynomConditions(size_t MaxPower = 8, bool HaveRightPart = true, Operation operation = OperatorEqual);
    // Если AllCoef = true, то будут учитываться все коэфициенты, если false - то нулевые коэфициенты старше MaxPower будут отброшены
    TPolynomConditions(const TPolynom& P, bool HaveRightPart = true, Operation operation = OperatorEqual, bool AllCoef = true);
    ~TPolynomConditions();

    std::vector<std::shared_ptr<const TNumeric>> GetCoef() const;
    std::vector<std::shared_ptr<TNumeric>> GetCoef();
    TPolynom asPolynom() const;
    std::shared_ptr<TNumeric>& GetCoefP(size_t power);
    std::shared_ptr<const TNumeric> GetCoefP(size_t power) const;
    std::shared_ptr<TNumeric>& GetRightPartP();
    TNumeric GetRightPart() const;

    std::vector<std::shared_ptr<TNumeric>> getEditables() {
        std::vector<std::shared_ptr<TNumeric>> res;
        for (size_t n = 0; n <= MaxPower; n++)
            res.push_back(GetCoefP(n));
        return res;
    }

    // Обновляет this->conditions. Если AllCoef = true, то будут учитываться все коэфициенты,
    // если false - то нулевые коэфициенты старше MaxPower будут отброшены.
    virtual void SetLeftPartP(const TPolynom& P, bool AllCoef = true);
    // ets left part to ax+b and update this->conditions
    virtual void SetLeftPart(const TNumeric& a, const TNumeric& b);
    // set left part to ax^2+bx+c and update this->conditions
    virtual void SetLeftPart(const TNumeric& a, const TNumeric& b, const TNumeric& c);
    // устанавливает максимальную степень многочлена MaxPower
    virtual void SetMaxPower(size_t MaxPower, Operation comparison_operation);

    void SetRightPart(const TNumeric& N);

    // меняет Conditions, если они есть
    void SetUnknownVar(TNumeric UnknownVar);

    virtual void SaveToFile(ofstream& f);
    virtual void LoadFromFile(ifstream& f);

    // возвращает UnknownVar^power
    TNumeric GetVarPower(size_t power);

    void Randomize(std::mt19937& rng);
};

class TPolynomialEquality : public TPolynomConditions, public TEquality {
    /// проверяет, чтобы все коэффициенты в многочлене были рациональными и вычисляет НОК знаменателей
    bool CheckRationalAndGetNOK(std::shared_ptr<THTMLWriter> Writer, const TPolynom& P, int& NOK);
    /// умножает все коэффициенты многочлена на NOK, после чего они должны стать целыми, и записывает их в IntCoefs
    bool GetIntCoefs(std::shared_ptr<THTMLWriter> Writer, int NOK, const TPolynom& P, vector<int>& IntCoefs);
    /// выносит общий множитель
    int factorOutCommonMultiplicator(std::shared_ptr<THTMLWriter> Writer, vector<int>& Coefs);
    /// выносит общий множитель вида x^k
    bool factorOutXk(std::shared_ptr<THTMLWriter> Writer, vector<int>& Coefs);
    /// ищет корни среди рациональных корней
    /// PRemaining - оставшийся многочлен после выноса рациональных корней
    bool SearchRationalRoots(std::shared_ptr<THTMLWriter> Writer, vector<int>& Coefs, TPolynom& PRemaining);

    /// пытается что-то еще сделать с оставшимся многочленом
    bool AnalyzePRemaining(std::shared_ptr<THTMLWriter> Writer, const TPolynom& PRemaining);

  public:
    // Эти данные относятся к решению, их в файл можно не сохранять
    /// истина, если найдены все корни
    bool AllRootsFound;
    /// истина, если многочлен вырожденный (0x^n+0x^(n-1)+...+0x+a = b). В этом случае решений или нет (a!=b), или решение - любое
    /// число.
    bool Degenerate;
    // Эти поля имеют смысл только если Degenerate == false:
    PNumeric LinearMultiplier;
    // корни (соответствуют множителям в разложении многочлена вида x-x_0
    vector<PNumeric> Roots;
    // кратность корней
    vector<size_t> RootsMultiplicity;

    // квадратичные остатки (содержащие пару комплексных сопряженных корней) (множители вида ax^2+bx+c)
    vector<TPolynom> SquareMods;
    // кратность
    vector<size_t> SquareModsMultiplicity;

    // множители, на которые разложился многочлен (todo: в порядке возрастания их степени); кратность записывается в MMultiplicity
    vector<TPolynom> Multiplicators;
    // кратность множителей
    std::vector<size_t> MMultiplicity;

    // сортирует Roots совместно с RootsMultiplicity
    void SortRoots();

    TPolynomialEquality(size_t MaxPower = 6);
    TPolynomialEquality(const TPolynom& P, bool AllCoef = true);
    ~TPolynomialEquality();
    virtual string GetTask();
    virtual string GetShortTask();
    virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer);
    bool GetSolution(std::shared_ptr<THTMLWriter> Writer, TPolynom P);
    virtual string GetClassName() {
        return "TPolynomialEquality";
    };
    virtual void BuildPhrases();

    void PrintAnswer(std::shared_ptr<THTMLWriter> Writer);

    void ClearSolution();
    void AddRoot(const TNumeric& N, size_t Multiplicity = 1);
    void AddMultiplicator(const TPolynom& P, size_t Multiplicity = 1);
    void CopyAnswer(const TPolynomialEquality* Eq);
    void AddAnswer(const TPolynomialEquality* Eq);
    virtual vector<string> GetKeyWords();
};

class TLinearEquality :
    public TPolynomialEquality
// ax + b = c
{
    void Assign(const TLinearEquality& L);

  public:
    TLinearEquality();
    ~TLinearEquality();
    explicit TLinearEquality(const TPolynom& P);
    explicit TLinearEquality(const TLinearEquality& L);
    void operator=(const TLinearEquality& L) {
        Assign(L);
    }
    virtual void SetLeftPartP(const TPolynom& P, bool unused_flag = false);
    virtual string GetTask();
    virtual string GetShortTask();
    /// возвращает непосредственно само решение без дублирования условия
    virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer);
    virtual void BuildPhrases();

    virtual string GetClassName() {
        return "TLinearEquality";
    };
    virtual vector<string> GetKeyWords();

    void Randomize(std::mt19937& rng);
};

class TSquareEquality :
    public TPolynomialEquality
// ax^2 + bx + c = 0
{
  public:
    TSquareEquality();
    /// полином должен быть квадратным, иначе будет исключение
    TSquareEquality(TPolynom P);
    ~TSquareEquality();
    virtual void SetLeftPartP(const TPolynom& P, bool unused_flag = false);
    virtual string GetTask();
    virtual string GetShortTask();
    virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer);
    virtual void BuildPhrases();

    virtual string GetClassName() {
        return "TSquareEquality";
    };
    virtual vector<string> GetKeyWords();

    void Randomize(std::mt19937& rng);
};

//*****************************************************************************************************************************
//              НЕРАВЕНСТВА
//*****************************************************************************************************************************

class TPolynomialInequality : public TPolynomConditions, public TInequality {
  public:
    // Depends on conditions: true, if ax^2 + bx + c < 0; false if ax^2 + bx + c > 0
    bool Less;
    /// true if inequality is strict
    bool Strict;
    bool SetType(bool Less, bool Strict);

    TPolynomialInequality(size_t MaxPower = 6, bool Less = true, bool Strict = true);
    TPolynomialInequality(const TPolynom& P, bool Less = true, bool Strict = true, bool AllCoef = true);
    ~TPolynomialInequality();
    virtual string GetTask();
    virtual string GetShortTask();
    virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer);

    virtual std::vector<std::shared_ptr<TNumeric>> GetTypes(std::shared_ptr<const TNumeric> N);  // выдаёт все возможные типы задачи
    virtual void SetType(std::shared_ptr<TNumeric> N, size_t Type);

    virtual string GetClassName() {
        return "TPolynomialInequality";
    }
    virtual void BuildPhrases();

    virtual void SaveToFile(ofstream& f);
    virtual void LoadFromFile(ifstream& f);
    virtual vector<string> GetKeyWords();
};

class TLinearInequality : public TPolynomialInequality {
    static const int IDa = 1;
    static const int IDb = 2;
    static const int IDc = 3;

  public:
    TLinearInequality(bool Less = true, bool Strict = true);
    /// коэффициенты будут такие же, как в L, а знак неравенства будет определятся флагами Less и Strict
    TLinearInequality(const TLinearInequality* L, bool Less = true, bool Strict = true);
    ~TLinearInequality();
    virtual string GetTask();
    virtual string GetShortTask();
    bool GetSolution(std::shared_ptr<THTMLWriter> Writer);
    virtual string GetClassName() {
        return "TLinearInequality";
    };
    virtual void BuildPhrases();
    virtual vector<string> GetKeyWords();

    void Randomize(std::mt19937& rng);
};

class TSquareInequality : public TPolynomialInequality {
  public:
    TSquareInequality(bool Less = true, bool Strict = true);
    ~TSquareInequality();
    /// коэффициенты будут такие же, как в L, а знак неравенства будет определятся флагами Less и Strict
    TSquareInequality(const TSquareInequality* L, bool Less = true, bool Strict = true);
    virtual string GetTask();
    virtual string GetShortTask();
    bool GetSolution(std::shared_ptr<THTMLWriter> Writer);
    virtual string GetClassName() {
        return "TSquareInequality";
    };
    virtual void BuildPhrases();
    virtual vector<string> GetKeyWords();

    void Randomize(std::mt19937& rng);
};

//*****************************************************************************************************************************
//              СИСТЕМЫ И СОВОКУПНОСТИ УРАВНЕНИЙ И НЕРАВЕНСТВ
//*****************************************************************************************************************************

template<class TInequalityClass, const char* ClassName>
class TSetOfInequalities : public TProblem, public TInequality {
  public:
    TInequalityClass FirstInequality, SecondInequality;
    TSetOfInequalities();
    ~TSetOfInequalities();
    virtual string GetTask();
    virtual string GetShortTask();
    virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer);
    /// выдаёт все возможные типы задачи, когда кликается по объекту N
    virtual vector<std::shared_ptr<TNumeric>> GetTypes(std::shared_ptr<const TNumeric> N);
    virtual void SetType(std::shared_ptr<TNumeric> N, size_t Type);
    virtual void SaveToFile(ofstream& f);
    virtual void LoadFromFile(ifstream& f);
    virtual void BuildPhrases();
    virtual string GetClassName() {
        return ClassName;
    };
    virtual vector<string> GetKeyWords();

    void Randomize(std::mt19937& rng);
};

template<class TInequalityClass, const char* ClassName>
class TSystemOfInequalities : public TSetOfInequalities<TInequalityClass, ClassName> {
    //    TInequality FirstInequality, SecondInequality;
  public:
    TSystemOfInequalities();
    ~TSystemOfInequalities(){};
    virtual string GetTask() {
        return MyTranslator.tr("Solve system of inequalities");
    };
    virtual string GetShortTask() {
        return MyTranslator.tr("system of inequalities");
    };
    virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer);
    virtual string GetClassName() {
        return ClassName;
    };
    virtual void BuildPhrases();
    virtual vector<string> GetKeyWords();
};

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
class TPolynomDerivative : public TPolynomConditions {
  public:
    TPolynomDerivative(size_t MaxPower = 8);
    ~TPolynomDerivative();

    virtual string GetTask();
    virtual string GetShortTask();
    virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer);

    virtual string GetClassName() {
        return "TPolynomDerivative";
    };
    virtual void BuildPhrases();
    virtual vector<string> GetKeyWords();

    void Randomize(std::mt19937& rng);
};

class TRationalFunctionConditions : public TProblem {
    int RightPartID() const {
        return MaxPowerDenominator + MaxPowerNumerator + 2;
    };
    bool HaveRightPart;  // истина, если необходимо добавлять в Conditions правую часть
                         // void Assign(const TRationalFunctionConditions& R);

    TNumeric GetVarPower(size_t power);

  public:
    TNumeric UnknownVar;  // обозначение неизвестной переменной
    Operation operation;
    size_t MaxPowerNumerator, MaxPowerDenominator;
    TRationalFunctionConditions(Operation operation, bool HaveRightPart = true, size_t MaxPowerNumerator = 4, size_t MaxPowerDenominator = 4);
    ~TRationalFunctionConditions();

    // void operation=(const TRationalFunctionConditions& R);

    PNumeric GetNumerator();
    PNumeric GetDenominator();

    TPolynom GetNumeratorP();
    TPolynom GetDenominatorP();

    TNumeric& GetRightPartP();
    TNumeric GetRightPart() const;
    void SetRightPart(std::shared_ptr<TNumeric> R);

    /// Если AllCoef = true, то будут учитываться все коэфициенты, если false - то нулевые коэфициенты старше MajorPower будут отброшены
    void SetNumerator(const TPolynom& P, bool AllCoef = true);

    // Если AllCoef = true, то будут учитываться все коэфициенты, если false - то нулевые коэфициенты старше MajorPower будут отброшены
    void SetDenominator(const TPolynom& P, bool AllCoef = true);

    size_t GetNumeratorCoefID(size_t power);
    size_t GetDenominatorCoefID(size_t power);

    void SetMaxPower(size_t MaxPowerNumerator, size_t MaxPowerDenominator);
    // virtual vector<string> GetKeyWords();

    virtual void SaveToFile(ofstream& f);
    virtual void LoadFromFile(ifstream& f);

    void Randomize(std::mt19937& rng);
};

class TRationalFunctionDerivative : public TRationalFunctionConditions {
  public:
    TRationalFunctionDerivative(size_t MaxPowerNumerator = 8, size_t MaxPowerDenominator = 8);
    ~TRationalFunctionDerivative();

    virtual string GetTask();
    virtual string GetShortTask();
    virtual bool GetSolution(std::shared_ptr<THTMLWriter> Writer);
    virtual string GetClassName() {
        return "TRationalFunctionDerivative";
    };
    virtual void BuildPhrases();

    virtual void SaveToFile(ofstream& f);
    virtual void LoadFromFile(ifstream& f);
    virtual vector<string> GetKeyWords();
};

#endif  // TPOLYNOMDERIVATIVE_H
