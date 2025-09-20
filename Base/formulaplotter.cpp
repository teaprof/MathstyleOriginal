#include <iostream>
#include "formulaplotter.h"
#include "symbols.h"

TFormulaPlotter::TFormulaPlotter(std::shared_ptr<TPaintCanvas> Canvas) : canvas_(Canvas) {
}

Metrics TFormulaPlotter::PrettyGetTextRectangle(std::shared_ptr<TNumeric> N, bool NeedBrackets, bool Simplify) const
{
    string AStr;
    Metrics res;
    switch(N->operation)
    {
    case OperatorConst:
        if(N->K.length() != 0)
        {
            if(N->K.find("_") != string::npos)
            {
                size_t pos = N->K.find("_");
                TNumeric Nsubs = MakeSubscript(TNumeric(N->K.substr(0, pos)), TNumeric(N->K.substr(pos+1)));
                res = PrettyGetTextRectangle(std::make_shared<TNumeric>(std::move(Nsubs)), false, Simplify);
            } else {
                AStr = Recognize(N->K);
                //проверяем отрицательность числа
                double f;
                if(sscanf(AStr.c_str(), "%lf", &f) > 0) //todo: OPTIMIZE HERE
                {
                    //отрицательное число всегда в скобках
                    if(f < 0) AStr = string("(")+AStr+")";
                }
                //}
                res.Width = canvas_->TextWidth(AStr);
                res.Height = canvas_->TightTextHeight(AStr);
                res.Depth = canvas_->TightTextDepth(AStr);
            };
        } else {
            AStr = "0";
            res.Width = canvas_->TextWidth(AStr);
            res.Height = canvas_->TightTextHeight(AStr);
            res.Depth = canvas_->TightTextDepth(AStr);
        };
        break;
    case OperatorInline:  res = InlineGetTextRectangle(N, NeedBrackets, Simplify); break;
    case OperatorInterval:
    case OperatorIntervalSegment:
    case OperatorSegmentInterval:
    case OperatorSegment: res = IntervalGetTextRectangle(N, NeedBrackets, Simplify);
        break;
    case OperatorLess:
    case OperatorLessOrEqual:
    case OperatorGreater:
    case OperatorGreaterOrEqual:
    case OperatorEqual: res = EqualGetTextRectangle(N, NeedBrackets, Simplify);
        break;
    case OperatorSum: res = SumGetTextRectangle(N, NeedBrackets, Simplify);
        break;
    case OperatorMinus: res = MinusGetTextRectangle(N, NeedBrackets, Simplify);
        break;
    case OperatorBelongsTo: res = BelongsToGetTextRectangle(N, NeedBrackets, Simplify);
        break;
    case OperatorProd: res = ProdGetTextRectangle(N, NeedBrackets, Simplify);
        break;
    case OperatorFrac: res = FracGetTextRectangle(N, NeedBrackets, Simplify);
        break;
    case OperatorSqrt: res = SqrtGetTextRectangle(N, NeedBrackets, Simplify);
        break;
    case OperatorSupIndex:
    case OperatorPow: res = PowGetTextRectangle(N, NeedBrackets, Simplify);
        break;
    case OperatorSubIndex: res = SubIndexGetTextRectangle(N, NeedBrackets, Simplify);
        break;
    case OperatorEqSet: res = EqSetGetTextRectangle(N, NeedBrackets, Simplify);
        break;
    case OperatorEqSystem: res = EqSystemGetTextRectangle(N, NeedBrackets, Simplify);
        break;
    case OperatorUnion: res = UnionGetTextRectangle(N, NeedBrackets, Simplify);
        break;
    case OperatorIntersection: res = IntersectionGetTextRectangle(N, NeedBrackets, Simplify);
        break;
    case OperatorDeriv: res = DerivGetTextRectangle(N, NeedBrackets, Simplify);
        break;
    case OperatorPlusMinus: res = PlusMinusGetTextRectangle(N, NeedBrackets, Simplify);
        break;
    case OperatorIntegral: res = IntegralGetTextRectangle(N, NeedBrackets, Simplify);
        break;
    case OperatorAbs: res = AbsGetTextRectangle(N, NeedBrackets, Simplify);
        break;
    case OperatorLog:
    case OperatorLn:
    case OperatorLg:
    case OperatorSin:
    case OperatorCos:
    case OperatorTg:
    case OperatorCtg:
    case OperatorArcsin:
    case OperatorArccos:
    case OperatorArctg:
    case OperatorSh:
    case OperatorCh:
    case OperatorExp:
    case OperatorSign:
    case OperatorSmallO:
    case OperatorBigO:
        res = FunctionGetTextRectangle(N, NeedBrackets, Simplify);
        break;
    };
    return res;
}

void TFormulaPlotter::DrawAtBaseLeft(std::shared_ptr<TNumeric> N, int X, int Y, int MaxWidth) const
{
    Q_UNUSED(MaxWidth);
    //MaxWidth не используется
    canvas_->Font = canvas_->FormulaFont;
    PrettyDrawAtBaseLeft(N, X, Y, false, false);
}

Metrics TFormulaPlotter::PrettyDrawAtBaseLeft(std::shared_ptr<TNumeric> N, int X, int Y, bool NeedBrackets, bool Simplify) const
//Y = baseline
{
    const Metrics metrics = PrettyGetTextRectangle(N, NeedBrackets, Simplify);
    canvas_->Pen.setColor(getFontColor(N));
    switch(N->operation)
    {
    case OperatorConst:
        if(N->K.length() != 0)
        {
            if(N->K.find("_") != string::npos)
            {
                size_t pos = N->K.find("_");
                TNumeric Nsubs = MakeSubscript(TNumeric(N->K.substr(0, pos)), TNumeric(N->K.substr(pos+1)));
                return PrettyDrawAtBaseLeft(std::make_shared<TNumeric>(std::move(Nsubs)), X, Y, false, Simplify);
            } else {
                string AStr = Recognize(N->K);
                //Если требуются скобки, то ставим их. Но если число положительное - то скобки никогда не ставятся
                /*                    if(NeedBrackets)
                    {*/
                //проверяем отрицательность числа
                double f;
                if(sscanf(AStr.c_str(), "%lf", &f) > 0)
                {
                    //отрицательное число всегда в скобках
                    if(f < 0) AStr = string("(")+AStr+")";
                }
                //                   }
                canvas_->TextOutA(X, Y, AStr);
            };
        } else {
            canvas_->Rectangle(X+1, Y-metrics.Height+2, X+metrics.Width-1, Y+metrics.Depth-2);
        };
        break;
    case OperatorInline:  InlineDraw(N, X, Y, metrics, NeedBrackets, Simplify); break;
    case OperatorInterval:
    case OperatorIntervalSegment:
    case OperatorSegmentInterval:
    case OperatorSegment: IntervalDraw(N, X, Y, metrics, NeedBrackets, Simplify); break;
    case OperatorLess:
    case OperatorLessOrEqual:
    case OperatorGreater:
    case OperatorGreaterOrEqual:
    case OperatorEqual:  EqualDraw(N, X, Y, metrics, NeedBrackets, Simplify);
        break;
    case OperatorSum:  SumDraw(N, X, Y, metrics, NeedBrackets, Simplify);
        break;
    case OperatorMinus:  MinusDraw(N, X, Y, metrics, NeedBrackets, Simplify);
        break;
    case OperatorBelongsTo:  BelongsToDraw(N, X, Y, metrics, NeedBrackets, Simplify);
        break;
    case OperatorProd: ProdDraw(N, X, Y, metrics, NeedBrackets, Simplify);
        break;
    case OperatorFrac: FracDraw(N, X, Y, metrics, NeedBrackets, Simplify);
        break;
    case OperatorSqrt: SqrtDraw(N, X, Y, metrics, NeedBrackets, Simplify);
        break;
    case OperatorSupIndex:
    case OperatorPow: PowDraw(N, X, Y, metrics, NeedBrackets, Simplify);
        break;
    case OperatorSubIndex: SubIndexDraw(N, X, Y, metrics, NeedBrackets, Simplify);
        break;
    case OperatorEqSet: EqSetDraw(N, X, Y, metrics, NeedBrackets, Simplify);
        break;
    case OperatorEqSystem: EqSystemDraw(N, X, Y, metrics, NeedBrackets, Simplify);
        break;
    case OperatorUnion: UnionDraw(N, X, Y, metrics, NeedBrackets, Simplify);
        break;
    case OperatorIntersection: IntersectionDraw(N, X, Y, metrics, NeedBrackets, Simplify);
        break;
    case OperatorDeriv: DerivDraw(N, X, Y, metrics, NeedBrackets, Simplify);
        break;
    case OperatorPlusMinus: PlusMinusDraw(N, X, Y, metrics, NeedBrackets, Simplify);
        break;
    case OperatorIntegral: IntegralDraw(N, X, Y, metrics, NeedBrackets, Simplify);
        break;
    case OperatorAbs: AbsDraw(N, X, Y, metrics, NeedBrackets, Simplify);
        break;
    case OperatorLog:
    case OperatorLn:
    case OperatorLg:
    case OperatorSin:
    case OperatorCos:
    case OperatorTg:
    case OperatorCtg:
    case OperatorArcsin:
    case OperatorArccos:
    case OperatorArctg:
    case OperatorExp:
    case OperatorSh:
    case OperatorCh:
    case OperatorSign:
    case OperatorSmallO:
    case OperatorBigO:
        FunctionDraw(N, X, Y, metrics, NeedBrackets, Simplify);
        break;
    };

    std::optional<QColor> border = getBorderColor(N);
    if(border.has_value())
    {
        canvas_->Pen.setColor(border.value());
        canvas_->Rectangle(X - 1, Y + metrics.Depth + 1, X + metrics.Width + 1, Y - metrics.Height - 1);
    };

#ifdef __DEBUG__
    cout<<CodeBasic(N)<<endl;
#endif
    return metrics;
};

