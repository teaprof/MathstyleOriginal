#ifndef FORMULAPLOTTER_H
#define FORMULAPLOTTER_H

#include <QRect>

#include "arithmetic.h"
#include "tline.h"

class TFormulaPlotter {
    Metrics PrettyDrawAtBaseLeftRecursive(int zorder, std::shared_ptr<TNumeric> N, int X, int Y, bool withBrackets) const;

    void DrawBracket(char Bracket, int X, int Y, int Height, int Depth) const;
    int GetBracketWidth(char Bracket, int Height, int Depth) const;
    void DrawBracket(string Bracket, int X, int Y, int Height, int Depth) const;
    int GetBracketWidth(string Bracket, int Height, int Depth) const;

    Metrics FunctionGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets) const;
    void FunctionDraw(int zorder, std::shared_ptr<TNumeric> N, int X, int Y, bool withBrackets) const;

    Metrics StrGetTextRectangle(std::shared_ptr<TNumeric> N, const char* separator_str, bool WithBrackets) const;
    void StrDraw(int zorder, std::shared_ptr<TNumeric> N, const char* separator_str, int X, int Y, bool withBrackets) const;

    Metrics InlineGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets) const;
    void InlineDraw(int zorder, std::shared_ptr<TNumeric> N, int X, int Y, bool withBrackets) const;

    Metrics EqualGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets) const;
    void EqualDraw(int zorder, std::shared_ptr<TNumeric> N, int X, int Y, bool withBrackets) const;

    Metrics BelongsToGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets) const;
    void BelongsToDraw(int zorder, std::shared_ptr<TNumeric> N, int X, int Y, bool withBrackets) const;

    /// for OperatorInterval, OperatorSegmentInterval, OperatorIntervalSegment, OperatorSegment
    Metrics IntervalGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets) const;
    void IntervalDraw(int zorder, std::shared_ptr<TNumeric> N, int X, int Y, bool withBrackets) const;

    Metrics SqrtGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets) const;
    void SqrtDraw(int zorder, std::shared_ptr<TNumeric> N, int X, int Y, bool withBrackets) const;

    Metrics ProdGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets) const;
    void ProdDraw(int zorder, std::shared_ptr<TNumeric> N, int X, int Y, bool withBrackets) const;

    Metrics SumGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets) const;
    void SumDraw(int zorder, std::shared_ptr<TNumeric> N, int X, int Y, bool withBrackets) const;

    Metrics MinusGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets) const;
    void MinusDraw(int zorder, std::shared_ptr<TNumeric> N, int X, int Y, bool withBrackets) const;

    Metrics PlusMinusGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets) const;
    void PlusMinusDraw(int zorder, std::shared_ptr<TNumeric> N, int X, int Y, bool withBrackets) const;

    Metrics FracGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets) const;
    void FracDraw(int zorder, std::shared_ptr<TNumeric> N, int X, int Y, bool withBrackets) const;

    Metrics PowGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets) const;
    void PowDraw(int zorder, std::shared_ptr<TNumeric> N, int X, int Y, bool withBrackets) const;

    Metrics AbsGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets) const;
    void AbsDraw(int zorder, std::shared_ptr<TNumeric> N, int X, int Y, bool withBrackets) const;

    Metrics SubIndexGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets) const;
    void SubIndexDraw(int zorder, std::shared_ptr<TNumeric> N, int X, int Y, bool withBrackets) const;

    Metrics LinesGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets, int LeftMargin) const;
    void LinesDraw(int zorder, std::shared_ptr<TNumeric> N, int X, int Y, bool NeedBrackets, int LeftMargin) const;

    Metrics EqSystemGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets) const;
    void EqSystemDraw(int zorder, std::shared_ptr<TNumeric> N, int X, int Y, bool withBrackets) const;

    Metrics EqSetGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets) const;
    void EqSetDraw(int zorder, std::shared_ptr<TNumeric> N, int X, int Y, bool withBrackets) const;

    Metrics IntegralGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets) const;
    void IntegralDraw(int zorder, std::shared_ptr<TNumeric> N, int X, int Y, bool withBrackets) const;

    Metrics UnionGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets) const;
    void UnionDraw(int zorder, std::shared_ptr<TNumeric> N, int X, int Y, bool withBrackets) const;

    Metrics IntersectionGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets) const;
    void IntersectionDraw(int zorder, std::shared_ptr<TNumeric> N, int X, int Y, bool withBrackets) const;

    Metrics DerivGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets) const;
    void DerivDraw(int zorder, std::shared_ptr<TNumeric> N, int X, int Y, bool withBrackets) const;

    bool simplify_{false};

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
    void invalidateRects() const {  /// \todo: remove const
        metrics_cache_.clear();
        zrect_cache_.clear();
    }
    std::shared_ptr<TPaintCanvas> canvas_;

    Metrics PrettyGetTextRectangleCached(std::shared_ptr<TNumeric> N, bool WithBrackets) const {
        return metrics_cache_[N];
    }

  public:
    std::shared_ptr<TNumeric> underMouse{nullptr};
    TFormulaPlotter(std::shared_ptr<TPaintCanvas> Canvas, bool simplify);
    void setCanvas(std::shared_ptr<TPaintCanvas> Canvas) {
        canvas_ = Canvas;
    }  // painter should be created inside PaintEvent
    virtual ~TFormulaPlotter() {}

    // Если Simplify == true, то функции рисуют формулу в хорошем виде, проводя некоторые преобразования. Иначе без преобразований
    // шрифт надо выбирать вручную, вызвав заранее     Canvas->Font = Canvas->FormulaFont;
    Metrics PrettyGetTextRectangle(std::shared_ptr<const TNumeric> N, bool withBrackets) const;
    Metrics PrettyGetTextRectangle(std::shared_ptr<TNumeric> N, bool withBrackets) const;
    Metrics PrettyDrawAtBaseLeft(std::shared_ptr<const TNumeric> N, int X, int Y, bool withBrackets) const;
    Metrics PrettyDrawAtBaseLeft(std::shared_ptr<TNumeric> N, int X, int Y, bool withBrackets) const;

    // функции рисуют формулу после некоторых преобразований, упрощающих её вид
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
        TFormulaPlotter formulaPlotter(Canvas, false);
        return formulaPlotter.GetTextRectangle(numeric_, MaxWidth);
    }
    virtual void DrawAtBaseLeft(std::shared_ptr<TPaintCanvas> Canvas, int X, int Y, int MaxWidth = -1) const {
        TFormulaPlotter formulaPlotter(Canvas, false);
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
