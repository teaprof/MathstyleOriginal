#ifndef POLYNOM_H
#define POLYNOM_H
#include<vector>
#include "arithmetic.h"

class TPolynom
//Полином с действительными коэффициентами
{
        void Assign(const TPolynom &P);
        void RemoveZeros(); //удаляет старшие нулевые коэффициенты
        TNumeric GetX0Candidate(); //находит максимальный общий множитель у всех коэффициентов
        size_t GetX2Candidates(vector<int> &a, vector<int> &b, vector<int> &c);

        TPolynom GetMultiplicity(TPolynom Divisor, size_t& Multiplicity) const;
        TPolynom TestX1(vector<int> Nom, vector<int> Denom, vector<TPolynom> &Res, vector<size_t> &Multiplicities) const;
        TPolynom TestX2(vector<int> a, vector<int> b, vector<int> c, vector<TPolynom> &Res, vector<size_t> &Multiplicities) const;

    public:        
        std::vector<TNumeric> Coef; //Coef[0] + Coef[1]*x + Coef[2]*x^2 + ... + Coef[n]*x^n
        //если Coef.size() == 0, то полином тождественно равен нулю
        //Вместо Coef[i] и Coef.size() лучше всего использовать GetCoef(i) и MajorPower()

        TNumeric& GetCoef(size_t Power);
        TNumeric GetCoef(size_t Power) const;
        TPolynom();
        TPolynom(const TPolynom &P);
        TPolynom(const std::vector<TNumeric>& Coef);
        TPolynom(const std::vector<int>& Coef);
        TPolynom(const TNumeric& a); //creates ax^0
        TPolynom(const TNumeric& a, const TNumeric &b); //creates ax+b
        TPolynom(const TNumeric& a, const TNumeric &b, const TNumeric& c); //creates ax^2+bx+c
        virtual ~TPolynom();

        TNumeric GetNumeric() const; //x^k + x^(k-1) + ... + x^0
        TNumeric GetNumeric(const TNumeric& UnknownVar) const; //x^k + x^(k-1) + ... + x^0
        TNumeric GetSeriesNumeric(const TNumeric& UnknownVar) const; //x^0 + x^1 + x^2 +...
        bool operator==(const TPolynom& P) const;
        TPolynom operator=(const TNumeric& N); // => P = const
        TPolynom operator=(const TPolynom& P);
        TPolynom operator+(const TPolynom& P);
        TPolynom operator-(const TPolynom& P);
        TPolynom operator*(const TPolynom& P);
        TPolynom operator/(const TPolynom& P);
        TPolynom operator%(const TPolynom& P); //остаток от деления
        TPolynom operator^(size_t Power);

        bool IsZero() const; //истина, если полином - тождественный нулю

        TPolynom operator*(const TNumeric& P);
        TPolynom operator/(const TNumeric& P);
        TPolynom Div(const TPolynom& P, TPolynom* Remainder) const;

        size_t MajorPower() const; //возвращает коэфициент при старшей степени (он не обязательно равен последнему элементу Coef, так как последний элемент Coef может быть равным нулю)
        TNumeric Calculate(const TNumeric &x) const; //вычисляет значение полинома в точке

        vector<double> Calculate(const vector<double> X) const;

        TPolynom Derivative();
        vector<TPolynom> Factorize() const; //факторизация возможна только для рациональных коэффициентов.

        void SaveToFile(ofstream &f);
        void LoadFromFile(ifstream &f);

        vector<TPolynom> FactorizeKroneker(vector<size_t> *Multiplicity = 0) const; //Факторизация методом Кронекера
        //Если Multiplicity = NULL, то все делители добавляются в res столько раз, какова их кратноть
        //Иначе все делители добавляются только один раз, а в Multiplicity - их кратность
};

bool PCancel(const TPolynom &P1, const TPolynom &P2, TPolynom *Res1, TPolynom *Res2); //сокращает многочлены на общие множители; возвращает истину, если хоть что-то удалось сократить

bool BuildIntegerPolynom(const vector<TNumeric> &X, const vector<TNumeric> &Y, TPolynom& P); //строит полином с целыми коэффициентами, проходящий через точки (X(i), Y(i))
bool BuildIntegerPolynom(const vector<int> &X, const vector<int> &Y, TPolynom& P); //строит полином с целыми коэффициентами, проходящий через точки (X(i), Y(i))


class TRationalFunction
//функция вида P(x)/Q(x)
{
        //void Assign(const TRationalFunction& R);
    public:
        TPolynom P, Q;
        TRationalFunction();
        TRationalFunction(const TPolynom P, const TPolynom Q);
        TRationalFunction(const std::vector<TNumeric> PCoef, const std::vector<TNumeric> QCoef);
        ~TRationalFunction();

        TNumeric GetNumeric() const;
        //TRationalFunction operator=(const TRationalFunction& R);
        TRationalFunction operator+(const TRationalFunction& R);
        TRationalFunction operator-(const TRationalFunction& R);
        TRationalFunction operator*(const TRationalFunction& R);
        TRationalFunction operator/(const TRationalFunction& R);

        TNumeric GetNumeric(const TNumeric& UnknownVar) { return MakeFrac(P.GetNumeric(UnknownVar), Q.GetNumeric(UnknownVar));}


        const TPolynom& Nominator() const {return P; }
        const TPolynom& Denominator() const {return Q; }
        TPolynom& Nominator() {return P; }
        TPolynom& Denominator() {return Q; }

        TNumeric Calculate(const TNumeric &X);
        TRationalFunction Derivative();
        TNumeric GetMainPartAndO(); //представляет рац функцию в форме Pn(x) + P'm(x)/Rk(x), m<k (O - "о" большое)s

        void Cancel(); //сократить числитель и знаменатель на общие множители
};

//vector<TNumeric> PolyDiv(vector<TNumeric> Nominator, vector<TNumeric> Denominator, vector<TNumeric> *Reminder);

#endif // POLYNOM_H