void TFormulaPlotter::DrawBracket(char Bracket, int X, int Y, int Height, int Depth) const
{
    string Str;
    Str += Bracket;
    QFont Temp = canvas_->Font;
    canvas_->Font.setWeight(QFont::Light);
    int dBaseLine = canvas_->SetTextHPlusD(Str, Height, Depth);
    //int RealH = canvas_->TextHeight(Str);
    //int RealD = canvas_->TextDepth(Str);
    //int W = canvas_->TextWidth(Str);

    canvas_->TextOutA(X, Y-dBaseLine, Str);
    //   canvas_->TextOutA(X, Y-RealD, Str);
    canvas_->Font = Temp;
    std::cout<<Height<<" "<<Depth<<std::endl;
};
int TFormulaPlotter::GetBracketWidth(char Bracket, int Height, int Depth) const
{
    Q_UNUSED(Depth);
    Q_UNUSED(Height);
    string Str;
    Str += Bracket;
    QFont Temp = canvas_->Font;
    canvas_->Font.setWeight(QFont::Light);
    //    int dBaseLine = canvas_->SetTextHPlusD(Str, Height, Depth);
    int    res = canvas_->TextWidth(Str);
    canvas_->Font = Temp;
    return res;
};

void TFormulaPlotter::DrawBracket(string Bracket, int X, int Y, int Height, int Depth) const
{
    string Str;
    Str += Bracket;
    QFont Temp = canvas_->Font;
    canvas_->Font.setWeight(QFont::Light);
    int dBaseLine = canvas_->SetTextHPlusD(Bracket, Height, Depth);
    //    canvas_->TextOutRect(X, Y-Height, X+W, Y+Depth+5, Str);
    canvas_->TextOutA(X, Y - dBaseLine, Str);
#ifdef __DEBUG__
    int W, RealH, RealD;
    canvas_->TightTextWHD(Str, &W, &RealH, &RealD);
    //canvas_->Rectangle(X, Y-res.Height - dBaseLine, X+W, Y+Depth, Qt::gray);
    //canvas_->Rectangle(X, Y-RealH - dBaseLine, X+W, Y+RealD - dBaseLine, Qt::red);
#endif
    canvas_->Font = Temp;
};
int TFormulaPlotter::GetBracketWidth(string Bracket, int Height, int Depth) const
{
    string Str;
    Str += Bracket;
    QFont Temp = canvas_->Font;
    canvas_->Font.setWeight(QFont::Light);
    canvas_->SetTextHPlusD(Bracket, Height, Depth);
    int    res = canvas_->TextWidth(Str);
    //int    res = canvas_->TightTextWidth(Str);
    canvas_->Font = Temp;
    return res;
}


Metrics TFormulaPlotter::FracGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const
{
    Metrics res;
    int MidLine = canvas_->TextHeight("1")/2;
    Metrics metrics_num = PrettyGetTextRectangle(N->operands[0], false, Simplify);
    res.Height = metrics_num.Height + metrics_num.Depth + 2 + MidLine;
    if(res.Width < metrics_num.Width)
        res.Width = metrics_num.Width;
    Metrics metrics_denom = PrettyGetTextRectangle(N->operands[1], false, Simplify);
    res.Depth = metrics_denom.Height + metrics_denom.Depth + 2 - MidLine;
    if(res.Depth < 0)
        res.Depth = 0;
    if(res.Width < metrics_denom.Width)
        res.Width = metrics_denom.Width;
    if(WithBrackets)
    {
        res.Width += GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), res.Height, res.Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), res.Height, res.Depth);
    }
    return res;
}
void TFormulaPlotter::FracDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const
{
    Metrics metrics1 = FracGetTextRectangle(N, false, Simplify);
    int WidthOfFrac = metrics1.Width;
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), X, Y, metrics.Height, metrics.Depth);
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), X + metrics.Width - GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), metrics.Height, metrics.Depth), Y, metrics.Height, metrics.Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), metrics.Height, metrics.Depth);
    };

    int MidLine = Y - canvas_->TextHeight("1")/2;

    metrics1 = PrettyGetTextRectangle(N->operands[0], false, Simplify);
    PrettyDrawAtBaseLeft(N->operands[0], X - metrics1.Width/2 + WidthOfFrac/2, MidLine - metrics1.Depth - 2, false, Simplify);

    metrics1 = PrettyGetTextRectangle(N->operands[1], false, Simplify);
    PrettyDrawAtBaseLeft(N->operands[1], X - metrics1.Width/2 + WidthOfFrac/2, MidLine + metrics1.Height + 2, false, Simplify);

    canvas_->Line(X, MidLine, X+WidthOfFrac, MidLine, Qt::black);
}

//==============================================================================

Metrics TFormulaPlotter::FunctionGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const
{
    Metrics res;
    string Name;
    switch(N->operation)
    {
    case OperatorLog: Name = "log"; break;
    case OperatorLn: Name = "ln"; break;
    case OperatorLg: Name = "lg"; break;
    case OperatorSin: Name = "sin"; break;
    case OperatorCos: Name = "cos"; break;
    case OperatorTg: Name = "tg"; break;
    case OperatorCtg: Name = "ctg"; break;
    case OperatorArcsin: Name = "arcsin"; break;
    case OperatorArccos: Name = "arccos"; break;
    case OperatorArctg: Name = "arctg"; break;
    case OperatorExp: Name = "exp"; break;
    case OperatorSh: Name = "sh"; break;
    case OperatorCh: Name = "ch"; break;
    case OperatorSign: Name = "sign"; break;
    case OperatorSmallO: Name = "o"; break;
    case OperatorBigO: Name = "O"; break;
    default: throw "TFormulaPlotter::FunctionGetTextRectangle: Unknown Function";
    };
    QFont FTemp = canvas_->Font;
    canvas_->Font.setBold(true);
    res.Width += canvas_->TextWidth(Name) + canvas_->TextWidth(" ");
    res.Height = canvas_->TextHeight(Name);
    res.Depth = canvas_->TextDepth(Name);
    canvas_->Font = FTemp;
    if(N->operation == OperatorLog)
    {
        QFont Font;
        Font = DecreaseFont(canvas_);
        Metrics metrics2 = PrettyGetTextRectangle(N->operands[1], true, Simplify);

        canvas_->Font = Font;
        res.Width += metrics2.Width;
        //int dres.Depth = H2 + D2 - Height/2;
        int d2 = metrics2.Height + metrics2.Depth;
        if(d2 > res.Depth)
            res.Depth = d2;
    }
    Metrics metrics1 = PrettyGetTextRectangle(N->operands[1], true, Simplify);
    res.Width += metrics1.Width;
    if(res.Height < metrics1.Height)
        res.Height = metrics1.Height;
    if(res.Depth < metrics1.Depth)
        res.Depth = metrics1.Depth;
    if(WithBrackets)
    {
        res.Width += GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), res.Height, res.Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), res.Height, res.Depth);
    };
    return res;
}

