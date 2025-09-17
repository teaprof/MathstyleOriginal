#ifndef ALGEBRA_OPERATIONS_H
#define ALGEBRA_OPERATIONS_H
#include <vector>
#include <Base/arithmetic.h>
using namespace std;

//раскладывает abs(N) на простые множители
vector<int> IntFactorize(int N);

//выдает все возможные произведения чисел из Divizors
vector<int> GetAllProds(vector<int> Divizors);

//находит наименьшее общее кратное
//Если есть коэффициенты меньше нуля, то берет их по модулю
//Если какой-то V[i] = 0, то он не учитывается
int GetNOK(int a, int b);
int GetNOK(vector<int> V);

//находит наибольший общий делитель
//Если есть коэффициенты меньше нуля, то берет их по модулю
//Если какой-то V[i] = 0, то он не учитывается
int GetNOD(int a, int b);
int GetNOD(const vector<int>& V);


//Факторизует N, простые множители записывает в P, их кратность - в Mult
void Factorize(int N, vector<int> &P, vector<int> &Mult);

//Ищет такое число Power и A1, B1, чтобы:
// A = A1^Power, B = B1^Power;
//Необходимо, чтобы A и B были положительны
int CheckCommonPower(int A, int B, int& Power, int &A1, int &B1);


bool SolveLinearSystem(vector<vector<TNumeric> > Coefs, vector<TNumeric> RightPart, vector<TNumeric> &Res, bool OnlyInteger = false); //решает систему уравнений с КВАДРАТНОЙ НЕВЫРОЖДЕННОЙ матрицей A_ij = Coef[i][j]
//OnlyInteger=true, если рассматриваются только целочисленные решения


#endif // ALGEBRA_OPERATIONS_H
