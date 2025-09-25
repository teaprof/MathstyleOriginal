#ifndef POLYNOM_H
#define POLYNOM_H
#include <vector>

#include "arithmetic.h"

class TPolynom
// Полином с действительными коэффициентами
{
    void Assign(const TPolynom& P);
    /// удаляет старшие нулевые коэффициенты
    void RemoveMajorZeros();  /// находит максимальный общий множитель у всех коэффициентов
    TNumeric GetX0Candidate();
    size_t GetX2Candidates(vector<int>& a, vector<int>& b, vector<int>& c);

    /// определяет, сколько раз многочлен делится на Divisor
    /// возвращает многочлен после деления на Divisor^Multiplicty
    TPolynom GetMultiplicity(const TPolynom& Divisor, size_t& Multiplicity) const;

    /// из множества X находит такие X, которые являются корнями многочлена
    /// записывает в Res одночлены x - x0, кратность корней - в Multiplicities, и возвращает многочлен, полученный делением
    /// исходного на найденные (x-x0) с учётом их кратности
    TPolynom
        TestX1(const vector<int>& Num, const vector<int>& Denom, vector<TPolynom>& Res, vector<size_t>& Multiplicities) const;

    /// ищет делители вида ax^2+bx+c
    TPolynom TestX2(const vector<int>& a,
                    const vector<int>& b,
                    const vector<int>& c,
                    vector<TPolynom>& Res,
                    vector<size_t>& Multiplicities) const;

  public:
    /// Coef[0] + Coef[1]*x + Coef[2]*x^2 + ... + Coef[n]*x^n
    /// если Coef.size() == 0, то полином тождественно равен нулю
    /// Вместо Coef[i] и Coef.size() лучше всего использовать GetCoef(i) и MajorPower()
    std::vector<PNumeric> Coef;

    PNumeric& GetCoef(size_t Power);
    PNumeric GetCoef(size_t Power) const;
    TPolynom();
    TPolynom(const TPolynom& P);
    TPolynom(const std::vector<PNumeric>& Coef);
    TPolynom(std::vector<TNumeric>&& Coef);
    TPolynom(const std::vector<int>& Coef);
    TPolynom(const TNumeric& a);                                        // creates ax^0
    TPolynom(const TNumeric& a, const TNumeric& b);                     // creates ax+b
    TPolynom(const TNumeric& a, const TNumeric& b, const TNumeric& c);  // creates ax^2+bx+c
    virtual ~TPolynom();

    /// the following function generate TNumeric representation of this polynom
    TNumeric asNumeric() const;                                   // x^k + x^(k-1) + ... + x^0
    TNumeric asNumeric(const TNumeric& UnknownVar) const;         // x^k + x^(k-1) + ... + x^0
    TNumeric GetSeriesNumeric(const TNumeric& UnknownVar) const;  // x^0 + x^1 + x^2 +...

    bool operator==(const TPolynom& P) const;
    TPolynom operator=(const TNumeric& N);  // => P = const
    TPolynom operator=(const TPolynom& P);
    TPolynom operator+(const TPolynom& P);
    TPolynom operator-(const TPolynom& P);
    TPolynom operator*(const TPolynom& P);
    TPolynom operator/(const TPolynom& P);
    TPolynom operator%(const TPolynom& P);  // остаток от деления
    TPolynom operator^(size_t Power);

    bool IsZero() const;  // истина, если полином - тождественный нулю

    TPolynom operator*(const TNumeric& P);
    TPolynom operator/(const TNumeric& P);
    /// выполняет целочисленное деление, то есть находит главную часть от деления
    TPolynom Div(const TPolynom& P, TPolynom* Remainder) const;

    size_t MajorPower() const;  // возвращает коэфициент при старшей степени (он не обязательно равен последнему элементу Coef,
                                // так как последний элемент Coef может быть равным нулю)

    TNumeric Calculate(const TNumeric& x) const;  // вычисляет значение полинома в точке
    vector<double> Calculate(const vector<double> X) const;

    TPolynom Derivative();
    vector<TPolynom> Factorize() const;  // факторизация возможна только для рациональных коэффициентов.

    void SaveToFile(ofstream& f);
    void LoadFromFile(ifstream& f);

    vector<TPolynom> FactorizeKroneker(vector<size_t>* Multiplicity = 0) const;  // Факторизация методом Кронекера
    // Если Multiplicity = NULL, то все делители добавляются в res столько раз, какова их кратноть
    // Иначе все делители добавляются только один раз, а в Multiplicity - их кратность
};

/// сокращает многочлены на общие множители; возвращает истину, если хоть что-то удалось сократить
bool PCancel(const TPolynom& P1, const TPolynom& P2, TPolynom* Res1, TPolynom* Res2);

/// строит полином с целыми коэффициентами, проходящий через точки (X(i), Y(i))
bool BuildIntegerPolynom(const vector<TNumeric>& X, const vector<TNumeric>& Y, TPolynom& P);

/// строит полином с целыми коэффициентами, проходящий через точки (X(i), Y(i))
bool BuildIntegerPolynom(const vector<int>& X, const vector<int>& Y, TPolynom& P);

/// функция вида P(x)/Q(x)
class TRationalFunction {
    // void Assign(const TRationalFunction& R);
  public:
    TPolynom P, Q;
    TRationalFunction();
    TRationalFunction(const TPolynom P, const TPolynom Q);
    TRationalFunction(const std::vector<PNumeric>& PCoef, const std::vector<PNumeric>& QCoef);
    TRationalFunction(std::vector<TNumeric>&& PCoef, std::vector<TNumeric>&& QCoef);
    ~TRationalFunction();

    TNumeric asNumeric() const;
    // TRationalFunction operator=(const TRationalFunction& R);
    TRationalFunction operator+(const TRationalFunction& R);
    TRationalFunction operator-(const TRationalFunction& R);
    TRationalFunction operator*(const TRationalFunction& R);
    TRationalFunction operator/(const TRationalFunction& R);

    TNumeric asNumeric(const TNumeric& UnknownVar) {
        return MakeFrac(P.asNumeric(UnknownVar), Q.asNumeric(UnknownVar));
    }

    const TPolynom& Numerator() const {
        return P;
    }
    const TPolynom& Denominator() const {
        return Q;
    }
    TPolynom& Numerator() {
        return P;
    }
    TPolynom& Denominator() {
        return Q;
    }

    TNumeric Calculate(const TNumeric& X);
    TRationalFunction Derivative();

    // представляет рац функцию в форме Pn(x) + P'm(x)/Rk(x), m<k (O - "о" большое)s
    TNumeric GetMainPartAndO();

    // сократить числитель и знаменатель на общие множители
    void reduce();
};

// vector<TNumeric> PolyDiv(vector<TNumeric> Numerator, vector<TNumeric> Denominator, vector<TNumeric> *Reminder);

#endif  // POLYNOM_H