void TFormulaPlotter::FunctionDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const
{
    string Name;
    switch(N->operation)
    {
    case OperatorLog: Name = "log"; break;
    case OperatorLn: Name = "ln"; break;
    case OperatorLg: Name = "lg"; break;
    case OperatorSin: Name = "sin"; break;
    case OperatorCos: Name = "cos"; break;
    case OperatorTg: Name = "tg"; break;
    case OperatorCtg: Name = "ctg"; break;
    case OperatorArcsin: Name = "arcsin"; break;
    case OperatorArccos: Name = "arccos"; break;
    case OperatorArctg: Name = "arctg"; break;
    case OperatorExp: Name = "exp"; break;
    case OperatorSh: Name = "sh"; break;
    case OperatorCh: Name = "ch"; break;
    case OperatorSign: Name = "sign"; break;
    case OperatorBigO: Name = "O"; break;
    case OperatorSmallO: Name = "o"; break;
    default: throw "TFormulaPlotter::FunctionGetTextRectangle: Unknown Function";
    };
#ifdef __DEBUG__
    cout<<CodeBasic(N)<<endl;
#endif

    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), X, Y, metrics.Height, metrics.Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), metrics.Height, metrics.Depth);
    };
    QFont FTemp = canvas_->Font;
    canvas_->Font.setBold(true);
    canvas_->TextOutA(X, Y, Name);
    X += canvas_->TextWidth(Name) + canvas_->TextWidth(" ")/2;
    canvas_->Font = FTemp;

    if(N->operation == OperatorLog)
    {
        QFont Font = DecreaseFont(canvas_);
        Metrics metrics1 = PrettyGetTextRectangle(N->operands[1], true, Simplify);
        PrettyDrawAtBaseLeft(N->operands[1], X, Y + metrics1.Height, true, Simplify);
        canvas_->Font = Font;
        X += metrics1.Width;
    };
    Metrics metrics2 = PrettyGetTextRectangle(N->operands[0], true, Simplify);
    PrettyDrawAtBaseLeft(N->operands[0], X, Y, true, Simplify);
    X+= metrics2.Width;

    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), X, Y, metrics.Height, metrics.Depth);
}

