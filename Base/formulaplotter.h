#ifndef FORMULAPLOTTER_H
#define FORMULAPLOTTER_H

#include <QRect>

#include "arithmetic.h"
#include "tline.h"

class TFormulaPlotter {
    void DrawBracket(char Bracket, int X, int Y, int Height, int Depth) const;
    int GetBracketWidth(char Bracket, int Height, int Depth) const;
    void DrawBracket(string Bracket, int X, int Y, int Height, int Depth) const;
    int GetBracketWidth(string Bracket, int Height, int Depth) const;

    Metrics PrettyGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool NeedBrackets, bool Simplify) const;

    Metrics FunctionGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const;
    void FunctionDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const;

    Metrics StrGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, const char* separator_str, bool WithBrackets, bool Simplify) const;
    void StrDraw(std::shared_ptr<TNumeric> N, const char* separator_str, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const;

    Metrics InlineGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const;
    void InlineDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const;

    Metrics EqualGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const;
    void EqualDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const;

    Metrics BelongsToGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const;
    void BelongsToDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const;

    /// for OperatorInterval, OperatorSegmentInterval, OperatorIntervalSegment, OperatorSegment
    Metrics IntervalGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const;
    void IntervalDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const;

    Metrics SqrtGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const;
    void SqrtDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const;

    Metrics ProdGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const;
    void ProdDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const;

    Metrics SumGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const;
    void SumDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const;

    Metrics MinusGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const;
    void MinusDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const;

    Metrics PlusMinusGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const;
    void PlusMinusDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const;

    Metrics FracGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const;
    void FracDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const;

    Metrics PowGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const;
    void PowDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const;

    Metrics AbsGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const;
    void AbsDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const;

    Metrics SubIndexGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const;
    void SubIndexDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const;

    Metrics LinesGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool WithBrackets, int LeftMargin, bool Simplify) const;
    void LinesDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, int LeftMargin, bool Simplify) const;

    Metrics EqSystemGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const;
    void EqSystemDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const;

    Metrics EqSetGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const;
    void EqSetDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const;

    Metrics IntegralGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const;
    void IntegralDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const;

    Metrics UnionGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const;
    void UnionDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const;

    Metrics IntersectionGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const;
    void IntersectionDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const;

    Metrics DerivGetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const;
    void DerivDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const;

  protected:  
    class ZRect : public QRect {
      public:
        int zorder;
        template<class... Args>
        ZRect(int zorder_, Args... args) : QRect(std::forward<Args>(args)...), zorder{zorder_} {}

        ZRect() = default;
    };
    mutable std::map<std::shared_ptr<TNumeric>, Metrics> metrics_cache_;
    mutable std::map<std::shared_ptr<TNumeric>, ZRect> zrect_cache_;
    void invalidateRects() const { /// \todo: remove const
        metrics_cache_.clear();
        zrect_cache_.clear();
    }
    std::shared_ptr<TPaintCanvas> canvas_;

    Metrics PrettyGetTextRectangleCached(std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const {
        return metrics_cache_[N];
    }

  public:
    std::shared_ptr<TNumeric> underMouse{nullptr};
    TFormulaPlotter(std::shared_ptr<TPaintCanvas> Canvas);
    void setCanvas(std::shared_ptr<TPaintCanvas> Canvas) {
        canvas_ = Canvas;
    }  // painter should be created inside PaintEvent
    virtual ~TFormulaPlotter() {}

    // Если Simplify == true, то функции рисуют формулу в хорошем виде, проводя некоторые преобразования. Иначе без преобразований
    // шрифт надо выбирать вручную, вызвав заранее     Canvas->Font = Canvas->FormulaFont;
    Metrics PrettyGetTextRectangle(std::shared_ptr<const TNumeric> N, bool NeedBrackets, bool Simplify) const;
    Metrics PrettyGetTextRectangle(std::shared_ptr<TNumeric> N, bool NeedBrackets, bool Simplify) const;
    Metrics PrettyDrawAtBaseLeft(std::shared_ptr<const TNumeric> N, int X, int Y, bool NeedBrackets, bool Simplify) const;
    Metrics PrettyDrawAtBaseLeft(std::shared_ptr<TNumeric> N, int X, int Y, bool NeedBrackets, bool Simplify) const;

    // функции рисуют формулу после некоторых преобразований, упрощающих её вид
    Metrics GetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, bool NeedBrackets) const;
    virtual Metrics GetTextRectangle(int zorder, std::shared_ptr<TNumeric> N, int MaxWidth = -1) const;
    Metrics GetTextRectangle(std::shared_ptr<TNumeric> N, bool NeedBrackets) const;
    virtual Metrics GetTextRectangle(std::shared_ptr<TNumeric> N, int MaxWidth = -1) const;
    void DrawAtBaseLeft(std::shared_ptr<TNumeric> N, int X, int Y, int MaxWidth = -1) const;
    // virtual void DrawAtBaseLeft(int X, int Y, int MaxWidth = -1) const;
    std::shared_ptr<TNumeric> getElementUnderMouse(int mouseX, int mouseY) const;
    static std::string CodeBasic(std::shared_ptr<const TNumeric> N);
    virtual QColor getFontColor(std::shared_ptr<TNumeric> N) const;
    virtual std::optional<QColor> getBorderColor(std::shared_ptr<TNumeric> N) const;
};

class TConstFormulaPlotter : public TRectangleElement {
    std::shared_ptr<TNumeric> numeric_;

  public:
    TConstFormulaPlotter(std::shared_ptr<TNumeric> N) : numeric_(N) {}

    virtual Metrics GetTextRectangle(std::shared_ptr<TPaintCanvas> Canvas, int MaxWidth = -1) const {
        TFormulaPlotter formulaPlotter(Canvas);
        return formulaPlotter.GetTextRectangle(numeric_, MaxWidth);
    }
    virtual void DrawAtBaseLeft(std::shared_ptr<TPaintCanvas> Canvas, int X, int Y, int MaxWidth = -1) const {
        TFormulaPlotter formulaPlotter(Canvas);
        formulaPlotter.DrawAtBaseLeft(numeric_, X, Y, MaxWidth);
    }

    QColor GetColor() const;

    std::shared_ptr<const TNumeric> numeric() const {
        return numeric_;
    }

    virtual void accept(TRectangleElementVisitor& visitor) const {
        visitor.visit(*this);
    }
};

QFont DecreaseFont(std::shared_ptr<TPaintCanvas> Canvas);

#endif  // FORMULAPLOTTER_H
