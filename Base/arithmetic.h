//---------------------------------------------------------------------------
// todo: упрощение выражений вида a^log(b, a) => exp(ln(b)/ln(a)*ln(a)) = exp(ln(b)) = b
#ifndef arithmeticH
#define arithmeticH

#define TCanvas QPainter
#define string string
#define NaN NAN
#define Infinity INFINITY

#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

using namespace std;

enum Operation {
    OperatorEqual = 7,
    OperatorConst = 0,
    OperatorSum = 1,
    OperatorMinus = 6,
    OperatorProd = 2,
    OperatorSqrt = 3,
    OperatorPow = 4,
    OperatorFrac = 5,

    OperatorEqSet = 8,     // совокупность уравнений
    OperatorEqSystem = 9,  // система уравнений

    OperatorBelongsTo = 11,
    OperatorInterval = 12,         // (x1, x2)
    OperatorSegmentInterval = 13,  //[x1, x2)
    OperatorIntervalSegment = 14,  //(x1, x2]
    OperatorSegment = 15,          //[x1, x2]

    OperatorInline = 16,  // все операнды в одну строчку /// \wtf?

    OperatorUnion = 18,
    OperatorIntersection = 19,

    OperatorLessOrEqual = 20,
    OperatorLess = 21,
    OperatorGreater = 22,
    OperatorGreaterOrEqual = 23,

    OperatorSubIndex = 24,  /// \todo: wtf? this is not an arithmetic operator
    OperatorSupIndex = 25,  /// \todo: wtf? this is not an arithmetic operator

    OperatorDeriv = 26,

    OperatorSin = 27,
    OperatorCos = 28,
    OperatorTg = 29,
    OperatorCtg = 30,

    OperatorArcsin = 31,
    OperatorArccos = 32,
    OperatorArctg = 33,

    OperatorLog = 34,  // logarithm of Operands[0] with base of Operands[1] = ln(Operands[0])/ln(Operands[1])
    OperatorLn = 35,   // natural logarithm
    OperatorLg = 36,   // decimal logarithm

    OperatorPlusMinus = 37,
    //  OperatorModule = 38,
    OperatorExp = 39,
    OperatorIntegral = 40,  //\int Operands[0] dOperands[1]
    OperatorAbs = 41,       // modulus
    OperatorSign = 42,      // signum

    OperatorSmallO = 43,  // o(x^n)
    OperatorBigO = 44,    // O(x^n)

    OperatorSh = 45,
    OperatorCh = 46
};

int GetOperatorPriority(Operation OpCode);
int CompareOperatorsPriority(Operation OpCode1, Operation OpCode2);

class TNumeric : public std::enable_shared_from_this<TNumeric> {
    /// упрощает выражение, где последним оператором является тригонометрическая функция
    TNumeric SimplifyTrig() const;
    /// упрощает выражение, где последним оператором является обратная тригонометрическая функция
    TNumeric SimplifyInverseTrig() const;
    /// упрощает выражение, где последним оператором является Log(A, B)
    TNumeric SimplifyLog() const;
    /// если это возможно, представляет объект в виде рациональной дроби; если не возможно, то false
    bool asRational(int& Numerator, int& Denominator) const;
    /// упрощает функции везде, где возможно
    TNumeric SimplifyFunctions() const;

    /// интерфейс к mathomatic
    TNumeric MathoCmd(const string& Cmd) const;

    /// проверяет, был ли чем-нибудь инициализирован объект ранее; если не был, то вызывает исключение
    void CheckInitialized() const;

    /// make TNumeric constructos accessible from this class only (see enable_shared_from_this documentation)
    struct Private {
        explicit Private() = default;
    };

  public:
    /// идентификатор, по которому можно получить указатель на объект TNumeric
    int role{-1};
    /// представление в виде строки (доступно только если operation == Operator::Const) /// \todo: проверить
    string strval;
    /// код операции
    Operation operation;
    /// операнды
    vector<std::shared_ptr<TNumeric>> operands;
    std::shared_ptr<TNumeric>& at(size_t index) {
        return operands[index];
    }

