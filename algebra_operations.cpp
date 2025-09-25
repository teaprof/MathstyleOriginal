#include "algebra_operations.h"

#include <algorithm>
#include <iostream>

using namespace std;

// #define __DEBUG__

int GetNOD(int a, int b) {
    if (a == 0 && b == 0)
        return 1;
    if (a == 0)
        return abs(b);
    if (b == 0)
        return abs(a);
    vector<int> amults = IntFactorize(a);
    vector<int> bmults = IntFactorize(b);
    int res = 1;
    for (size_t i = 0; i < amults.size(); i++) {
        vector<int>::iterator pos = find(bmults.begin(), bmults.end(), amults[i]);
        if (pos != bmults.end()) {
            res *= amults[i];
            bmults.erase(pos);
        }
    }
    return res;
}

vector<int> IntFactorize(int N)
// раскладывает N на простые множители
{
    vector<int> Res;
    size_t AbsN = abs(N);
    for (size_t i = 2; i <= AbsN;) {
        bool IsSimple = true;
        for (size_t j = 2; j < i; j++)
            if (i % j == 0) {
                IsSimple = false;
                break;
            }
        if (IsSimple) {
            if (AbsN % i == 0) {
                Res.push_back(i);
                AbsN = AbsN / i;
            } else
                i++;
        } else
            i++;
    };
    if (AbsN != 1 || Res.empty())
        Res.push_back(AbsN);
    return Res;
}

vector<int> GetAllProds(vector<int> Divizors)
// выдает все возможные произведения чисел из Divizors
{
    vector<int> Res;
    vector<bool> Included;
    Included.assign(Divizors.size(), false);
    bool Last = false;
    while (!Last) {
        // вычисляем очередное произведение
        int Prod = 1;
        for (size_t i = 0; i < Divizors.size(); i++)
            if (Included[i])
                Prod *= Divizors[i];
        // если его еще не было, то добавляем
        if (find(Res.begin(), Res.end(), Prod) == Res.end())
            Res.push_back(Prod);
        // переходим к следующей комбинации
        size_t i = 0;
        do {
            if (Included[i] == false) {
                Included[i] = true;
                break;
            } else {
                Included[i] = false;
                i++;
            };
        } while (i < Included.size());
        if (i == Included.size())
            Last = true;
    }
    return Res;
}

int GetNOK(int a, int b)
// находим наименьшее общее кратное
{
    if (a == 0 && b == 0)
        return 1;
    if (a == 0)
        return abs(b);
    if (b == 0)
        return abs(a);
    vector<int> Fa = IntFactorize(a);
    vector<int> Fb = IntFactorize(b);
    for (size_t i = 0; i < Fa.size(); i++) {
        vector<int>::iterator it = find(Fb.begin(), Fb.end(), Fa[i]);
        if (it != Fb.end())
            Fb.erase(it);
    }
    int res = a;
    for (size_t i = 0; i < Fb.size(); i++)
        res *= Fb[i];
    return res;
}

void Factorize(int N, vector<int>& P, vector<int>& Mult) {
    if (N < 0)
        N = -N;
    if (N == 0)
        return;
    if (N == 1)
        return;
    int Q = 2;
    while (Q < N) {
        int M = 0;
        while (N % Q == 0) {
            N = N / Q;
            M++;
        };
        if (M > 0) {
            P.push_back(Q);
            Mult.push_back(M);
        }
        Q++;
    }
}

int CheckCommonPower(int A, int B, int& Power, int& A1, int& B1)
// Ищет такое число Power и A1, B1, чтобы:
//  A = A1^Power, B = B1^Power;
// Необходимо, чтобы A и B были положительны
{
    vector<int> AP, AM, BP, BM;
    int k1, k2;
    if ((A == 1) & (B == 1)) {
        Power = 1;
        A1 = 1;
        B1 = 1;
        return Power;
    } else {
        if (A != 1) {
            Factorize(A, AP, AM);
            k1 = GetNOD(AM);
        } else
            k1 = 1;
        if (B != 1) {
            Factorize(B, BP, BM);
            k2 = GetNOD(BM);
        } else
            k2 = 1;

        if (A == 1)
            Power = k2;
        else if (B == 1)
            Power = k1;
        else
            Power = k1 < k2 ? k1 : k2;

        A1 = 1;
        for (size_t i = 0; i < AM.size(); i++) {
            AM[i] /= Power;
            for (int k = 0; k < AM[i]; k++)
                A1 *= AP[k];
        };
        B1 = 1;
        for (size_t i = 0; i < BM.size(); i++) {
            BM[i] /= Power;
            for (int k = 0; k < BM[i]; k++)
                B1 *= BP[k];
        };
    };
    return Power;
}