//==============================================================================
Metrics TFormulaPlotter::StrGetTextRectangle(std::shared_ptr<TNumeric> N, const char* Str, bool WithBrackets, bool Simplify) const
{
    Metrics res;
    for(size_t i = 0; i < N->operands.size(); i++)
    {
        bool NeedBrackets = false;
        if(TNumeric::CompareOperatorsPriority(N->operation, N->operands[i]->operation)>0)
            NeedBrackets = true;
        Metrics metrics = PrettyGetTextRectangle(N->operands[i], NeedBrackets, Simplify);
        res.Width += metrics.Width;
        if(metrics.Height > res.Height)
            res.Height = metrics.Height;
        if(metrics.Depth > res.Depth)
            res.Depth = metrics.Depth;
    };
    res.Width += (N->operands.size()-1)*(canvas_->TextWidth(Str) + canvas_->TextWidth(" "));
    if(WithBrackets)
    {
        res.Width += GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), res.Height, res.Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), res.Height, res.Depth);
    };
    return res;
}
void TFormulaPlotter::StrDraw(std::shared_ptr<TNumeric> N, const char* Str, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const
{
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), X, Y, metrics.Height, metrics.Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), metrics.Height, metrics.Depth);
    };
    for(size_t i = 0; i < N->operands.size(); i++)
    {
        bool NeedBrackets = false;
        if(TNumeric::CompareOperatorsPriority(N->operation, N->operands[i]->operation)>0)
            NeedBrackets = true;
        Metrics metrics1 = PrettyGetTextRectangle(N->operands[i], NeedBrackets, Simplify);
        PrettyDrawAtBaseLeft(N->operands[i], X, Y, NeedBrackets, Simplify);
        X = X + metrics1.Width;
        if(i+1 < N->operands.size())
        {
            //          int Plusres.Height = canvas_->TextHeight(Str);
            canvas_->TextOutA(X, Y, Str);
            X += canvas_->TextWidth(Str);
        };
    };
    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), X, Y, metrics.Height, metrics.Depth);
}
//==============================================================================
//==============================================================================
Metrics TFormulaPlotter::IntervalGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const
{
    Metrics res;
    Q_UNUSED(WithBrackets);
    for(size_t i = 0; i < N->operands.size(); i++)
    {
        Metrics metrics = PrettyGetTextRectangle(N->operands[i], false, Simplify);
        res.Width += metrics.Width;
        if(metrics.Height > res.Height)
            res.Height = metrics.Height;
        if(metrics.Depth > res.Depth)
            res.Depth = metrics.Depth;
    };
    res.Width += (N->operands.size()-1)*canvas_->TextWidth(",");
    string LeftBracket = UniCode2UTF8String(MyLeftParenthesis),RightBracket = UniCode2UTF8String(MyRightParenthesis);
    switch(N->operation)
    {
    case OperatorInterval: LeftBracket=UniCode2UTF8String(MyLeftParenthesis); RightBracket=UniCode2UTF8String(MyRightParenthesis); break;
    case OperatorSegmentInterval: LeftBracket='['; RightBracket=UniCode2UTF8String(MyRightParenthesis); break;
    case OperatorIntervalSegment: LeftBracket=UniCode2UTF8String(MyLeftParenthesis); RightBracket=']'; break;
    case OperatorSegment: LeftBracket='['; RightBracket=']'; break;
    };

    res.Width += GetBracketWidth(LeftBracket, res.Height, res.Depth) + GetBracketWidth(RightBracket, res.Height, res.Depth);
    return res;
};
void TFormulaPlotter::IntervalDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(NeedBrackets);
    string LeftBracket = UniCode2UTF8String(MyLeftParenthesis);
    string RightBracket = UniCode2UTF8String(MyRightParenthesis);
    switch(N->operation)
    {
    case OperatorInterval:
        LeftBracket=UniCode2UTF8String(MyLeftParenthesis);
        RightBracket=UniCode2UTF8String(MyRightParenthesis);
        break;
    case OperatorSegmentInterval:
        LeftBracket='[';
        RightBracket=UniCode2UTF8String(MyRightParenthesis);
        break;
    case OperatorIntervalSegment:
        LeftBracket=UniCode2UTF8String(MyLeftParenthesis);
        RightBracket=']';
        break;
    case OperatorSegment:
        LeftBracket='['; RightBracket=']';
        break;
    };
    DrawBracket(LeftBracket, X, Y, metrics.Height, metrics.Depth);
    X = X + GetBracketWidth(LeftBracket, metrics.Height, metrics.Depth);
    for(size_t i = 0; i < N->operands.size(); i++)
    {
        Metrics metrics1 = PrettyGetTextRectangle(N->operands[i], false, Simplify);
        PrettyDrawAtBaseLeft(N->operands[i], X, Y, false, Simplify);
        X = X + metrics1.Width;
        if(i+1 < N->operands.size())
        {
            canvas_->TextOutA(X, Y, ",");
            X = X + canvas_->TextWidth(",");
        };
    }
    DrawBracket(RightBracket, X, Y, metrics.Height, metrics.Depth);
}
//==============================================================================
//==============================================================================
Metrics TFormulaPlotter::InlineGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const
{
    Metrics res;
    for(size_t i = 0; i < N->operands.size(); i++)
    {
        Metrics metrics = PrettyGetTextRectangle(N->operands[i], false, Simplify);
        res.Width += metrics.Width;
        if(metrics.Height > res.Height)
            res.Height = res.Height;
        if(metrics.Depth > res.Depth)
            res.Depth = metrics.Depth;
    };
    if(WithBrackets)
        res.Width += GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), res.Height, res.Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), res.Height, res.Depth);
    return res;
}
void TFormulaPlotter::InlineDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const
{
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), X, Y, metrics.Height, metrics.Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), metrics.Height, metrics.Depth);
    };
    for(size_t i = 0; i < N->operands.size(); i++)
    {
        Metrics metrics1 = PrettyGetTextRectangle(N->operands[i], false, Simplify);
        PrettyDrawAtBaseLeft(N->operands[i], X, Y, false, Simplify);
        X = X + metrics1.Width;
    };
    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), X, Y, metrics.Height, metrics.Depth);
}
//==============================================================================
//==============================================================================
Metrics TFormulaPlotter::SumGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const
{
    Metrics res;
    TNumeric Temp(*N); //работаем с Temp, так как this = const
    string StrPlus = UniCode2UTF8String(MyPlusSign);
    string StrMinus = UniCode2UTF8String(MyMinusSign);
    vector<size_t> Exclude; //слагаемые, равные нулю, их лучше всего исключить
#ifdef __DEBUG__
    cout<<CodeBasic(N)<<endl;
#endif
    if(Simplify)
    {
        for(size_t i = 0; i < Temp.operands.size(); i++)
        {
            if(Temp.operands[i]->operation == OperatorConst && Temp.operands[i]->K == "0")Exclude.push_back(i);
            if(Temp.operands[i]->operation == OperatorFrac)
            {
                if(Temp.operands[i]->operands[0]->operation==OperatorConst && Temp.operands[i]->operands[0]->K == "0")Exclude.push_back(i);
            }
            if(Temp.operands[i]->operation == OperatorProd)
            {
                for(size_t j = 0; j < Temp.operands[i]->operands.size(); j++)
                    if(Temp.operands[i]->operands[j]->operation==OperatorConst && Temp.operands[i]->operands[j]->K == "0")
                    {
                        Exclude.push_back(i);
                        break;
                    };
            }
        }
        if(Exclude.size() == Temp.operands.size() && Temp.operands.size() > 0)
        //все слагаемые - нули, оставляем только одно из них
        {
            Exclude.erase(Exclude.begin());
        };
    };
    size_t Drawn = 0; //количество уже отрисованных слагаемых
    for(size_t i = 0; i < Temp.operands.size(); i++)
    {
        if(find(Exclude.begin(), Exclude.end(), i) != Exclude.end())
            continue;
        bool NeedBrackets = false;
        if(TNumeric::CompareOperatorsPriority(N->operation, Temp.operands[i]->operation)>0)
            NeedBrackets = true;
        vector<size_t> TakenMinuses; //Операнды, у которых минусы уже учтены в знаке перед произведением или дробью
        size_t MinusesCount = 0;

        if((Simplify) & (Drawn > 0))
        //Пытаемся вынести минус, эта операция не явлеятся thread-safe
        //у первого (Drawn = 0) слагаемого минус не выносится
        {
            if(Temp.operands[i]->operation == OperatorProd || Temp.operands[i]->operation == OperatorFrac)
            {
                for(size_t j = 0; j < Temp.operands[i]->operands.size(); j++)
                {
                    if(Temp.operands[i]->operands[j]->operation == OperatorConst)
                    {
                        if(Temp.operands[i]->operands[j]->K.length() > 0 && Temp.operands[i]->operands[j]->K[0]=='-')
                        {
                            TakenMinuses.push_back(j);
                            Temp.operands[i]->operands[j]->K.erase(Temp.operands[i]->operands[j]->K.begin());
                            MinusesCount++;
                        }
                    }
                }
            }
            if(Temp.operands[i]->operation == OperatorConst)
                if(Temp.operands[i]->K.length() > 0 && Temp.operands[i]->K[0]=='-')
                {
                    MinusesCount++;
                    Temp.operands[i]->K.erase(Temp.operands[i]->K.begin());
                }
        }
        //рисуем знак
        if(MinusesCount % 2 == 0)
        {
            //плюс, перед первым членом не ставится
            if(Drawn>0) res.Width += canvas_->TextWidth(StrPlus);
        }
        else
        {
            //минус
            res.Width += canvas_->TextWidth(StrMinus);
        }

        if(TNumeric::CompareOperatorsPriority(N->operation, Temp.operands[i]->operation)>0) NeedBrackets = true;
        Metrics metrics;
        if(Simplify)
        //если надо было упростить, то рисуем упрощенные слагаемые
        {
            metrics = PrettyGetTextRectangle(Temp.operands[i], NeedBrackets, Simplify);
        } else {
            //ничего не упрощали, тогда рисуем исходные слагаемые. При этом поле Active у этих слагаемых меняется и вытаскивается функцией PrettyDrawAtBaseLeft в корень дерева
            metrics = PrettyGetTextRectangle(N->operands[i], NeedBrackets, Simplify);
        }

        if(Simplify)
        {
            //возвращаем минус обратно
            if(Temp.operands[i]->operation == OperatorProd || Temp.operands[i]->operation == OperatorFrac)
            {
                for(size_t j = 0; j < TakenMinuses.size(); j++)
                    Temp.operands[i]->operands[TakenMinuses[j]]->K = string("-")+Temp.operands[i]->operands[TakenMinuses[j]]->K;
            }
            if(Temp.operands[i]->operation == OperatorConst && MinusesCount % 2 == 1)
            {
                Temp.operands[i]->K = string("-") + Temp.operands[i]->K;
            }
        };
        res.Width += metrics.Width;
        if(metrics.Height > res.Height)
            res.Height = metrics.Height;
        if(metrics.Depth > res.Depth)
            res.Depth = metrics.Depth;
        Drawn++;
    };
    if(WithBrackets)
    {
        res.Width += GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), res.Height, res.Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), res.Height, res.Depth);
    };
#ifdef __DEBUG__
    cout<<CodeBasic(N)<<endl;
