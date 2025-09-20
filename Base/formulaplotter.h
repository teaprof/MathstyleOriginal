#ifndef FORMULAPLOTTER_H
#define FORMULAPLOTTER_H

#include "tline.h"
#include "arithmetic.h"

class TFormulaPlotter : public TRectangleElement
{
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

    mutable int EditableFlags; //определяет, какого типа значения может принимать это поле
    mutable int MouseX, MouseY;
    mutable bool DrawMouse;
    mutable int CursorPos; //позиция курсора
    mutable TNumeric* Active; //элемент под мышкой
    mutable TNumeric* Selected; //выбранный элемент

public:        
    //Для ускорения отрисовки
    mutable bool OwnSizeActual; //true, если нижеуказанные размеры актуальны
    mutable int OwnWidth, OwnHeight, OwnDepth;
    mutable bool SizeWithBrackets; //при проверке актуальности размеров нужно сравнивать SizeWithBrackets с NeedBrackets только для корневого элемента
    //Для остальных SizeWidthBrackets можно не проверять, так как корневой элемент отвечает за расстановку скобок ниже него самого,
    //лишь бы он сам был вызван с тем же значением NeedBrackets

    TNumeric N;


    TFormulaPlotter(const TNumeric& Numeric);

    bool IsSizeActual() const; //true, если размеры всех Operands актуальны



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

    virtual void accept(TRectangleElementVisitor& visitor) const {
        visitor.visit(*this);
    }
    std::string CodeBasic() const {
        return N.CodeBasic();
    }
};

QFont DecreaseFont(TPaintCanvas* Canvas);

#endif // FORMULAPLOTTER_H
