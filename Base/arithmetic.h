//---------------------------------------------------------------------------
//todo: упрощение выражений вида a^log(b, a) => exp(ln(b)/ln(a)*ln(a)) = exp(ln(b)) = b
#ifndef arithmeticH
#define arithmeticH

#define TCanvas QPainter
#define string string
#define NaN NAN
#define Infinity INFINITY

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <optional>
#include <functional>
#include "tline.h"

#ifndef __int16
#define __int16 short int
#endif


using namespace std;


const int OperatorEqual = 7;
const int OperatorConst = 0;
const int OperatorSum = 1;
const int OperatorMinus = 6;
const int OperatorProd = 2;
const int OperatorSqrt = 3;
const int OperatorPow = 4;
const int OperatorFrac = 5;

const int OperatorEqSet = 8; //совокупность уравнений
const int OperatorEqSystem = 9; //система уравнений

const int OperatorLines = 10; //просто набор строк

const int OperatorBelongsTo = 11;
const int OperatorInterval = 12; // (x1, x2)
const int OperatorSegmentInterval = 13; //[x1, x2)
const int OperatorIntervalSegment = 14; //(x1, x2]
const int OperatorSegment = 15; //[x1, x2]

const int OperatorInline = 16; //все операнды в одну строчку
const int OperatorTab = 17; //отступ, а в остальном - как OperatorLines

const int OperatorUnion = 18;
const int OperatorIntersection = 19;


const int OperatorLessOrEqual = 20;
const int OperatorLess = 21;
const int OperatorGreater = 22;
const int OperatorGreaterOrEqual = 23;

const int OperatorSubIndex = 24;
const int OperatorSupIndex = 25;

const int OperatorDeriv = 26;

const int OperatorSin = 27;
const int OperatorCos = 28;
const int OperatorTg = 29;
const int OperatorCtg = 30;

const int OperatorArcsin = 31;
const int OperatorArccos = 32;
const int OperatorArctg = 33;

const int OperatorLog = 34; //logarithm of Operands[0] with base of Operands[1] = ln(Operands[0])/ln(Operands[1])
const int OperatorLn = 35; //natural logarithm
const int OperatorLg = 36; //decimal logarithm

const int OperatorPlusMinus = 37;
//const int OperatorModule = 38;
const int OperatorExp = 39;
const int OperatorIntegral = 40; //\int Operands[0] dOperands[1]
const int OperatorAbs = 41; //modulus
const int OperatorSign = 42; //signum

const int OperatorSmallO = 43; //o(x^n)
const int OperatorBigO = 44; //O(x^n)

const int OperatorSh = 45;
const int OperatorCh = 46;


QFont DecreaseFont(TPaintCanvas* Canvas);

enum TEditableFlags
{
    NoEditable = 0x00,
    ConstAllowed = 0x01,
    FunctionsAllowed = 0x02
    //NextFlag = 0x04 and so on
};

class TNumeric : public TRectangleElement
{
      void CreateClear(); //создает чистый TNumeric
      void Assign(const TNumeric& N);
      void DrawBracket(char Bracket, TPaintCanvas* Canvas, int X, int Y, int Height, int Depth) const;
      int GetBracketWidth(char Bracket, TPaintCanvas* Canvas, int Height, int Depth) const;
      void DrawBracket(string Bracket, TPaintCanvas* Canvas, int X, int Y, int Height, int Depth) const;
      int GetBracketWidth(string Bracket, TPaintCanvas* Canvas, int Height, int Depth) const;


      void FunctionGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
      void FunctionDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

      void StrGetTextRectangle(const char *Str, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
      void StrDraw(const char *Str, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

      void InlineGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
      void InlineDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

      void EqualGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
      void EqualDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

      void BelongsToGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
      void BelongsToDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

      void IntervalGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const; //for OperatorInterval, OperatorSegmentInterval, OperatorIntervalSegment, OperatorSegment
      void IntervalDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

      void SqrtGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
      void SqrtDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

      void ProdGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
      void ProdDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

      void SumGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
      void SumDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

      void MinusGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
      void MinusDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

      void PlusMinusGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
      void PlusMinusDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

      void FracGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
      void FracDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

      void PowGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
      void PowDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

      void AbsGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
      void AbsDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

      void SubIndexGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
      void SubIndexDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

      void LinesGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, int LeftMargin, bool Simplify) const;
      void LinesDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, int LeftMargin, bool Simplify) const;

      void EqSystemGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
      void EqSystemDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

      void EqSetGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
      void EqSetDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

      void IntegralGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
      void IntegralDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