#endif
    return res;
}
void TFormulaPlotter::SumDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const
{
#ifdef __DEBUG__
    cout<<CodeBasic(N)<<endl;
#endif
    TNumeric Temp(*N); //работаем с Temp, так как this = const
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), X, Y, metrics.Height, metrics.Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), metrics.Height, metrics.Depth);
    };
    vector<size_t> Exclude; //слагаемые, равные нулю, их лучше всего исключить
    if(Simplify)
    {
        for(size_t i = 0; i < Temp.operands.size(); i++)
        {
            if(Temp.operands[i]->operation == OperatorConst && Temp.operands[i]->K == "0")Exclude.push_back(i);
            if(Temp.operands[i]->operation == OperatorFrac)
            {
                if(Temp.operands[i]->operands[0]->operation==OperatorConst && Temp.operands[i]->operands[0]->K == "0")Exclude.push_back(i);
            }
            if(Temp.operands[i]->operation == OperatorProd)
            {
                for(size_t j = 0; j < Temp.operands[i]->operands.size(); j++)
                    if(Temp.operands[i]->operands[j]->operation==OperatorConst && Temp.operands[i]->operands[j]->K == "0")
                    {
                        Exclude.push_back(i);
                        break;
                    };
            }
        }
        if(Exclude.size() == Temp.operands.size() && Temp.operands.size() > 0)
        //все слагаемые - нули, оставляем только одно из них
        {
            Exclude.erase(Exclude.begin());
        };
    };

    string StrPlus = UniCode2UTF8String(MyPlusSign);
    string StrMinus = UniCode2UTF8String(MyMinusSign);
    size_t Drawn = 0; //количество уже отрисованных слагаемых

    for(size_t i = 0; i < Temp.operands.size(); i++)
    {
        if(find(Exclude.begin(), Exclude.end(), i) != Exclude.end())continue;
        bool NeedBrackets = false;
        if(TNumeric::CompareOperatorsPriority(N->operation, Temp.operands[i]->operation)>0) NeedBrackets = true;
        vector<size_t> TakenMinuses; //Операнды, у которых минусы уже учтены в знаке перед произведением или дробью
        size_t MinusesCount = 0 ;

        if(Simplify && Drawn > 0)
        //Пытаемся вынести минус, эта операция не явлеятся thread-safe
        //у первого (Drawn = 0) слагаемого минус не выносится
        {
            if(Temp.operands[i]->operation == OperatorProd || Temp.operands[i]->operation == OperatorFrac)
            {
                for(size_t j = 0; j < Temp.operands[i]->operands.size(); j++)
                {
                    if(Temp.operands[i]->operands[j]->operation == OperatorConst)
                    {
                        if(Temp.operands[i]->operands[j]->K.length() > 0 && Temp.operands[i]->operands[j]->K[0]=='-')
                        {
                            TakenMinuses.push_back(j);
                            Temp.operands[i]->operands[j]->K.erase(Temp.operands[i]->operands[j]->K.begin());
                            MinusesCount++;
                        }
                    }
                }
            }
            if(Temp.operands[i]->operation == OperatorConst)
                if(Temp.operands[i]->K.length() > 0 && Temp.operands[i]->K[0]=='-')
                {
                    MinusesCount++;
                    Temp.operands[i]->K.erase(Temp.operands[i]->K.begin());
                }
        }
        //рисуем знак
        if(MinusesCount % 2 == 0)
        {
            //плюс, перед первым членом не ставится
            if(Drawn>0)
            {
                canvas_->TextOutA(X, Y, StrPlus);
                X = X + canvas_->TextWidth(StrPlus);
            }
        }
        else
        {
            //минус, рисуется всегда
            canvas_->TextOutA(X, Y, StrMinus);
            X = X + canvas_->TextWidth(StrMinus);
        }

        if(TNumeric::CompareOperatorsPriority(N->operation, Temp.operands[i]->operation)>0) NeedBrackets = true;
        Metrics metrics1;
        if(Simplify)
        //если надо было упростить, то рисуем упрощенные слагаемые
        {
            metrics1 = PrettyGetTextRectangle(Temp.operands[i], NeedBrackets, Simplify);
            PrettyDrawAtBaseLeft(Temp.operands[i], X, Y, NeedBrackets, Simplify);
        } else {
            //ничего не упрощали, тогда рисуем исходные слагаемые. При этом поле Active у этих слагаемых меняется и вытаскивается функцией PrettyDrawAtBaseLeft в корень дерева
            metrics1 = PrettyGetTextRectangle(N->operands[i], NeedBrackets, Simplify);
            PrettyDrawAtBaseLeft(Temp.operands[i], X, Y, NeedBrackets, Simplify);
        }

        if(Simplify)
        {
            //возвращаем минус обратно
            if(Temp.operands[i]->operation == OperatorProd || Temp.operands[i]->operation == OperatorFrac)
            {
                for(size_t j = 0; j < TakenMinuses.size(); j++)
                    Temp.operands[i]->operands[TakenMinuses[j]]->K = string("-")+Temp.operands[i]->operands[TakenMinuses[j]]->K;
            }
            if(Temp.operands[i]->operation == OperatorConst && MinusesCount % 2 == 1)
            {
                Temp.operands[i]->K = string("-") + Temp.operands[i]->K;
            }
        };

        X = X + metrics1.Width;
        Drawn++;
    };
    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), X, Y, metrics.Height, metrics.Depth);
}
//==============================================================================
//==============================================================================
Metrics TFormulaPlotter::MinusGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const
{
    Metrics res;
    TNumeric Temp(*N); //работаем с Temp, так как this = const
#ifdef __DEBUG__
    cout<<CodeBasic(N)<<endl;
#endif
    string StrPlus = UniCode2UTF8String(MyPlusSign);
    string StrMinus = UniCode2UTF8String(MyMinusSign);
    for(size_t i = 0; i < Temp.operands.size(); i++)
    {
        bool NeedBrackets = false;
        if(TNumeric::CompareOperatorsPriority(N->operation, Temp.operands[i]->operation)>0) NeedBrackets = true;
        vector<size_t> TakenMinuses; //Операнды, у которых минусы уже учтены в знаке перед произведением или дробью
        size_t MinusesCount = 0;

        if(Simplify)
        //Пытаемся вынести минус, эта операция не явлеятся thread-safe
        {
            if(Temp.operands[i]->operation == OperatorProd || Temp.operands[i]->operation == OperatorFrac)
            {
                for(size_t j = 0; j < Temp.operands[i]->operands.size(); j++)
                {
                    if(Temp.operands[i]->operands[j]->operation == OperatorConst)
                    {
                        if(Temp.operands[i]->operands[j]->K.length() > 0 && Temp.operands[i]->operands[j]->K[0]=='-')
                        {
                            TakenMinuses.push_back(j);
                            Temp.operands[i]->operands[j]->K.erase(Temp.operands[i]->operands[j]->K.begin());
                            MinusesCount++;
                        }
                    }
                }
            }
            if(Temp.operands[i]->operation == OperatorConst)
                if(Temp.operands[i]->K.length() > 0 && Temp.operands[i]->K[0]=='-')
                {
                    MinusesCount++;
                    Temp.operands[i]->K.erase(Temp.operands[i]->K.begin());
                }
        }
        //рисуем знак
        if(MinusesCount % 2 == 1)
        {
            //минус, и ещё один минус перед вторым членом и далее
            if(i == 0) res.Width += canvas_->TextWidth(StrMinus);
            else res.Width += canvas_->TextWidth(StrPlus);
        } else {
            //плюс, перед первым членом знак не нужен
            //начиная со второго - знак минус
            if(i>0) res.Width += canvas_->TextWidth(StrMinus);
        }

        Metrics metrics;
        if(Simplify)
        //если надо было упростить, то рисуем упрощенные слагаемые
        {
            metrics = PrettyGetTextRectangle(Temp.operands[i], NeedBrackets, Simplify);
        } else {
            //ничего не упрощали, тогда рисуем исходные слагаемые. При этом поле Active у этих слагаемых меняется и вытаскивается функцией PrettyDrawAtBaseLeft в корень дерева
            metrics = PrettyGetTextRectangle(N->operands[i], NeedBrackets, Simplify);
        }


        if(Simplify)
        {
            //возвращаем минус обратно
            if(Temp.operands[i]->operation == OperatorProd || Temp.operands[i]->operation == OperatorFrac)
            {
                for(size_t j = 0; j < TakenMinuses.size(); j++)
                    Temp.operands[i]->operands[TakenMinuses[j]]->K = string("-")+Temp.operands[i]->operands[TakenMinuses[j]]->K;
            }
            if(Temp.operands[i]->operation == OperatorConst && (MinusesCount % 2 == 1))
            {
                Temp.operands[i]->K = string("-") + Temp.operands[i]->K;
            }
        };
        res.Width += metrics.Width;
        if(metrics.Height > res.Height)
            res.Height = metrics.Height;
        if(metrics.Depth > res.Depth)
            res.Depth = metrics.Depth;
    };
    if(WithBrackets)
    {
        res.Width += GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), res.Height, res.Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), res.Height, res.Depth);
    };
    return res;
}
void TFormulaPlotter::MinusDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const
{
    TNumeric Temp(*N); //работаем с Temp, так как this = const
#ifdef __DEBUG__
    cout<<CodeBasic(N)<<endl;
#endif
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), X, Y, metrics.Height, metrics.Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), metrics.Height, metrics.Depth);
    };
    string StrPlus = UniCode2UTF8String(MyPlusSign);
    string StrMinus = UniCode2UTF8String(MyMinusSign);
    for(size_t i = 0; i < Temp.operands.size(); i++)
    {
        bool NeedBrackets = false;
        if(TNumeric::CompareOperatorsPriority(N->operation, Temp.operands[i]->operation)>0) NeedBrackets = true;
        vector<size_t> TakenMinuses; //Операнды, у которых минусы уже учтены в знаке перед произведением или дробью
        size_t MinusesCount = 0 ;

        if(Simplify)
        //Пытаемся вынести минус, эта операция не явлеятся thread-safe
        //todo: скопировать из SumDraw
        {
            if(Temp.operands[i]->operation == OperatorProd || Temp.operands[i]->operation == OperatorFrac)
            {
                for(size_t j = 0; j < Temp.operands[i]->operands.size(); j++)
                {
                    if(Temp.operands[i]->operands[j]->operation == OperatorConst)
                    {
                        if(Temp.operands[i]->operands[j]->K.length() > 0 && Temp.operands[i]->operands[j]->K[0]=='-')
                        {
                            TakenMinuses.push_back(j);
                            Temp.operands[i]->operands[j]->K.erase(Temp.operands[i]->operands[j]->K.begin());
                            MinusesCount++;
                        }
                    }
                }
            }
            if(Temp.operands[i]->operation == OperatorConst)
                if(Temp.operands[i]->K.length() > 0 && Temp.operands[i]->K[0]=='-')
                {
                    MinusesCount++;
                    Temp.operands[i]->K.erase(Temp.operands[i]->K.begin());
                }
        }
        //рисуем знак
        if(MinusesCount % 2 == 1)
        {
            //минус, и ещё один минус перед вторым членом и далее
            if(i == 0)
            {
                canvas_->TextOutA(X, Y, StrMinus);
                X = X + canvas_->TextWidth(StrMinus);
            } else  {
                canvas_->TextOutA(X, Y, StrPlus);
                X = X + canvas_->TextWidth(StrPlus);
            }
        }
        else
        {
            //плюс, перед первым членом знак не нужен
            //начиная со второго - знак минус
            if(i>0)
            {
                canvas_->TextOutA(X, Y, StrMinus);
                X = X + canvas_->TextWidth(StrMinus);
            }
        }

        Metrics metrics1;
        if(Simplify)
        //если надо было упростить, то рисуем упрощенные слагаемые
        {
            metrics1 = PrettyGetTextRectangle(Temp.operands[i], NeedBrackets, Simplify);
            PrettyDrawAtBaseLeft(Temp.operands[i], X, Y, NeedBrackets, Simplify);
        } else {
            //ничего не упрощали, тогда рисуем исходные слагаемые. При этом поле Active у этих слагаемых меняется и вытаскивается функцией PrettyDrawAtBaseLeft в корень дерева
            metrics1 = PrettyGetTextRectangle(N->operands[i], NeedBrackets, Simplify);
            PrettyDrawAtBaseLeft(N->operands[i], X, Y, NeedBrackets, Simplify);
        }

        if(Simplify)
        {
            //возвращаем минус обратно
            if(Temp.operands[i]->operation == OperatorProd || Temp.operands[i]->operation == OperatorFrac)
            {
                for(size_t j = 0; j < TakenMinuses.size(); j++)
                    Temp.operands[i]->operands[TakenMinuses[j]]->K = string("-")+Temp.operands[i]->operands[TakenMinuses[j]]->K;
            }
            if(Temp.operands[i]->operation == OperatorConst && (MinusesCount % 2== 1))
            {
                Temp.operands[i]->K = string("-") + Temp.operands[i]->K;
            }
        };
        X = X + metrics1.Width;
    };
    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), X, Y, metrics.Height, metrics.Depth);
};
//==============================================================================
//==============================================================================
Metrics TFormulaPlotter::UnionGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const
{
    char Sign[10];
    UniCode2UTF8(Sign, UnicodeUnion);
    return StrGetTextRectangle(N, Sign, WithBrackets, Simplify);
}

