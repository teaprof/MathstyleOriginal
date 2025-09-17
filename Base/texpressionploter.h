#ifndef TEXPRESSIONPLOTER_H
#define TEXPRESSIONPLOTER_H

#define TCanvas QPainter
#define AnsiString string
#define NaN NAN
#define Infinity INFINITY

#include <vector>
#include <string>
#include <fstream>

//---------------------------------------------------------------------------

#include "tline.h"

#ifndef __int16
#define __int16 short int
#endif


using namespace std;

class TExpressionPloter : public TRectangleElement
{
      void Assign(const TNumeric& N);
      void DrawBracket(char Bracket, TPaintCanvas* Canvas, int X, int Y, int Height, int Depth);
      int GetBracketWidth(char Bracket, TPaintCanvas* Canvas, int Height, int Depth);


      void SymbolGetTextRectangle(const char *Sign, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets);
      void SymbolDraw(const char *Sign, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets);

      void InlineGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets);
      void InlineDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets);

      void EqualGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets);
      void EqualDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets);

      void BelongsToGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets);
      void BelongsToDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets);

      void IntervalGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets); //for OperatorInterval, OperatorSegmentInterval, OperatorIntervalSegment, OperatorSegment
      void IntervalDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets);


      void SqrtGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets);
      void SqrtDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets);

      void ProdGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets);
      void ProdDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets);

      void SumGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets);
      void SumDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets);

      void MinusGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets);
      void MinusDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets);

      void FracGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets);
      void FracDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets);

      void PowGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets);
      void PowDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets);

      void SubIndexGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets);
      void SubIndexDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets);

      void LinesGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, int LeftMargin = 0);
      void LinesDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, int LeftMargin = 0);

      void EqSystemGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets);
      void EqSystemDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets);

      void EqSetGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets);
      void EqSetDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets);

      void UnionGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets);
      void UnionDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets);

      void IntersectionGetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets);
      void IntersectionDraw(TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets);

      int GetOperatorPriority(int OpCode);
      int CompareOperatorsPriority(int OpCode1, int OpCode2);
   public:
      int ID; //идентификатор, по которому можно получить указатель на объект TNumeric
      bool Editable; //true, если поле можно редактировать
      int MouseX, MouseY;
      bool DrawMouse;
      TNumeric* Active; //элемент под мышкой
      TNumeric* Selected; //выбранный элемент
      int CursorPos; //позиция курсора

      //Создает совокупоность из двух уравнений: N1 | N2
      void MakeEqSet(const TNumeric& N1, const TNumeric& N2);
      //Создает систему из двух уравнений: N1 & N2
      void MakeEqSystem(const TNumeric& N1, const TNumeric& N2);

      void GetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool NeedBrackets);

      int Draw(TPaintCanvas* Canvas, int X, int Y, bool NeedBrackets);

      virtual void GetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, int MaxWidth = -1)
      {
          //MaxWidth не используется
          GetTextRectangle(Canvas, Width, Height, Depth, false);
      }

      virtual void Draw(TNumeric* N, TPaintCanvas* Canvas, int X, int Y, int MaxWidth = -1)
      {
          Draw(Canvas, X, Y, false);
      }

      void Assign(const char* str);
      void Assign(char* str);

      void LoadFromFile(ifstream &f);
      void WriteToFile(ofstream &f);
};
//---------------------------------------------------------------------------
#endif // TEXPRESSIONPLOTER_H