    void OperandsPushback(const TNumeric& Val);
    void OperandsPushback(TNumeric&& Val);
    void OperandsPushback(std::shared_ptr<TNumeric> val);
    void OperandsClear();

    TNumeric() = default;
    /// initialize operands with the same pointers as N.operands
    TNumeric(const TNumeric& N) = default;
    TNumeric(TNumeric&& numeric) = default;
    TNumeric& operator=(const TNumeric& N) = default;
    TNumeric& operator=(TNumeric&& N) = default;
    virtual ~TNumeric();

    explicit TNumeric(double value);
    explicit TNumeric(int value);
    explicit TNumeric(const std::string& value);

    TNumeric deepCopy() const;
    std::shared_ptr<TNumeric> deepCopyPtr() const;

    template<class... Args>
    static std::shared_ptr<TNumeric> create(Args... args) {
        return std::make_shared<TNumeric>(args...);
    }

    /// returns true if object is like "x" or "x_{something}" (object designates variable)
    bool IsVariable() const;
    /// Если *this и N совпадают полностью друг с другом
    bool IsEqual(const TNumeric& N) const;
    /// Если variable хоть как-то входит в выражение, то true
    bool DependsOn(const char* variable) const;
    /// Если *this - N символьно равно нулю, то true
    bool operator==(const TNumeric& N) const;
    bool operator!=(const TNumeric& N) const;
    bool operator==(double Value) const;
    bool operator!=(double Value) const;

    TNumeric operator*(const TNumeric& N) const;
    TNumeric operator*(int N) const {
        return this->operator*(TNumeric(N));
    }
    TNumeric operator+(const TNumeric& N) const;
    TNumeric operator+(int N) const {
        return this->operator+(TNumeric(N));
    }
    TNumeric operator-(const TNumeric& N) const;
    TNumeric operator-(int N) const {
        return this->operator-(TNumeric(N));
    }
    TNumeric operator-() const;
    TNumeric operator/(const TNumeric& N) const;
    TNumeric operator/(int N) const {
        return this->operator/(TNumeric(N));
    }
    TNumeric sqrt() const;
    TNumeric operator^(const TNumeric& N) const;
    TNumeric operator^(int N) const {
        return this->operator^(TNumeric(N));
    }

    TNumeric Derivative(const string VarName = "x") const;

    /// Создает совокупоность из двух уравнений: N1 | N2
    void MakeEqSet(const TNumeric& N1, const TNumeric& N2);
    /// Создает систему из двух уравнений: N1 & N2
    void MakeEqSystem(const TNumeric& N1, const TNumeric& N2);

    void AssignV(const char* str);
    void Assign(char* str);

    string CodeBasic() const;
    /// упрощает представление формулы (заменяет a^(1/2) на sqrt(a))
    TNumeric SimplifyPresentation() const;

    double Calculate() const;
    TNumeric Substitute(const string& var, const TNumeric& Val) const;

    bool hasRole(int role) const;
    std::shared_ptr<const TNumeric> GetByRoleConst(int role) const;
    std::shared_ptr<TNumeric>& GetByRole(int role, std::shared_ptr<TNumeric>& self);
    TNumeric replaceByRole(int role, TNumeric&& N);

    TNumeric Simplify() const;
    TNumeric Unfactor() const;

    /// true, если выражение является арифметическим выражением и является полным, то есть не содержит пустых прямоугольников
    bool CanCalculate() const;

    /// возвращает true, если коэффициент является целым числом. Число записывает в Int
    bool isInteger(int* Int) const;
    bool isInteger() const;