void TFormulaPlotter::UnionDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const
{
    char Sign[10];
    UniCode2UTF8(Sign, UnicodeUnion);
    StrDraw(N, Sign, X, Y, metrics, NeedBrackets, Simplify);
}

//==============================================================================
//==============================================================================
Metrics TFormulaPlotter::PlusMinusGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const
{
    char Sign[10];
    UniCode2UTF8(Sign, UnicodePlusMinus);
    return StrGetTextRectangle(N, Sign, WithBrackets, Simplify);
}
void TFormulaPlotter::PlusMinusDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const
{
    char Sign[10];
    UniCode2UTF8(Sign, UnicodePlusMinus);
    StrDraw(N, Sign, X, Y, metrics, NeedBrackets, Simplify);
}


//==============================================================================
//==============================================================================

Metrics TFormulaPlotter::IntersectionGetTextRectangle(std::shared_ptr<TNumeric> N,  bool WithBrackets, bool Simplify) const
{
    char Sign[10];
    UniCode2UTF8(Sign, UnicodeIntersection);
    return StrGetTextRectangle(N, Sign, WithBrackets, Simplify);
}

void TFormulaPlotter::IntersectionDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const
{
    char Sign[10];
    UniCode2UTF8(Sign, UnicodeIntersection);
    StrDraw(N, Sign, X, Y, metrics, NeedBrackets, Simplify);
}