int GetNOD(const vector<int>& V) {
    if (V.size() == 0)
        return 1;
    if (V.size() == 1)
        return V[0];
    int NOD = GetNOD(V[0], V[1]);
    for (size_t i = 2; i < V.size(); i++)
        NOD = GetNOD(NOD, V[i]);
    return NOD;
}

int GetNOK(vector<int> V) {
    int NOK = 1;
    int Divisor = 2;
    if (V.size() == 0)
        return 1;
    for (size_t i = 0; i < V.size(); i++)
        V[i] = abs(V[i]);
    do {
        bool AllOnes = true;     // все V[i] равны 1, дальше искать их делители бесполезно
        bool IsDivisor = false;  // число Divisor является делителем хотя бы одного V[i]
        for (size_t i = 0; i < V.size(); i++) {
            if (V[i] != 1 && V[i] != 0) {
                AllOnes = false;
                if (V[i] % Divisor == 0) {
                    V[i] = V[i] / Divisor;
                    IsDivisor = true;
                }
            }
        }
        if (IsDivisor)
            NOK *= Divisor;  // возможно, число Divisor еще раз является делителем
        else
            Divisor++;  // число Divisor делителем больше не является
        if (AllOnes)
            break;
    } while (true);
    return NOK;
}

bool Gauss(size_t left, size_t up, vector<vector<TNumeric>>& Coefs, vector<TNumeric>& RightPart)
// проходит гауссом от (left, up) до нижнего правого угла
{
    if (up >= Coefs.size())
        return true;  // достигли низа
    if (left >= Coefs[0].size())
        return true;  // достигли права

    size_t NonZero = up;  // номер уравнения с ненулевым коэффициентом при переменной номер left
    TNumeric Zero("0");
    while (NonZero < Coefs.size() && Coefs[NonZero][left] == Zero)
        NonZero++;

    if (NonZero < Coefs.size()) {
        for (size_t i = up; i < Coefs.size(); i++) {
            if (i != NonZero) {
                TNumeric Mult = TNumeric("-1") * Coefs[i][left] / Coefs[NonZero][left];
                // прибавляем к строке i строку NonZero*Mult
                for (size_t j = 0; j < Coefs[0].size(); j++)
                    Coefs[i][j] = (Coefs[i][j] + Coefs[NonZero][j] * Mult).Simplify();
                RightPart[i] = (RightPart[i] + RightPart[NonZero] * Mult).Simplify();
            };
        }
    };
    return Gauss(left + 1, up + 1, Coefs, RightPart);
};

bool SolveLinearSystem(vector<vector<TNumeric>> Coefs, vector<TNumeric> RightPart, vector<TNumeric>& Res, bool OnlyInteger)
// Решаем систему методом Гаусса
{
    if (Gauss(0, 0, Coefs, RightPart) == false)
        return false;

#ifdef __DEBUG__
    for (size_t i = 0; i < Coefs.size(); i++) {
        for (size_t j = 0; j < Coefs[i].size(); j++)
            cout << Coefs[i][j].CodeBasic() << "\t";
        cout << endl;
    };
#endif

    Res.assign(Coefs.size(), TNumeric("0"));
    for (size_t i = 0; i < Coefs.size(); i++) {
        size_t row = Coefs.size() - 1 - i;
        if (Coefs[row][row] == TNumeric("0"))
            return false;
        Res[row] = RightPart[row];
        for (size_t j = row + 1; j < Coefs.size(); j++)
            Res[row] = Res[row] - Coefs[row][j] * Res[j];
        Res[row] = (Res[row] / Coefs[row][row]).Simplify();
        if (OnlyInteger) {
            if (Res[row].isInteger(0) == false)
                return false;
        }
    }
    return true;
}