      void UnionGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
      void UnionDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

      void IntersectionGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
      void IntersectionDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

      void DerivGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
      void DerivDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

      int GetOperatorPriority(int OpCode) const;
      int CompareOperatorsPriority(int OpCode1, int OpCode2) const;

      TNumeric SimplifyTrig() const; //упрощает выражение, где последним оператором является тригонометрическая функция
      TNumeric SimplifyInverseTrig() const; //упрощает выражение, где последним оператором является обратная тригонометрическая функция
      TNumeric SimplifyLog() const; //упрощает выражение, где последним оператором является Log(A, B)
      bool GetRational(int &Nominator, int &Denominator); //если это возможно, представляет объект в виде рациональной дроби; если не возможно, то false
      void SimplifyFunctions(); //упрощает функции везде, где возможно

      TNumeric MathoCmd(const string& Cmd) const; //интерфейс к mathomatic

      void CheckInitialized() const; //проверяет, был ли чем-нибудь инициализирован объект ранее; если не был, то вызывает исключение

      //Для ускорения отрисовки
      mutable bool OwnSizeActual; //true, если нижеуказанные размеры актуальны
      mutable int OwnWidth, OwnHeight, OwnDepth;
      mutable bool SizeWithBrackets; //при проверке актуальности размеров нужно сравнивать SizeWithBrackets с NeedBrackets только для корневого элемента
      //Для остальных SizeWidthBrackets можно не проверять, так как корневой элемент отвечает за расстановку скобок ниже него самого,
      //лишь бы он сам был вызван с тем же значением NeedBrackets
      bool IsSizeActual() const; //true, если размеры всех Operands актуальны
    public:
      mutable int ID; //идентификатор, по которому можно получить указатель на объект TNumeric
      mutable int EditableFlags; //определяет, какого типа значения может принимать это поле
      mutable int MouseX, MouseY;
      mutable bool DrawMouse;
      mutable TNumeric* Active; //элемент под мышкой
      mutable TNumeric* Selected; //выбранный элемент
      mutable int CursorPos; //позиция курсора

      vector<TNumeric> Operands; //на прямую можно вызывать только Operands.operator[] и const-функции (типа size())
      void OperandsPushback(const TNumeric &Val);
      void OperandsClear();

      string K; // todo: wtf?
      int Operator;
      TNumeric();
      virtual ~TNumeric();
      explicit TNumeric(double d);
      explicit TNumeric(int d);
      explicit TNumeric(size_t d);
      explicit TNumeric(string d);
      TNumeric(const TNumeric& N);     

      bool IsVariable() const; //returns true if object is like "x" or "x_{something}" (object designates variable)
      bool IsEqual(const TNumeric &N) const; //Если *this и N совпадают полностью друг с другом
      bool DependsOn(const char* variable) const; //Если variable хоть как-то входит в выражение, то true
      bool operator==(const TNumeric& N) const; //Если *this - N символьно равно нулю, то true
      bool operator!=(const TNumeric& N) const
      {
          return !operator == (N);
      }



      bool operator==(double Value) const
      {
          TNumeric Temp = Simplify();
          if(Temp.CanCalculate()) return Temp.Calculate() == Value;
          else return false;
      }
      bool operator!=(double Value) const
      {
          return !operator ==(Value);
      }           

      void operator=(const TNumeric& N);
      TNumeric operator*(const TNumeric& N) const;
      TNumeric operator*(int N) const  { return this->operator *(TNumeric(N)); }
      TNumeric operator+(const TNumeric& N) const;
      TNumeric operator+(int N) const  { return this->operator +(TNumeric(N)); }
      TNumeric operator-(const TNumeric& N) const;
      TNumeric operator-(int N) const  { return this->operator -(TNumeric(N)); }
      TNumeric operator-() const ;
      TNumeric operator/(const TNumeric& N) const;
      TNumeric operator/(int N) const  { return this->operator /(TNumeric(N)); }
      TNumeric sqrt() const;
      TNumeric operator^(const TNumeric& N) const;
      TNumeric operator^(int N) const  { return this->operator ^(TNumeric(N)); }

      TNumeric Derivative(const string VarName = "x") const;

      //Создает совокупоность из двух уравнений: N1 | N2
      void MakeEqSet(const TNumeric& N1, const TNumeric& N2);
      //Создает систему из двух уравнений: N1 & N2
      void MakeEqSystem(const TNumeric& N1, const TNumeric& N2);