//==============================================================================
//==============================================================================
Metrics TFormulaPlotter::ProdGetTextRectangle(std::shared_ptr<TNumeric> N,  bool WithBrackets, bool Simplify) const
{
    Metrics res;
    vector<size_t> ExcludeOperands;
    if(Simplify)
    {
        for(size_t j = 0; j < N->operands.size(); j++)
            if(N->operands[j]->operation == OperatorConst && N->operands[j]->K == "1") ExcludeOperands.push_back(j);
        if(ExcludeOperands.size() == N->operands.size() && ExcludeOperands.size() > 0) ExcludeOperands.erase(ExcludeOperands.begin());
    }
    char Str[10];
    UniCode2UTF8(Str, UnicodeMiddleDot);
    for(size_t i = 0; i < N->operands.size(); i++)
    {
        if(Simplify && find(ExcludeOperands.begin(), ExcludeOperands.end(), i) != ExcludeOperands.end()) continue;
        bool NeedBrackets = false;
        if(TNumeric::CompareOperatorsPriority(N->operation, N->operands[i]->operation)>0) NeedBrackets = true;
        Metrics metrics = PrettyGetTextRectangle(N->operands[i], NeedBrackets, Simplify);
        res.Width += metrics.Width;
        if(metrics.Height > res.Height)
            res.Height = metrics.Height;
        if(metrics.Depth > res.Depth)
            res.Depth = metrics.Depth;
        if(i+1 < N->operands.size() && !(Simplify && find(ExcludeOperands.begin(), ExcludeOperands.end(), i+1) != ExcludeOperands.end()))
            res.Width += canvas_->TextWidth(Str);
    };
    if(WithBrackets)
    {
        res.Width += GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), res.Height, res.Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), res.Height, res.Depth);
    };
    return res;
};
void TFormulaPlotter::ProdDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const
{
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), X, Y, metrics.Height, metrics.Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), metrics.Height, metrics.Depth);
    };
    vector<size_t> ExcludeOperands;
    if(Simplify)
    {
        for(size_t j = 0; j < N->operands.size(); j++)
            if(N->operands[j]->operation == OperatorConst && N->operands[j]->K == "1") ExcludeOperands.push_back(j);
        if(ExcludeOperands.size() == N->operands.size() && ExcludeOperands.size() > 0) ExcludeOperands.erase(ExcludeOperands.begin());
    }
    char Str[10];
    UniCode2UTF8(Str, UnicodeMiddleDot);
    for(size_t i = 0; i < N->operands.size(); i++)
    {
        if(Simplify && find(ExcludeOperands.begin(), ExcludeOperands.end(), i) != ExcludeOperands.end())
            continue;
        bool NeedBrackets = false;
        if(TNumeric::CompareOperatorsPriority(N->operation, N->operands[i]->operation)>0) NeedBrackets = true;
        Metrics metrics = PrettyGetTextRectangle(N->operands[i], NeedBrackets, Simplify);
        PrettyDrawAtBaseLeft(N->operands[i], X, Y, NeedBrackets, Simplify);
        X = X + metrics.Width;
        if(i+1 < N->operands.size() && !(Simplify && find(ExcludeOperands.begin(), ExcludeOperands.end(), i+1) != ExcludeOperands.end()))
        {
            //canvas_->TextOutA(X, Y, "*");
#ifdef __DEBUG__
            int W, H, D;
            W = canvas_->TextWidth(Str);
            H = canvas_->TextHeight(Str);
            D = canvas_->TextDepth(Str);
            canvas_->Line(X, Y, X+W, Y, QPen(Qt::red));
            canvas_->Rectangle(X, Y-H, X+W, Y+D, Qt::red);
#endif
            canvas_->TextOutA(X, Y, Str);
            X = X + canvas_->TextWidth(Str);
        };
    };
    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), X, Y, metrics.Height, metrics.Depth);
}
//==============================================================================
//==============================================================================
Metrics TFormulaPlotter::BelongsToGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const
{
    char Sign[10];
    UniCode2UTF8(Sign, UnicodeElementOf);
    return StrGetTextRectangle(N, Sign, WithBrackets, Simplify);
};
void TFormulaPlotter::BelongsToDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const
{
    char Sign[10];
    UniCode2UTF8(Sign, 0x2208);
    StrDraw(N, Sign, X, Y, metrics, NeedBrackets, Simplify);
};
//==============================================================================
Metrics TFormulaPlotter::EqualGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const
{
    string str;
    switch(N->operation)
    {
    case OperatorLess: str = UniCode2UTF8String(MyLessThanSign); break;
    case OperatorLessOrEqual:  str = UniCode2UTF8String(MyLessOrEqualSign); break;
    case OperatorGreater: str = UniCode2UTF8String(MyGreaterThanSign); break;
    case OperatorGreaterOrEqual: str = UniCode2UTF8String(MyGreaterOrEqualSign); break;
    case OperatorEqual:   str = UniCode2UTF8String(MyEqualSign); break;
    default: str = "?";
    }
    return StrGetTextRectangle(N, str.c_str(), WithBrackets, Simplify);
};
void TFormulaPlotter::EqualDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const
{
    string str;
    switch(N->operation)
    {
    case OperatorLess: str = UniCode2UTF8String(MyLessThanSign); break;
    case OperatorLessOrEqual:  str = UniCode2UTF8String(MyLessOrEqualSign); break;
    case OperatorGreater: str = UniCode2UTF8String(MyGreaterThanSign); break;
    case OperatorGreaterOrEqual: str = UniCode2UTF8String(MyGreaterOrEqualSign); break;
    case OperatorEqual:   str = UniCode2UTF8String(MyEqualSign); break;
    default: str = "?";
    }
    StrDraw(N, str.c_str(), X, Y, metrics, NeedBrackets, Simplify);
};
//==============================================================================
//==============================================================================
Metrics TFormulaPlotter::PowGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const
{
    Metrics metrics1 = PrettyGetTextRectangle(N->operands[0], true, Simplify);
    Metrics res = metrics1;
    QFont Font = DecreaseFont(canvas_);
    Metrics metrics2 = PrettyGetTextRectangle(N->operands[1], true, Simplify);
    canvas_->Font = Font;
    if(res.Height < metrics1.Height/2 + metrics2.Height + metrics2.Depth)
        res.Height = metrics1.Height/2 + metrics2.Height + metrics2.Depth;
    if(res.Depth < metrics2.Depth - metrics1.Height/2)
        res.Depth = metrics1.Depth - metrics1.Height/2;
    res.Width += metrics2.Width;
    if(WithBrackets)
    {
        res.Width += GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), res.Height, res.Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), res.Height, res.Depth);
    };
    return res;
};
void TFormulaPlotter::PowDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const
{
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), X, Y, metrics.Height, metrics.Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), metrics.Height, metrics.Depth);
    };
    Metrics metrics1 = PrettyGetTextRectangle(N->operands[0], true, Simplify);
    PrettyDrawAtBaseLeft(N->operands[0], X, Y, true, Simplify);
    X += metrics1.Width;
    QFont Font = DecreaseFont(canvas_);
    Metrics metrics2 = PrettyGetTextRectangle(N->operands[1], true, Simplify);
    PrettyDrawAtBaseLeft(N->operands[1], X, Y - metrics1.Height/2 - metrics2.Depth, true, Simplify);
    canvas_->Font = Font;
    X+=metrics2.Width;

    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), X , Y, metrics.Height, metrics.Depth);
};
//==============================================================================
//==============================================================================
Metrics TFormulaPlotter::AbsGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const
{
    Metrics res;
    Q_UNUSED(WithBrackets);
    Metrics metrics1 = PrettyGetTextRectangle(N->operands[0], false, Simplify);
    res = metrics1;
    res.Width += 2*GetBracketWidth("|", res.Height, res.Depth);
    return res;
};
void TFormulaPlotter::AbsDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(NeedBrackets);
    DrawBracket("|", X, Y, metrics.Height, metrics.Depth);
    X += GetBracketWidth("|", metrics.Height, metrics.Depth);

    Metrics metrics1 = PrettyGetTextRectangle(N->operands[0], false, Simplify);
    PrettyDrawAtBaseLeft(N->operands[0], X, Y, false, Simplify);
    X += metrics1.Width;

    DrawBracket("|", X , Y, metrics.Height, metrics.Depth);
};
//==============================================================================
//==============================================================================
Metrics TFormulaPlotter::DerivGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const
{
    Metrics res;
    res.Width = res.Height = res.Depth = 0;
    Metrics metrics1 = PrettyGetTextRectangle(N->operands[0], true, Simplify);
    res = metrics1;

    int W2 = canvas_->TextWidth("'");

    res.Width += W2;
    if(WithBrackets)
    {
        res.Width += GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), res.Height, res.Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), res.Height, res.Depth);
    }
    return res;
}
void TFormulaPlotter::DerivDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const
{
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), X, Y, metrics.Height, metrics.Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), metrics.Height, metrics.Depth);
    };
    Metrics metrics1 = PrettyGetTextRectangle(N->operands[0], true, Simplify);
    PrettyDrawAtBaseLeft(N->operands[0], X, Y, true, Simplify);
    X+=metrics1.Width;

    int W2 = canvas_->TextWidth("'");
    int H2 = canvas_->TextHeight("'");
    canvas_->TextOutA(X, Y - metrics1.Height + H2, "'");
    X += W2;


    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), X , Y, metrics.Height, metrics.Depth);
};
//==============================================================================
//==============================================================================
Metrics TFormulaPlotter::SubIndexGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const
{
    Metrics res;
    Metrics metrics = PrettyGetTextRectangle(N->operands[0], true, Simplify);
    res = metrics;
    QFont Font = DecreaseFont(canvas_);
    metrics = PrettyGetTextRectangle(N->operands[1], true, Simplify);
    canvas_->Font = Font;
    if(metrics.Height + metrics.Depth > 2*res.Depth)
        res.Depth = (metrics.Height + metrics.Depth)/2 + 1;

    res.Width += metrics.Width;
    if(WithBrackets)
    {
        res.Width += GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), res.Height, res.Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), res.Height, res.Depth);
    };
    return res;
};
void TFormulaPlotter::SubIndexDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const
{
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), X, Y, metrics.Height, metrics.Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), metrics.Height, metrics.Depth);
    };
    Metrics metrics1 = PrettyGetTextRectangle(N->operands[0], true, Simplify);
    PrettyDrawAtBaseLeft(N->operands[0], X, Y, true, Simplify);
    X += metrics1.Width;

    QFont Font = DecreaseFont(canvas_);
    Metrics metrics2 = PrettyGetTextRectangle(N->operands[1], true, Simplify);
    PrettyDrawAtBaseLeft(N->operands[1], X, Y + (metrics.Height - metrics.Depth)/2, true, Simplify);
    X += metrics2.Width;
    canvas_->Font = Font;

    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), X , Y, metrics.Height, metrics.Depth);
};
//==============================================================================
//==============================================================================
Metrics TFormulaPlotter::SqrtGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const
{
    Metrics res;
    Metrics metrics = PrettyGetTextRectangle(N->operands[0], false, Simplify);
    res = metrics;
    res.Width += 10;
    res.Height = res.Height + 10;
    if(WithBrackets)
    {
        res.Width += GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), res.Height, res.Depth) +GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), res.Height, res.Depth);
    };
    return res;
};
void TFormulaPlotter::SqrtDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const
{
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), X, Y, metrics.Height, metrics.Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), metrics.Height, metrics.Depth);
    };
    Metrics metrics1 = PrettyGetTextRectangle(N->operands[0], false, Simplify);
    auto [W, H, D] = metrics1;
    auto [Width, Height, Depth] = metrics;
    PrettyDrawAtBaseLeft(N->operands[0], X+10, Y, false, Simplify);
    QColor Color = canvas_->Pen.color();
    canvas_->Line(X, Y, X+2, Y, Color, 1);
    canvas_->Line(X+2, Y, X+5, Y + Depth, Color, 2);
    canvas_->Line(X+5, Y + Depth, X+9, Y - Height + 2, Color, 1);
    canvas_->Line(X+9, Y - Height + 2, X + 10 + W, Y - Height + 2, Color, 1);
    canvas_->Line(X + 10 + W, Y - Height + 2, X + 10 +W, Y - Height + 5, Color, 1);

    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), X+10+W, Y, Height, Depth);
};
//==============================================================================
Metrics  TFormulaPlotter::LinesGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets, int LeftMargin, bool Simplify) const
//одна или несколько строк, объединенных оператором совокупности или пересечения
{
    Metrics res;

    int InterLineSpacing = canvas_->TextHeight("A")/3;
    for(size_t i = 0; i < N->operands.size(); i++)
    {
        Metrics metrics = PrettyGetTextRectangle(N->operands[i], false, Simplify);
        if(metrics.Width > res.Width)
            res.Width = metrics.Width;
        res.Height += metrics.Depth + metrics.Height;
        if(i+1 < N->operands.size())
            res.Height += InterLineSpacing;
    };
    res.Depth = res.Height/2;
    res.Height = res.Height - res.Depth;
    if(WithBrackets)
    {
        res.Width += GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), res.Height, res.Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), res.Height, res.Depth);
    };

    res.Width += LeftMargin;
    return res;
};
void TFormulaPlotter::LinesDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, int LeftMargin, bool Simplify) const
{
    X += LeftMargin;
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), X, Y, metrics.Height, metrics.Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), metrics.Height, metrics.Depth);
    };
    int InterLineSpacing = canvas_->TextHeight("A")/3;
    for(size_t i = 0; i < N->operands.size(); i++)
    {
        Metrics metrics1 = PrettyGetTextRectangle(N->operands[i], false, Simplify);
        PrettyDrawAtBaseLeft(N->operands[i], X, Y-metrics.Height+metrics1.Height, false, Simplify);
        Y += metrics1.Height + metrics1.Depth;
        if(i+1 < N->operands.size())
            Y += InterLineSpacing;
    };
    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), X + metrics.Width - GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), metrics.Height, metrics.Depth), Y, metrics.Height, metrics.Depth);
};