    /// обозначаем функции, которых нет в пакете mathomatic, за новые переменные a_i; первая переменная получает номер StartID;
    /// StartID постепенно увеличивается
    TNumeric EliminateUnimplementedFunctions(size_t& StartID, map<string, TNumeric>& vars_to_functions) const;
    /// заменяем переменные обратно на функции
    TNumeric RestoreUnimplementedFunctions(const map<string, TNumeric>& vars_to_functions) const;

    void LoadFromFile(ifstream& f);
    void SaveToFile(ofstream& f);
};

using PNumeric = std::shared_ptr<TNumeric>;

string DeleteExternalBrackets(string q);
TNumeric MakeEquality(const TNumeric& N1, const TNumeric& N2);
TNumeric MakeBelongsTo(const TNumeric& N1, const TNumeric& N2);
TNumeric MakeInterval(const TNumeric& X1, const TNumeric& X2, bool includeleft = false, bool includeright = false);
TNumeric MakeInline(const TNumeric& N1, const TNumeric& N2);
TNumeric MakeInline(const TNumeric& N1, const TNumeric& N2, const TNumeric& N3);
TNumeric MakeUnion(const TNumeric& N1, const TNumeric& N2);
TNumeric MakeSubscript(const TNumeric& N1, const TNumeric& N2);
TNumeric MakeSuperscript(const TNumeric& N1, const TNumeric& N2);
TNumeric MakePow(const TNumeric& N1, const TNumeric& N2);
TNumeric MakeExp(const TNumeric& N);
TNumeric MakeCh(const TNumeric& N);
TNumeric MakeSh(const TNumeric& N);
TNumeric MakeSqrt(const TNumeric& N);

TNumeric MakeFrac(const TNumeric& N1, const TNumeric& N2);
TNumeric MakeProd(TNumeric&& N1, TNumeric&& N2);
TNumeric MakeLn(const TNumeric& N1);
TNumeric MakeLog(const TNumeric& N, const TNumeric& Base);

TNumeric MakeCos(const TNumeric& N);
TNumeric MakeSin(const TNumeric& N);
TNumeric MakeTg(const TNumeric& N);
TNumeric MakeCtg(const TNumeric& N);
TNumeric MakeArccos(const TNumeric& N);
TNumeric MakeArcsin(const TNumeric& N);
TNumeric MakeArctg(const TNumeric& N);
TNumeric MakeAbs(const TNumeric& N);
TNumeric MakeSign(const TNumeric& N);
TNumeric MakeBigO(const TNumeric& N);
TNumeric MakeSmallO(const TNumeric& N);

TNumeric MakePlusMinus(const TNumeric& N);

TNumeric MakeSystemOfEquations(const TNumeric& N1, const TNumeric& N2);
TNumeric MakeSetOfEquations(const TNumeric& N1, const TNumeric& N2);

TNumeric MakeIntegral(const TNumeric& N, const TNumeric& dx);
TNumeric MakeIntegral(const TNumeric& N, const string& dx);

TNumeric GetPolynom(
    size_t Power,
    size_t StartID);  // конструирует многочлен с нулевыми коэффициентами. role устанавливаются от StartID до StartID+Power

//---------------------------------------------------------------------------
std::shared_ptr<const TNumeric> FindParent(std::shared_ptr<const TNumeric> Root, std::shared_ptr<const TNumeric> Child);
bool CanErase(std::shared_ptr<const TNumeric> Root, std::shared_ptr<const TNumeric>* WhatToDelete);
// std::shared_ptr<const TNumeric> EraseNumeric(std::shared_ptr<const TNumeric> Root, std::shared_ptr<const TNumeric>
// WhatToDelete);
//---------------------------------------------------------------------------

extern TNumeric NumPi;
extern TNumeric NumPi2;
extern TNumeric NumPi4;
extern TNumeric NumPi3;
extern TNumeric NumPi6;
extern TNumeric Num3Pi4;
extern TNumeric Num2Pi3;
extern TNumeric Num5Pi6;
extern TNumeric Num2Pi3;

//---------------------------------------------------------------------------
#endif