      //Если Simplify == true, то функции рисуют формулу в хорошем виде, проводя некоторые преобразования. Иначе без преобразований
      //шрифт надо выбирать вручную, вызвав заранее     Canvas->Font = Canvas->FormulaFont;
      void PrettyGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;
      int PrettyDrawAtBaseLeft(TPaintCanvas* Canvas, int X, int Y, bool NeedBrackets, bool Simplify) const;


      //функции рисуют формулу после некоторых преобразований, упрощающих её вид
      void GetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool NeedBrackets) const
      {
          //MaxWidth не используется
          Canvas->Font = Canvas->FormulaFont;
          PrettyGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, true);
      }

      int DrawAtBaseLeft(TPaintCanvas* Canvas, int X, int Y, bool NeedBrackets) const
      {
          //MaxWidth не используется
          Canvas->Font = Canvas->FormulaFont;
          return PrettyDrawAtBaseLeft(Canvas, X, Y, NeedBrackets, true);
      }


      virtual void GetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, int MaxWidth = -1) const
      {
          //MaxWidth не используется
          Q_UNUSED(MaxWidth);
          Canvas->Font = Canvas->FormulaFont;
          GetTextRectangle(Canvas, Width, Height, Depth, false);
      }

      virtual void DrawAtBaseLeft(TPaintCanvas* Canvas, int X, int Y, int MaxWidth = -1) const;

      void Assign(const char* str);
      void Assign(char* str);

      string CodeBasic() const;
      void SimplifyPresentation(); //упрощает представление формулы (заменяет a^(1/2) на sqrt(a))

      double Calculate() const;
      TNumeric Substitute(const string& var, const TNumeric& Val) const;

      bool hasID(int ID);
      std::optional<std::reference_wrapper<TNumeric>> GetByID(int ID);
      void ClearID(); //устанавливает ID у себя и всех потомков в -1

      TNumeric Simplify() const;
      TNumeric Unfactor() const;

      bool CanCalculate() const; //true, если выражение является арифметическим выражением и является полным, то есть не содержит пустых прямоугольников
      virtual void SetEditableFlags(int Flags); //устанавливает значение Editable у себя и у всех потомков

      void LoadFromFile(ifstream &f);
      void WriteToFile(ofstream &f);

      bool IsInteger(int* Int = 0) const; //возвращает true, если коэффициент является целым числом. Число записывает в Int

      void EliminateFunctions(size_t &StartID, map<string, TNumeric>& Map); //обозначаем функции, которых нет в пакете mathomatic, за новые переменные a_i; первая переменная получает номер StartID;
      //StartID постепенно увеличивается
      void RestoreFunctions(const map<string, TNumeric>& V); //заменяем переменные обратно на функции

};


string DeleteExternalBrackets(string q);
TNumeric MakeTab(const TNumeric& N1);
TNumeric MakeEquality(const TNumeric& N1, const TNumeric& N2);
TNumeric MakeBelongsTo(const TNumeric& N1, const TNumeric& N2);
TNumeric MakeInterval(const TNumeric &X1, const TNumeric& X2, bool includeleft = false, bool includeright = false);
TNumeric MakeInline(const TNumeric &N1, const TNumeric &N2);
TNumeric MakeInline(const TNumeric &N1, const TNumeric &N2, const TNumeric &N3);
TNumeric MakeUnion(const TNumeric &N1, const TNumeric &N2);
TNumeric MakeSubscript(const TNumeric &N1, const TNumeric &N2);
TNumeric MakeSuperscript(const TNumeric &N1, const TNumeric &N2);
TNumeric MakePow(const TNumeric &N1, const TNumeric &N2);
TNumeric MakeExp(const TNumeric &N);
TNumeric MakeCh(const TNumeric &N);
TNumeric MakeSh(const TNumeric &N);
TNumeric MakeSqrt(const TNumeric& N);

TNumeric MakeFrac(const TNumeric &N1, const TNumeric &N2);
TNumeric MakeProd(const TNumeric& N1, const TNumeric &N2);
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

TNumeric GetPolynom(size_t Power, size_t StartID); //конструирует многочлен с нулевыми коэффициентами. ID устанавливаются от StartID до StartID+Power

//---------------------------------------------------------------------------
TNumeric* FindParent(TNumeric* Root, TNumeric* Child);
bool CanErase(TNumeric* Root, TNumeric* WhatToDelete);
void EraseNumeric(TNumeric* Root, TNumeric* WhatToDelete);
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

//Operands.clear() OperandsClear()
//Operands.push_back(N) OperandsPushback(N)
//Operands(i) FOperands(i)

//---------------------------------------------------------------------------
#endif