Metrics TFormulaPlotter::EqSystemGetTextRectangle(std::shared_ptr<TNumeric> N,  bool WithBrackets, bool Simplify) const
{
    Metrics res = LinesGetTextRectangle(N, WithBrackets, 0, Simplify);
    res.Width += GetBracketWidth(UniCode2UTF8String(MyLeftCurlyBracket), res.Height, res. Depth);
    return res;
}
void TFormulaPlotter::EqSystemDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const
{
    string bracket = UniCode2UTF8String(MyLeftCurlyBracket);
    DrawBracket(bracket, X, Y, metrics.Height, metrics.Depth);
    int BW = GetBracketWidth(bracket, metrics.Height, metrics.Depth);
    X+=BW;
    LinesDraw(N, X, Y, metrics, NeedBrackets, 0, Simplify);
}
Metrics TFormulaPlotter::EqSetGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const
{
    Metrics res;
    res = LinesGetTextRectangle(N, WithBrackets, 0, Simplify);
    res.Width += GetBracketWidth(UniCode2UTF8String(MyLeftCurlyBracket), res.Height, res.Depth);
    return res;
};
void TFormulaPlotter::EqSetDraw(std::shared_ptr<TNumeric>N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const
{
    string bracket = UniCode2UTF8String(MyLeftCurlyBracket);
    DrawBracket(bracket, X, Y, metrics.Height, metrics.Depth);
    int BW = GetBracketWidth(bracket, metrics.Height, metrics.Depth);
    X+=BW;
    LinesDraw(N, X, Y, metrics, NeedBrackets, 0, Simplify);
}
Metrics TFormulaPlotter::IntegralGetTextRectangle(std::shared_ptr<TNumeric> N, bool WithBrackets, bool Simplify) const
{
    Metrics res;
    bool Brackets = false;
    if(TNumeric::GetOperatorPriority(N->operands[0]->operation) < TNumeric::GetOperatorPriority(OperatorIntegral)) Brackets = true;

    auto [W1, H1, D1] = PrettyGetTextRectangle(N->operands[0], Brackets, Simplify);
    auto [W2, H2, D2] = PrettyGetTextRectangle(N->operands[1], false, Simplify);
    auto d = std::make_shared<TNumeric>("d");
    auto [dW, dH, dD] = PrettyGetTextRectangle(d, false, false);
    res.Width = W1 + W2 + dW;
    res.Height = max(max(H1, H2), dH);
    res.Depth = max(max(D1, D2), dD);
    res.Height += res.Height/2;
    res.Depth += res.Depth/2;
    res.Width += GetBracketWidth(UniCode2UTF8String(UnicodeIntegral), res.Height, res.Depth);

    if(WithBrackets)
    {
        res.Width += GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), res.Height, res.Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), res.Height, res.Depth);
    };
    return res;
}

void TFormulaPlotter::IntegralDraw(std::shared_ptr<TNumeric> N, int X, int Y, const Metrics& metrics, bool NeedBrackets, bool Simplify) const
{
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), X, Y, metrics.Height, metrics.Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), metrics.Height, metrics.Depth);
    };

    bool Brackets = false;
    if(TNumeric::GetOperatorPriority(N->operands[0]->operation) < TNumeric::GetOperatorPriority(OperatorIntegral)) Brackets = true;
    Metrics metrics1 = PrettyGetTextRectangle(N->operands[0], Brackets, Simplify);
    Metrics metrics2 = PrettyGetTextRectangle(N->operands[1], false, Simplify);
    auto d = std::make_shared<TNumeric>("d");
    Metrics dmetrics = PrettyGetTextRectangle(d, false, false);


    DrawBracket(UniCode2UTF8String(UnicodeIntegral), X, Y, metrics.Height, metrics.Depth);
    X += GetBracketWidth(UniCode2UTF8String(UnicodeIntegral), metrics.Height, metrics.Depth);

    PrettyDrawAtBaseLeft(N->operands[0], X, Y, Brackets, Simplify);
    X += metrics1.Width;

    PrettyDrawAtBaseLeft(d, X, Y, false, false);
    X += dmetrics.Width;

    PrettyDrawAtBaseLeft(N->operands[1], X, Y, false, Simplify);
    X += metrics2.Width;

    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), X + metrics.Width - GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), metrics.Height, metrics.Depth), Y, metrics.Height, metrics.Depth);
        //X = X + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
}


//функции рисуют формулу после некоторых преобразований, упрощающих её вид
Metrics TFormulaPlotter::GetTextRectangle(std::shared_ptr<TNumeric> N, bool NeedBrackets) const
{
    canvas_->Font = canvas_->FormulaFont;
    return PrettyGetTextRectangle(N, NeedBrackets, true);
}

Metrics TFormulaPlotter::GetTextRectangle(std::shared_ptr<TNumeric> N, int MaxWidth) const
{
    //MaxWidth не используется
    Q_UNUSED(MaxWidth);
    canvas_->Font = canvas_->FormulaFont;
    return GetTextRectangle(N, false);
}

std::string TFormulaPlotter::CodeBasic(std::shared_ptr<const TNumeric> N) {
    return N->CodeBasic();
}

QColor TFormulaPlotter::getFontColor(std::shared_ptr<TNumeric> N) const {
    (void)N;
    QColor color;
    return color.black();
}
std::optional<QColor> TFormulaPlotter::getBorderColor(std::shared_ptr<TNumeric> N) const {
    (void)N;
    return std::nullopt;
}

//===========================================================================

QFont DecreaseFont(std::shared_ptr<TPaintCanvas> canvas)
{
    QFont Font = canvas->Font;
    int PixelSize = canvas->Font.pixelSize();
    if(PixelSize > 0)
        canvas->Font.setPixelSize(PixelSize*2/3);
    else
    {
        int PointSize = canvas->Font.pointSize();
        if(PointSize > 6)
            canvas->Font.setPointSize(PointSize*2/3);
    }
    return Font;
}

