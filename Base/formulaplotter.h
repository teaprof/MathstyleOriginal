#ifndef FORMULAPLOTTER_H
#define FORMULAPLOTTER_H

#include "tline.h"
#include "arithmetic.h"

class FormulaPlotter
{
    void DrawBracket(char Bracket, TPaintCanvas* Canvas, int X, int Y, int Height, int Depth) const;
    int GetBracketWidth(char Bracket, TPaintCanvas* Canvas, int Height, int Depth) const;
    void DrawBracket(string Bracket, TPaintCanvas* Canvas, int X, int Y, int Height, int Depth) const;
    int GetBracketWidth(string Bracket, TPaintCanvas* Canvas, int Height, int Depth) const;


    void FunctionGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
    void FunctionDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

    void StrGetTextRectangle(const char *Str, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
    void StrDraw(const char *Str, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

    void InlineGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
    void InlineDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

    void EqualGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
    void EqualDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

    void BelongsToGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
    void BelongsToDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

    void IntervalGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const; //for OperatorInterval, OperatorSegmentInterval, OperatorIntervalSegment, OperatorSegment
    void IntervalDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

    void SqrtGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
    void SqrtDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

    void ProdGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
    void ProdDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

    void SumGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
    void SumDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

    void MinusGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
    void MinusDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

    void PlusMinusGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
    void PlusMinusDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

    void FracGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
    void FracDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

    void PowGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
    void PowDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

    void AbsGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
    void AbsDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

    void SubIndexGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
    void SubIndexDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

    void LinesGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, int LeftMargin, bool Simplify) const;
    void LinesDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, int LeftMargin, bool Simplify) const;

    void EqSystemGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
    void EqSystemDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

    void EqSetGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
    void EqSetDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

    void IntegralGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
    void IntegralDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

    void UnionGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
    void UnionDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

    void IntersectionGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
    void IntersectionDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;

    void DerivGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const;
    void DerivDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;
public:
    //Если Simplify == true, то функции рисуют формулу в хорошем виде, проводя некоторые преобразования. Иначе без преобразований
    //шрифт надо выбирать вручную, вызвав заранее     Canvas->Font = Canvas->FormulaFont;
    void PrettyGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const;
    int PrettyDrawAtBaseLeft(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, bool NeedBrackets, bool Simplify) const;


    //функции рисуют формулу после некоторых преобразований, упрощающих её вид
    void GetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool NeedBrackets) const
    {
        //MaxWidth не используется
        Canvas->Font = Canvas->FormulaFont;
        PrettyGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, true);
    }

    int DrawAtBaseLeft(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, bool NeedBrackets) const
    {
        //MaxWidth не используется
        Canvas->Font = Canvas->FormulaFont;
        return PrettyDrawAtBaseLeft(Canvas, X, Y, NeedBrackets, true);
    }


    virtual void GetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, int MaxWidth = -1) const
    {
        //MaxWidth не используется
        Q_UNUSED(MaxWidth);
        Canvas->Font = Canvas->FormulaFont;
        GetTextRectangle(Canvas, Width, Height, Depth, false);
    }

    virtual void DrawAtBaseLeft(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int MaxWidth = -1) const;
};

QFont DecreaseFont(TPaintCanvas* Canvas);

#endif // FORMULAPLOTTER_H
