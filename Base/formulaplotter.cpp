#include "formulaplotter.h"

void TFormulaPlotter::PrettyGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    if(IsSizeActual() && NeedBrackets == this->SizeWithBrackets)
    {
        Width = this->OwnWidth;
        Height = this->OwnHeight;
        Depth = this->OwnDepth;
    };
    string AStr;
    Width = 0;
    Height = 0;
    Depth = 0;
    switch(Operator)
    {
    case OperatorConst:
        if(K.length() != 0)
        {
            if(K.find("_") != string::npos)
            {
                size_t pos = K.find("_");
                TNumeric N = MakeSubscript(TNumeric(K.substr(0, pos)), TNumeric(K.substr(pos+1)));
                N.PrettyGetTextRectangle(Canvas, Width, Height, Depth, false, Simplify);
            } else {
                AStr = Recognize(K);
                //Если требуются скобки, то ставим их. Но если число положительное - то скобки никогда не ставятся
                //if(NeedBrackets)
                //{
                //проверяем отрицательность числа
                double f;
                if(sscanf(AStr.c_str(), "%lf", &f) > 0) //todo: OPTIMIZE HERE
                {
                    //отрицательное число всегда в скобках
                    if(f < 0) AStr = string("(")+AStr+")";
                }
                //}
                Width =Canvas->TextWidth(AStr);
                Height = Canvas->TightTextHeight(AStr);
                Depth = Canvas->TightTextDepth(AStr);
            };
        } else {
            AStr = "0";
            Width = Canvas->TextWidth(AStr);
            Height = Canvas->TightTextHeight(AStr);
            Depth = Canvas->TightTextDepth(AStr);
        };
        break;
    case OperatorInline:  InlineGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify); break;
    case OperatorInterval:
    case OperatorIntervalSegment:
    case OperatorSegmentInterval:
    case OperatorSegment: IntervalGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
    case OperatorLess:
    case OperatorLessOrEqual:
    case OperatorGreater:
    case OperatorGreaterOrEqual:
    case OperatorEqual: EqualGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
    case OperatorSum: SumGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
    case OperatorMinus: MinusGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
    case OperatorBelongsTo: BelongsToGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
    case OperatorProd: ProdGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
    case OperatorFrac: FracGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
    case OperatorSqrt: SqrtGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
    case OperatorSupIndex:
    case OperatorPow: PowGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
    case OperatorSubIndex: SubIndexGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
    case OperatorEqSet: EqSetGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
    case OperatorEqSystem: EqSystemGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
    case OperatorTab: LinesGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, 40, Simplify);
        break;
    case OperatorLines: LinesGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, 0, Simplify);
        break;
    case OperatorUnion: UnionGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
    case OperatorIntersection: IntersectionGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
    case OperatorDeriv: DerivGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
    case OperatorPlusMinus: PlusMinusGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
    case OperatorIntegral: IntegralGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
    case OperatorAbs: AbsGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
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
        FunctionGetTextRectangle(Canvas, Width, Height, Depth, NeedBrackets, Simplify);
        break;
    };
    OwnSizeActual = true;
    this->OwnWidth = Width;
    this->OwnHeight = Height;
    this->OwnDepth = Depth;
    this->SizeWithBrackets = NeedBrackets;
};

void TFormulaPlotter::DrawAtBaseLeft(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int MaxWidth) const
{
    Q_UNUSED(MaxWidth);
#ifdef __DEBUG__
    cout<<CodeBasic()<<endl;
#endif
    //MaxWidth не используется
    Canvas->Font = Canvas->FormulaFont;
    DrawAtBaseLeft(Canvas, X, Y, false);
}

int TFormulaPlotter::PrettyDrawAtBaseLeft(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, bool NeedBrackets, bool Simplify) const
//Y = baseline
{
    int W, H, D;
    PrettyGetTextRectangle(Canvas, W, H, D, NeedBrackets, Simplify);
#ifdef __DEBUG__
    Canvas->Rectangle(X, Y-H, X+W, Y+D, Qt::gray);
#endif

    for(size_t i = 0; i < Operands.size(); i++)
    {
        Operands[i].DrawMouse = DrawMouse;
        Operands[i].MouseX = MouseX;
        Operands[i].MouseY = MouseY;
        Operands[i].Active = 0;
        Operands[i].Selected = Selected;
    };

    Active = 0;

    if(EditableFlags != NoEditable)
        Canvas->Pen.setColor(Canvas->EditableColor);
    else
        Canvas->Pen.setColor(Canvas->FormulaColor);

    switch(Operator)
    {
    case OperatorConst:
        if(K.length() != 0)
        {
            if(K.find("_") != string::npos)
            {
                size_t pos = K.find("_");
                TNumeric N = MakeSubscript(TNumeric(K.substr(0, pos)), TNumeric(K.substr(pos+1)));
                return N.PrettyDrawAtBaseLeft(Canvas, X, Y, false, Simplify);
            } else {
                string AStr = Recognize(K);
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
                Canvas->TextOutA(X, Y, AStr);
            };
        } else {
            Canvas->Rectangle(X+1, Y-H+2, X+W-1, Y+D-2);
        };
        break;
    case OperatorInline:  InlineDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify); break;
    case OperatorInterval:
    case OperatorIntervalSegment:
    case OperatorSegmentInterval:
    case OperatorSegment: IntervalDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify); break;
    case OperatorLess:
    case OperatorLessOrEqual:
    case OperatorGreater:
    case OperatorGreaterOrEqual:
    case OperatorEqual:  EqualDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
    case OperatorSum:  SumDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
    case OperatorMinus:  MinusDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
    case OperatorBelongsTo:  BelongsToDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
    case OperatorProd: ProdDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
    case OperatorFrac: FracDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
    case OperatorSqrt: SqrtDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
    case OperatorSupIndex:
    case OperatorPow: PowDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
    case OperatorSubIndex: SubIndexDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
    case OperatorEqSet: EqSetDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
    case OperatorEqSystem: EqSystemDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
    case OperatorTab: LinesDraw(Canvas, X, Y, W, H, D, NeedBrackets, 40, Simplify);
        break;
    case OperatorLines: LinesDraw(Canvas, X, Y, W, H, D, NeedBrackets, 0, Simplify);
        break;
    case OperatorUnion: UnionDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
    case OperatorIntersection: IntersectionDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
    case OperatorDeriv: DerivDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
    case OperatorPlusMinus: PlusMinusDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
    case OperatorIntegral: IntegralDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
    case OperatorAbs: AbsDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
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
        FunctionDraw(Canvas, X, Y, W, H, D, NeedBrackets, Simplify);
        break;
    };

    if(DrawMouse)
    {
        //определяем, лежит ли элемент this под мышкой
        if(Active==0 && MouseX > X && MouseX < X+W && MouseY < Y + D && MouseY > Y - H)
            Active = (TNumeric*) this;
        //если кто-то из Operands лежит под мышкой, то это важнее - транслируем Active на верх к корню дерева
        for(size_t i = 0; i < Operands.size(); i++)
            if(Operands[i].Active)
                Active = Operands[i].Active;
        //если этот элемент является Active, то рисуем рамку
        if(Active == this)
            Canvas->Rectangle(X - 1, Y + D + 1, X + W + 1, Y - H - 1);
    };

    if(Selected == this)
    {
        Canvas->Rectangle(X - 1, Y + D + 1, X + W + 1, Y - H - 1, Qt::blue);
    };
#ifdef __DEBUG__
    cout<<CodeBasic()<<endl;
#endif
    return 0;
};

void TFormulaPlotter::DrawBracket(char Bracket, TPaintCanvas* Canvas, int X, int Y, int Height, int Depth) const
{
    string Str;
    Str += Bracket;
    QFont Temp = Canvas->Font;
    Canvas->Font.setWeight(QFont::Light);
    int dBaseLine = Canvas->SetTextHPlusD(Str, Height, Depth);
    //int RealH = Canvas->TextHeight(Str);
    //int RealD = Canvas->TextDepth(Str);
    //int W = Canvas->TextWidth(Str);

    Canvas->TextOutA(X, Y-dBaseLine, Str);
    //   Canvas->TextOutA(X, Y-RealD, Str);
    Canvas->Font = Temp;
};
int TFormulaPlotter::GetBracketWidth(char Bracket, TPaintCanvas* Canvas, int Height, int Depth) const
{
    Q_UNUSED(Depth);
    Q_UNUSED(Height);
    string Str;
    Str += Bracket;
    QFont Temp = Canvas->Font;
    Canvas->Font.setWeight(QFont::Light);
    //    int dBaseLine = Canvas->SetTextHPlusD(Str, Height, Depth);
    int    res = Canvas->TextWidth(Str);
    Canvas->Font = Temp;
    return res;
};

void TFormulaPlotter::DrawBracket(string Bracket, TPaintCanvas* Canvas, int X, int Y, int Height, int Depth) const
{
    string Str;
    Str += Bracket;
    QFont Temp = Canvas->Font;
    Canvas->Font.setWeight(QFont::Light);
    int dBaseLine = Canvas->SetTextHPlusD(Bracket, Height, Depth);
    //    Canvas->TextOutRect(X, Y-Height, X+W, Y+Depth+5, Str);
    Canvas->TextOutA(X, Y - dBaseLine, Str);
#ifdef __DEBUG__
    int W, RealH, RealD;
    Canvas->TightTextWHD(Str, &W, &RealH, &RealD);
    //Canvas->Rectangle(X, Y-Height - dBaseLine, X+W, Y+Depth, Qt::gray);
    //Canvas->Rectangle(X, Y-RealH - dBaseLine, X+W, Y+RealD - dBaseLine, Qt::red);
#endif
    Canvas->Font = Temp;
};
int TFormulaPlotter::GetBracketWidth(string Bracket, TPaintCanvas* Canvas, int Height, int Depth) const
{
    string Str;
    Str += Bracket;
    QFont Temp = Canvas->Font;
    Canvas->Font.setWeight(QFont::Light);
    Canvas->SetTextHPlusD(Bracket, Height, Depth);
    int    res = Canvas->TextWidth(Str);
    //int    res = Canvas->TightTextWidth(Str);
    Canvas->Font = Temp;
    return res;
};


void TFormulaPlotter::FracGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    int W, H, D;
    int MidLine = Canvas->TextHeight("1")/2;
    Width = Height = Depth = 0;
    Operands[0].PrettyGetTextRectangle(Canvas, W, H, D, false, Simplify);
    Height = H + D + 2 + MidLine;
    if(Width < W)
        Width = W;
    Operands[1].PrettyGetTextRectangle(Canvas, W, H, D, false, Simplify);
    Depth = H + D + 2 - MidLine;
    if(Depth < 0) Depth = 0;
    if(Width < W)
        Width = W;
    if(WithBrackets)
    {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
};
void TFormulaPlotter::FracDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    int X1, Y1, D1;
    int WidthOfFrac;
    FracGetTextRectangle(Canvas, X1, Y1, D1, false, Simplify);
    WidthOfFrac = X1;
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X + Width - GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth), Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
    };

    int MidLine = Y - Canvas->TextHeight("1")/2;

    Operands[0].PrettyGetTextRectangle(Canvas, X1, Y1, D1, false, Simplify);
    Operands[0].PrettyDrawAtBaseLeft(Canvas, X - X1/2 + WidthOfFrac/2, MidLine - D1 - 2, false, Simplify);

    Operands[1].PrettyGetTextRectangle(Canvas, X1, Y1, D1, false, Simplify);
    Operands[1].PrettyDrawAtBaseLeft(Canvas, X - X1/2 + WidthOfFrac/2, MidLine + Y1 + 2, false, Simplify);

    Canvas->Line(X, MidLine, X+WidthOfFrac, MidLine, Qt::black);
};


//==============================================================================

void TFormulaPlotter::FunctionGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    string Name;
    switch(Operator)
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
    QFont FTemp = Canvas->Font;
    Canvas->Font.setBold(true);
    Width += Canvas->TextWidth(Name) + Canvas->TextWidth(" ");
    Height = Canvas->TextHeight(Name);
    Depth = Canvas->TextDepth(Name);
    Canvas->Font = FTemp;
    int W1, H1, D1;
    int W2, H2, D2;
    if(Operator == OperatorLog)
    {
        QFont Font;
        Font = DecreaseFont(Canvas);
        Operands[1].PrettyGetTextRectangle(Canvas, W2, H2, D2, true, Simplify);

        Canvas->Font = Font;
        Width = Width + W2;
        //int dDepth = H2 + D2 - Height/2;
        int dDepth = H2 + D2;
        if(dDepth > Depth)  Depth = dDepth;
    }
    Operands[0].PrettyGetTextRectangle(Canvas, W1, H1, D1, true, Simplify);
    Width += W1;
    if(Height < H1) Height = H1;
    if(Depth < D1) Depth = D1;
    if(WithBrackets)
    {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
}

void TFormulaPlotter::FunctionDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(Width);
    string Name;
    switch(Operator)
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
    cout<<CodeBasic()<<endl;
#endif

    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
    };
    QFont FTemp = Canvas->Font;
    Canvas->Font.setBold(true);
    Canvas->TextOutA(X, Y, Name);
    X += Canvas->TextWidth(Name) + Canvas->TextWidth(" ")/2;
    Canvas->Font = FTemp;
    int W1, H1, D1;
    int W2, H2, D2;

    if(Operator == OperatorLog)
    {
        QFont Font = DecreaseFont(Canvas);

        Operands[1].PrettyGetTextRectangle(Canvas, W1, H1, D1, true, Simplify);
        //        Operands[1].PrettyDrawAtBaseLeft(Canvas, X, Y - Height/2 + H1, true, Simplify);
        Operands[1].PrettyDrawAtBaseLeft(Canvas, X, Y + H1, true, Simplify);
        Canvas->Font = Font;

        X += W1;

    };
    Operands[0].PrettyGetTextRectangle(Canvas, W2, H2, D2, true, Simplify);
    Operands[0].PrettyDrawAtBaseLeft(Canvas, X, Y, true, Simplify);
    X+= W2;

    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X, Y, Height, Depth);
}

//==============================================================================
void TFormulaPlotter::StrGetTextRectangle(const char* Str, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    int W, H, D;
    for(size_t i = 0; i < Operands.size(); i++)
    {
        bool NeedBrackets = false;
        if(CompareOperatorsPriority(Operator, Operands[i].Operator)>0) NeedBrackets = true;
        Operands[i].PrettyGetTextRectangle(Canvas, W, H, D, NeedBrackets, Simplify);
        Width += W;
        if(H > Height)
            Height = H;
        if(D > Depth)
            Depth = D;
    };
    Width += (Operands.size()-1)*(Canvas->TextWidth(Str) + Canvas->TextWidth(" "));
    if(WithBrackets)
    {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
};
void TFormulaPlotter::StrDraw(const char* Str, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(Width);
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
    };
    for(size_t i = 0; i < Operands.size(); i++)
    {
        int W1, H1, D1;
        bool NeedBrackets = false;
        if(CompareOperatorsPriority(Operator, Operands[i].Operator)>0) NeedBrackets = true;
        Operands[i].PrettyGetTextRectangle(Canvas, W1, H1, D1, NeedBrackets, Simplify);
        Operands[i].PrettyDrawAtBaseLeft(Canvas, X, Y, NeedBrackets, Simplify);
        X = X + W1;
        if(i+1 < Operands.size())
        {
            //          int PlusHeight = Canvas->TextHeight(Str);
            Canvas->TextOutA(X, Y, Str);
            X = X + Canvas->TextWidth(Str);
        };
    };
    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X, Y, Height, Depth);
};
//==============================================================================
//==============================================================================
void TFormulaPlotter::IntervalGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    Q_UNUSED(WithBrackets);
    int W, H, D;
    for(size_t i = 0; i < Operands.size(); i++)
    {
        Operands[i].PrettyGetTextRectangle(Canvas, W, H, D, false, Simplify);
        Width += W;
        if(H > Height)
            Height = H;
        if(D > Depth)
            Depth = D;
    };
    Width += (Operands.size()-1)*Canvas->TextWidth(",");
    string LeftBracket = UniCode2UTF8String(MyLeftParenthesis),RightBracket = UniCode2UTF8String(MyRightParenthesis);
    switch(Operator)
    {
    case OperatorInterval: LeftBracket=UniCode2UTF8String(MyLeftParenthesis); RightBracket=UniCode2UTF8String(MyRightParenthesis); break;
    case OperatorSegmentInterval: LeftBracket='['; RightBracket=UniCode2UTF8String(MyRightParenthesis); break;
    case OperatorIntervalSegment: LeftBracket=UniCode2UTF8String(MyLeftParenthesis); RightBracket=']'; break;
    case OperatorSegment: LeftBracket='['; RightBracket=']'; break;
    };

    Width = Width + GetBracketWidth(LeftBracket, Canvas, Height, Depth) + GetBracketWidth(RightBracket, Canvas, Height, Depth);
};
void TFormulaPlotter::IntervalDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(NeedBrackets);
    Q_UNUSED(Width);
    string LeftBracket = UniCode2UTF8String(MyLeftParenthesis), RightBracket = UniCode2UTF8String(MyRightParenthesis);
    switch(Operator)
    {
    case OperatorInterval: LeftBracket=UniCode2UTF8String(MyLeftParenthesis); RightBracket=UniCode2UTF8String(MyRightParenthesis); break;
    case OperatorSegmentInterval: LeftBracket='['; RightBracket=UniCode2UTF8String(MyRightParenthesis); break;
    case OperatorIntervalSegment: LeftBracket=UniCode2UTF8String(MyLeftParenthesis); RightBracket=']'; break;
    case OperatorSegment: LeftBracket='['; RightBracket=']'; break;
    };
    DrawBracket(LeftBracket, Canvas, X, Y, Height, Depth);
    X = X + GetBracketWidth(LeftBracket, Canvas, Height, Depth);
    for(size_t i = 0; i < Operands.size(); i++)
    {
        int W1, H1, D1;
        Operands[i].PrettyGetTextRectangle(Canvas, W1, H1, D1, false, Simplify);
        Operands[i].PrettyDrawAtBaseLeft(Canvas, X, Y, false, Simplify);
        X = X + W1;
        if(i+1 < Operands.size())
        {
            Canvas->TextOutA(X, Y, ",");
            X = X + Canvas->TextWidth(",");
        };
    };
    DrawBracket(RightBracket, Canvas, X, Y, Height, Depth);
};
//==============================================================================
//==============================================================================
void TFormulaPlotter::InlineGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    int W, H, D;
    for(size_t i = 0; i < Operands.size(); i++)
    {
        Operands[i].PrettyGetTextRectangle(Canvas, W, H, D, false, Simplify);
        Width += W;
        if(H > Height)
            Height = H;
        if(D > Depth)
            Depth = D;
    };
    if(WithBrackets)
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
};
void TFormulaPlotter::InlineDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(Width);
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
    };
    for(size_t i = 0; i < Operands.size(); i++)
    {
        int W1, H1, D1;
        Operands[i].PrettyGetTextRectangle(Canvas, W1, H1, D1, false, Simplify);
        Operands[i].PrettyDrawAtBaseLeft(Canvas, X, Y, false, Simplify);
        X = X + W1;
    };
    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X, Y, Height, Depth);
};
//==============================================================================
//==============================================================================
void TFormulaPlotter::SumGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    TNumeric Temp(*this); //работаем с Temp, так как this = const
    int W, H, D;
    string StrPlus = UniCode2UTF8String(MyPlusSign);
    string StrMinus = UniCode2UTF8String(MyMinusSign);
    vector<size_t> Exclude; //слагаемые, равные нулю, их лучше всего исключить
#ifdef __DEBUG__
    cout<<CodeBasic()<<endl;
#endif
    if(Simplify)
    {
        for(size_t i = 0; i < Temp.Operands.size(); i++)
        {
            if(Temp.Operands[i].Operator == OperatorConst && Temp.Operands[i].K == "0")Exclude.push_back(i);
            if(Temp.Operands[i].Operator == OperatorFrac)
            {
                if(Temp.Operands[i].Operands[0].Operator==OperatorConst && Temp.Operands[i].Operands[0].K == "0")Exclude.push_back(i);
            }
            if(Temp.Operands[i].Operator == OperatorProd)
            {
                for(size_t j = 0; j < Temp.Operands[i].Operands.size(); j++)
                    if(Temp.Operands[i].Operands[j].Operator==OperatorConst && Temp.Operands[i].Operands[j].K == "0")
                    {
                        Exclude.push_back(i);
                        break;
                    };
            }
        }
        if(Exclude.size() == Temp.Operands.size() && Temp.Operands.size() > 0)
        //все слагаемые - нули, оставляем только одно из них
        {
            Exclude.erase(Exclude.begin());
        };
    };
    size_t Drawn = 0; //количество уже отрисованных слагаемых
    for(size_t i = 0; i < Temp.Operands.size(); i++)
    {
        if(find(Exclude.begin(), Exclude.end(), i) != Exclude.end())continue;
        bool NeedBrackets = false;
        if(CompareOperatorsPriority(Operator, Temp.Operands[i].Operator)>0) NeedBrackets = true;
        vector<size_t> TakenMinuses; //Операнды, у которых минусы уже учтены в знаке перед произведением или дробью
        size_t MinusesCount = 0;

        if((Simplify) & (Drawn > 0))
        //Пытаемся вынести минус, эта операция не явлеятся thread-safe
        //у первого (Drawn = 0) слагаемого минус не выносится
        {
            if(Temp.Operands[i].Operator == OperatorProd || Temp.Operands[i].Operator == OperatorFrac)
            {
                for(size_t j = 0; j < Temp.Operands[i].Operands.size(); j++)
                {
                    if(Temp.Operands[i].Operands[j].Operator == OperatorConst)
                    {
                        if(Temp.Operands[i].Operands[j].K.length() > 0 && Temp.Operands[i].Operands[j].K[0]=='-')
                        {
                            TakenMinuses.push_back(j);
                            Temp.Operands[i].Operands[j].K.erase(Temp.Operands[i].Operands[j].K.begin());
                            MinusesCount++;
                        }
                    }
                }
            }
            if(Temp.Operands[i].Operator == OperatorConst)
                if(Temp.Operands[i].K.length() > 0 && Temp.Operands[i].K[0]=='-')
                {
                    MinusesCount++;
                    Temp.Operands[i].K.erase(Temp.Operands[i].K.begin());
                }
        }
        //рисуем знак
        if(MinusesCount % 2 == 0)
        {
            //плюс, перед первым членом не ставится
            if(Drawn>0) Width += Canvas->TextWidth(StrPlus);
        }
        else
        {
            //минус
            Width += Canvas->TextWidth(StrMinus);
        }

        if(CompareOperatorsPriority(Operator, Temp.Operands[i].Operator)>0) NeedBrackets = true;
        if(Simplify)
        //если надо было упростить, то рисуем упрощенные слагаемые
        {
            Temp.Operands[i].PrettyGetTextRectangle(Canvas, W, H, D, NeedBrackets, Simplify);
        } else {
            //ничего не упрощали, тогда рисуем исходные слагаемые. При этом поле Active у этих слагаемых меняется и вытаскивается функцией PrettyDrawAtBaseLeft в корень дерева
            Operands[i].PrettyGetTextRectangle(Canvas, W, H, D, NeedBrackets, Simplify);
        }

        if(Simplify)
        {
            //возвращаем минус обратно
            if(Temp.Operands[i].Operator == OperatorProd || Temp.Operands[i].Operator == OperatorFrac)
            {
                for(size_t j = 0; j < TakenMinuses.size(); j++)
                    Temp.Operands[i].Operands[TakenMinuses[j]].K = string("-")+Temp.Operands[i].Operands[TakenMinuses[j]].K;
            }
            if(Temp.Operands[i].Operator == OperatorConst && MinusesCount % 2 == 1)
            {
                Temp.Operands[i].K = string("-") + Temp.Operands[i].K;
            }
        };
        Width += W;
        if(H > Height)
            Height = H;
        if(D > Depth)
            Depth = D;
        Drawn++;
    };
    if(WithBrackets)
    {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
#ifdef __DEBUG__
    cout<<CodeBasic()<<endl;
#endif
};
void TFormulaPlotter::SumDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(Width);
#ifdef __DEBUG__
    cout<<CodeBasic()<<endl;
#endif
    TNumeric Temp(*this); //работаем с Temp, так как this = const
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
    };
    vector<size_t> Exclude; //слагаемые, равные нулю, их лучше всего исключить
    if(Simplify)
    {
        for(size_t i = 0; i < Temp.Operands.size(); i++)
        {
            if(Temp.Operands[i].Operator == OperatorConst && Temp.Operands[i].K == "0")Exclude.push_back(i);
            if(Temp.Operands[i].Operator == OperatorFrac)
            {
                if(Temp.Operands[i].Operands[0].Operator==OperatorConst && Temp.Operands[i].Operands[0].K == "0")Exclude.push_back(i);
            }
            if(Temp.Operands[i].Operator == OperatorProd)
            {
                for(size_t j = 0; j < Temp.Operands[i].Operands.size(); j++)
                    if(Temp.Operands[i].Operands[j].Operator==OperatorConst && Temp.Operands[i].Operands[j].K == "0")
                    {
                        Exclude.push_back(i);
                        break;
                    };
            }
        }
        if(Exclude.size() == Temp.Operands.size() && Temp.Operands.size() > 0)
        //все слагаемые - нули, оставляем только одно из них
        {
            Exclude.erase(Exclude.begin());
        };
    };

    string StrPlus = UniCode2UTF8String(MyPlusSign);
    string StrMinus = UniCode2UTF8String(MyMinusSign);
    size_t Drawn = 0; //количество уже отрисованных слагаемых

    for(size_t i = 0; i < Temp.Operands.size(); i++)
    {
        if(find(Exclude.begin(), Exclude.end(), i) != Exclude.end())continue;
        int W1, H1, D1;
        bool NeedBrackets = false;
        if(CompareOperatorsPriority(Operator, Temp.Operands[i].Operator)>0) NeedBrackets = true;
        vector<size_t> TakenMinuses; //Операнды, у которых минусы уже учтены в знаке перед произведением или дробью
        size_t MinusesCount = 0 ;

        if(Simplify && Drawn > 0)
        //Пытаемся вынести минус, эта операция не явлеятся thread-safe
        //у первого (Drawn = 0) слагаемого минус не выносится
        {
            if(Temp.Operands[i].Operator == OperatorProd || Temp.Operands[i].Operator == OperatorFrac)
            {
                for(size_t j = 0; j < Temp.Operands[i].Operands.size(); j++)
                {
                    if(Temp.Operands[i].Operands[j].Operator == OperatorConst)
                    {
                        if(Temp.Operands[i].Operands[j].K.length() > 0 && Temp.Operands[i].Operands[j].K[0]=='-')
                        {
                            TakenMinuses.push_back(j);
                            Temp.Operands[i].Operands[j].K.erase(Temp.Operands[i].Operands[j].K.begin());
                            MinusesCount++;
                        }
                    }
                }
            }
            if(Temp.Operands[i].Operator == OperatorConst)
                if(Temp.Operands[i].K.length() > 0 && Temp.Operands[i].K[0]=='-')
                {
                    MinusesCount++;
                    Temp.Operands[i].K.erase(Temp.Operands[i].K.begin());
                }
        }
        //рисуем знак
        if(MinusesCount % 2 == 0)
        {
            //плюс, перед первым членом не ставится
            if(Drawn>0)
            {
                Canvas->TextOutA(X, Y, StrPlus);
                X = X + Canvas->TextWidth(StrPlus);
            }
        }
        else
        {
            //минус, рисуется всегда
            Canvas->TextOutA(X, Y, StrMinus);
            X = X + Canvas->TextWidth(StrMinus);
        }

        if(CompareOperatorsPriority(Operator, Temp.Operands[i].Operator)>0) NeedBrackets = true;
        if(Simplify)
        //если надо было упростить, то рисуем упрощенные слагаемые
        {
            Temp.Operands[i].PrettyGetTextRectangle(Canvas, W1, H1, D1, NeedBrackets, Simplify);
            Temp.Operands[i].PrettyDrawAtBaseLeft(Canvas, X, Y, NeedBrackets, Simplify);
        } else {
            //ничего не упрощали, тогда рисуем исходные слагаемые. При этом поле Active у этих слагаемых меняется и вытаскивается функцией PrettyDrawAtBaseLeft в корень дерева
            Operands[i].PrettyGetTextRectangle(Canvas, W1, H1, D1, NeedBrackets, Simplify);
            Operands[i].PrettyDrawAtBaseLeft(Canvas, X, Y, NeedBrackets, Simplify);
        }

        if(Simplify)
        {
            //возвращаем минус обратно
            if(Temp.Operands[i].Operator == OperatorProd || Temp.Operands[i].Operator == OperatorFrac)
            {
                for(size_t j = 0; j < TakenMinuses.size(); j++)
                    Temp.Operands[i].Operands[TakenMinuses[j]].K = string("-")+Temp.Operands[i].Operands[TakenMinuses[j]].K;
            }
            if(Temp.Operands[i].Operator == OperatorConst && MinusesCount % 2 == 1)
            {
                Temp.Operands[i].K = string("-") + Temp.Operands[i].K;
            }
        };

        X = X + W1;
        Drawn++;
    };
    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X, Y, Height, Depth);
};
//==============================================================================
//==============================================================================
void TFormulaPlotter::MinusGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    TNumeric Temp(*this); //работаем с Temp, так как this = const
#ifdef __DEBUG__
    cout<<CodeBasic()<<endl;
#endif
    int W, H, D;
    string StrPlus = UniCode2UTF8String(MyPlusSign);
    string StrMinus = UniCode2UTF8String(MyMinusSign);
    for(size_t i = 0; i < Temp.Operands.size(); i++)
    {
        bool NeedBrackets = false;
        if(CompareOperatorsPriority(Operator, Temp.Operands[i].Operator)>0) NeedBrackets = true;
        vector<size_t> TakenMinuses; //Операнды, у которых минусы уже учтены в знаке перед произведением или дробью
        size_t MinusesCount = 0;

        if(Simplify)
        //Пытаемся вынести минус, эта операция не явлеятся thread-safe
        {
            if(Temp.Operands[i].Operator == OperatorProd || Temp.Operands[i].Operator == OperatorFrac)
            {
                for(size_t j = 0; j < Temp.Operands[i].Operands.size(); j++)
                {
                    if(Temp.Operands[i].Operands[j].Operator == OperatorConst)
                    {
                        if(Temp.Operands[i].Operands[j].K.length() > 0 && Temp.Operands[i].Operands[j].K[0]=='-')
                        {
                            TakenMinuses.push_back(j);
                            Temp.Operands[i].Operands[j].K.erase(Temp.Operands[i].Operands[j].K.begin());
                            MinusesCount++;
                        }
                    }
                }
            }
            if(Temp.Operands[i].Operator == OperatorConst)
                if(Temp.Operands[i].K.length() > 0 && Temp.Operands[i].K[0]=='-')
                {
                    MinusesCount++;
                    Temp.Operands[i].K.erase(Temp.Operands[i].K.begin());
                }
        }
        //рисуем знак
        if(MinusesCount % 2 == 1)
        {
            //минус, и ещё один минус перед вторым членом и далее
            if(i == 0) Width += Canvas->TextWidth(StrMinus);
            else Width += Canvas->TextWidth(StrPlus);
        } else {
            //плюс, перед первым членом знак не нужен
            //начиная со второго - знак минус
            if(i>0) Width += Canvas->TextWidth(StrMinus);
        }

        if(Simplify)
        //если надо было упростить, то рисуем упрощенные слагаемые
        {
            Temp.Operands[i].PrettyGetTextRectangle(Canvas, W, H, D, NeedBrackets, Simplify);
        } else {
            //ничего не упрощали, тогда рисуем исходные слагаемые. При этом поле Active у этих слагаемых меняется и вытаскивается функцией PrettyDrawAtBaseLeft в корень дерева
            Operands[i].PrettyGetTextRectangle(Canvas, W, H, D, NeedBrackets, Simplify);
        }


        if(Simplify)
        {
            //возвращаем минус обратно
            if(Temp.Operands[i].Operator == OperatorProd || Temp.Operands[i].Operator == OperatorFrac)
            {
                for(size_t j = 0; j < TakenMinuses.size(); j++)
                    Temp.Operands[i].Operands[TakenMinuses[j]].K = string("-")+Temp.Operands[i].Operands[TakenMinuses[j]].K;
            }
            if(Temp.Operands[i].Operator == OperatorConst && (MinusesCount % 2 == 1))
            {
                Temp.Operands[i].K = string("-") + Temp.Operands[i].K;
            }
        };
        Width += W;
        if(H > Height)
            Height = H;
        if(D > Depth)
            Depth = D;
    };
    if(WithBrackets)
    {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
};
void TFormulaPlotter::MinusDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(Width);
    TNumeric Temp(*this); //работаем с Temp, так как this = const
#ifdef __DEBUG__
    cout<<CodeBasic()<<endl;
#endif
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
    };
    string StrPlus = UniCode2UTF8String(MyPlusSign);
    string StrMinus = UniCode2UTF8String(MyMinusSign);
    for(size_t i = 0; i < Temp.Operands.size(); i++)
    {
        int W1, H1, D1;
        bool NeedBrackets = false;
        if(CompareOperatorsPriority(Operator, Temp.Operands[i].Operator)>0) NeedBrackets = true;
        vector<size_t> TakenMinuses; //Операнды, у которых минусы уже учтены в знаке перед произведением или дробью
        size_t MinusesCount = 0 ;

        if(Simplify)
        //Пытаемся вынести минус, эта операция не явлеятся thread-safe
        //todo: скопировать из SumDraw
        {
            if(Temp.Operands[i].Operator == OperatorProd || Temp.Operands[i].Operator == OperatorFrac)
            {
                for(size_t j = 0; j < Temp.Operands[i].Operands.size(); j++)
                {
                    if(Temp.Operands[i].Operands[j].Operator == OperatorConst)
                    {
                        if(Temp.Operands[i].Operands[j].K.length() > 0 && Temp.Operands[i].Operands[j].K[0]=='-')
                        {
                            TakenMinuses.push_back(j);
                            Temp.Operands[i].Operands[j].K.erase(Temp.Operands[i].Operands[j].K.begin());
                            MinusesCount++;
                        }
                    }
                }
            }
            if(Temp.Operands[i].Operator == OperatorConst)
                if(Temp.Operands[i].K.length() > 0 && Temp.Operands[i].K[0]=='-')
                {
                    MinusesCount++;
                    Temp.Operands[i].K.erase(Temp.Operands[i].K.begin());
                }
        }
        //рисуем знак
        if(MinusesCount % 2 == 1)
        {
            //минус, и ещё один минус перед вторым членом и далее
            if(i == 0)
            {
                Canvas->TextOutA(X, Y, StrMinus);
                X = X + Canvas->TextWidth(StrMinus);
            } else  {
                Canvas->TextOutA(X, Y, StrPlus);
                X = X + Canvas->TextWidth(StrPlus);
            }
        }
        else
        {
            //плюс, перед первым членом знак не нужен
            //начиная со второго - знак минус
            if(i>0)
            {
                Canvas->TextOutA(X, Y, StrMinus);
                X = X + Canvas->TextWidth(StrMinus);
            }
        }

        if(Simplify)
        //если надо было упростить, то рисуем упрощенные слагаемые
        {
            Temp.Operands[i].PrettyGetTextRectangle(Canvas, W1, H1, D1, NeedBrackets, Simplify);
            Temp.Operands[i].PrettyDrawAtBaseLeft(Canvas, X, Y, NeedBrackets, Simplify);
        } else {
            //ничего не упрощали, тогда рисуем исходные слагаемые. При этом поле Active у этих слагаемых меняется и вытаскивается функцией PrettyDrawAtBaseLeft в корень дерева
            Operands[i].PrettyGetTextRectangle(Canvas, W1, H1, D1, NeedBrackets, Simplify);
            Operands[i].PrettyDrawAtBaseLeft(Canvas, X, Y, NeedBrackets, Simplify);
        }

        if(Simplify)
        {
            //возвращаем минус обратно
            if(Temp.Operands[i].Operator == OperatorProd || Temp.Operands[i].Operator == OperatorFrac)
            {
                for(size_t j = 0; j < TakenMinuses.size(); j++)
                    Temp.Operands[i].Operands[TakenMinuses[j]].K = string("-")+Temp.Operands[i].Operands[TakenMinuses[j]].K;
            }
            if(Temp.Operands[i].Operator == OperatorConst && (MinusesCount % 2== 1))
            {
                Temp.Operands[i].K = string("-") + Temp.Operands[i].K;
            }
        };
        X = X + W1;
    };
    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X, Y, Height, Depth);
};
//==============================================================================
//==============================================================================
void TFormulaPlotter::UnionGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    char Sign[10];
    UniCode2UTF8(Sign, UnicodeUnion);
    StrGetTextRectangle(Sign, Canvas, Width, Height, Depth, WithBrackets, Simplify);
}

void TFormulaPlotter::UnionDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    char Sign[10];
    UniCode2UTF8(Sign, UnicodeUnion);
    StrDraw(Sign, Canvas, X, Y, Width, Height, Depth, NeedBrackets, Simplify);
}

//==============================================================================
//==============================================================================
void TFormulaPlotter::PlusMinusGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    char Sign[10];
    UniCode2UTF8(Sign, UnicodePlusMinus);
    StrGetTextRectangle(Sign, Canvas, Width, Height, Depth, WithBrackets, Simplify);
}
void TFormulaPlotter::PlusMinusDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    char Sign[10];
    UniCode2UTF8(Sign, UnicodePlusMinus);
    StrDraw(Sign, Canvas, X, Y, Width, Height, Depth, NeedBrackets, Simplify);
}


//==============================================================================
//==============================================================================

void TFormulaPlotter::IntersectionGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    char Sign[10];
    UniCode2UTF8(Sign, UnicodeIntersection);
    StrGetTextRectangle(Sign, Canvas, Width, Height, Depth, WithBrackets, Simplify);
}

void TFormulaPlotter::IntersectionDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    char Sign[10];
    UniCode2UTF8(Sign, UnicodeIntersection);
    StrDraw(Sign, Canvas, X, Y, Width, Height, Depth, NeedBrackets, Simplify);
}

//==============================================================================
//==============================================================================
void TFormulaPlotter::ProdGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    int W, H, D;
    vector<size_t> ExcludeOperands;
    if(Simplify)
    {
        for(size_t j = 0; j < Operands.size(); j++)
            if(Operands[j].Operator == OperatorConst && Operands[j].K == "1") ExcludeOperands.push_back(j);
        if(ExcludeOperands.size() == Operands.size() && ExcludeOperands.size() > 0) ExcludeOperands.erase(ExcludeOperands.begin());
    }
    char Str[10];
    UniCode2UTF8(Str, UnicodeMiddleDot);
    for(size_t i = 0; i < Operands.size(); i++)
    {
        if(Simplify && find(ExcludeOperands.begin(), ExcludeOperands.end(), i) != ExcludeOperands.end()) continue;
        bool NeedBrackets = false;
        if(CompareOperatorsPriority(Operator, Operands[i].Operator)>0) NeedBrackets = true;
        Operands[i].PrettyGetTextRectangle(Canvas, W, H, D, NeedBrackets, Simplify);
        Width += W;
        if(H > Height)
            Height = H;
        if(D > Depth)
            Depth = D;
        if(i+1 < Operands.size() && !(Simplify && find(ExcludeOperands.begin(), ExcludeOperands.end(), i+1) != ExcludeOperands.end()))
            Width += Canvas->TextWidth(Str);
    };
    if(WithBrackets)
    {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
};
void TFormulaPlotter::ProdDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(Width);

    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
    };
    vector<size_t> ExcludeOperands;
    if(Simplify)
    {
        for(size_t j = 0; j < Operands.size(); j++)
            if(Operands[j].Operator == OperatorConst && Operands[j].K == "1") ExcludeOperands.push_back(j);
        if(ExcludeOperands.size() == Operands.size() && ExcludeOperands.size() > 0) ExcludeOperands.erase(ExcludeOperands.begin());
    }
    char Str[10];
    UniCode2UTF8(Str, UnicodeMiddleDot);
    for(size_t i = 0; i < Operands.size(); i++)
    {
        if(Simplify && find(ExcludeOperands.begin(), ExcludeOperands.end(), i) != ExcludeOperands.end())
            continue;
        int W1, H1, D1;
        bool NeedBrackets = false;
        if(CompareOperatorsPriority(Operator, Operands[i].Operator)>0) NeedBrackets = true;
        Operands[i].PrettyGetTextRectangle(Canvas, W1, H1, D1, NeedBrackets, Simplify);
        Operands[i].PrettyDrawAtBaseLeft(Canvas, X, Y, NeedBrackets, Simplify);
        X = X + W1;
        if(i+1 < Operands.size() && !(Simplify && find(ExcludeOperands.begin(), ExcludeOperands.end(), i+1) != ExcludeOperands.end()))
        {
            //Canvas->TextOutA(X, Y, "*");
#ifdef __DEBUG__
            int W, H, D;
            W = Canvas->TextWidth(Str);
            H = Canvas->TextHeight(Str);
            D = Canvas->TextDepth(Str);
            Canvas->Line(X, Y, X+W, Y, QPen(Qt::red));
            Canvas->Rectangle(X, Y-H, X+W, Y+D, Qt::red);
#endif
            Canvas->TextOutA(X, Y, Str);
            X = X + Canvas->TextWidth(Str);
        };
    };
    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X, Y, Height, Depth);
};
//==============================================================================
//==============================================================================
void TFormulaPlotter::BelongsToGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    char Sign[10];
    UniCode2UTF8(Sign, UnicodeElementOf);
    StrGetTextRectangle(Sign, Canvas, Width, Height, Depth, WithBrackets, Simplify);
};
void TFormulaPlotter::BelongsToDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    char Sign[10];
    UniCode2UTF8(Sign, 0x2208);
    StrDraw(Sign, Canvas, X, Y, Width, Height, Depth, NeedBrackets, Simplify);
};
//==============================================================================
void TFormulaPlotter::EqualGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    string str;
    switch(Operator)
    {
    case OperatorLess: str = UniCode2UTF8String(MyLessThanSign); break;
    case OperatorLessOrEqual:  str = UniCode2UTF8String(MyLessOrEqualSign); break;
    case OperatorGreater: str = UniCode2UTF8String(MyGreaterThanSign); break;
    case OperatorGreaterOrEqual: str = UniCode2UTF8String(MyGreaterOrEqualSign); break;
    case OperatorEqual:   str = UniCode2UTF8String(MyEqualSign); break;
    default: str = "?";
    }
    StrGetTextRectangle(str.c_str(), Canvas, Width, Height, Depth, WithBrackets, Simplify);
};
void TFormulaPlotter::EqualDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    string str;
    switch(Operator)
    {
    case OperatorLess: str = UniCode2UTF8String(MyLessThanSign); break;
    case OperatorLessOrEqual:  str = UniCode2UTF8String(MyLessOrEqualSign); break;
    case OperatorGreater: str = UniCode2UTF8String(MyGreaterThanSign); break;
    case OperatorGreaterOrEqual: str = UniCode2UTF8String(MyGreaterOrEqualSign); break;
    case OperatorEqual:   str = UniCode2UTF8String(MyEqualSign); break;
    default: str = "?";
    }
    StrDraw(str.c_str(), Canvas, X, Y, Width, Height, Depth, NeedBrackets, Simplify);
};
//==============================================================================
//==============================================================================
void TFormulaPlotter::PowGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    int W1, H1, D1;
    int W2, H2, D2;
    Width = Height = Depth = 0;
    Operands[0].PrettyGetTextRectangle(Canvas, W1, H1, D1, true, Simplify);
    Height = H1;
    Depth = D1;
    Width = W1;
    QFont Font = DecreaseFont(Canvas);
    Operands[1].PrettyGetTextRectangle(Canvas, W2, H2, D2, true, Simplify);
    Canvas->Font = Font;
    if(Height < H1/2 + H2 + D2)
        Height = H1/2 + H2 + D2;
    if(Depth < D2 - H1/2)
        Depth = D2 - H1/2;
    Width += W2;
    if(WithBrackets)
    {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
};
void TFormulaPlotter::PowDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(Width);
    int W1, H1, D1;
    int W2, H2, D2;
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
    };
    Operands[0].PrettyGetTextRectangle(Canvas, W1, H1, D1, true, Simplify);
    Operands[0].PrettyDrawAtBaseLeft(Canvas, X, Y, true, Simplify);
    X += W1;
    QFont Font = DecreaseFont(Canvas);
    Operands[1].PrettyGetTextRectangle(Canvas, W2, H2, D2, true, Simplify);
    Operands[1].PrettyDrawAtBaseLeft(Canvas, X, Y - H1/2 - D2, true, Simplify);
    Canvas->Font = Font;
    X+=W2;

    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X , Y, Height, Depth);
};
//==============================================================================
//==============================================================================
void TFormulaPlotter::AbsGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    Q_UNUSED(WithBrackets);
    int W1, H1, D1;
    Width = Height = Depth = 0;
    Operands[0].PrettyGetTextRectangle(Canvas, W1, H1, D1, false, Simplify);
    Height = H1;
    Depth = D1;
    Width = W1;
    Width = Width + 2*GetBracketWidth("|", Canvas, Height, Depth);
};
void TFormulaPlotter::AbsDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(NeedBrackets);
    Q_UNUSED(Width);
    int W1, H1, D1;
    DrawBracket("|", Canvas, X, Y, Height, Depth);
    X = X + GetBracketWidth("|", Canvas, Height, Depth);

    Operands[0].PrettyGetTextRectangle(Canvas, W1, H1, D1, false, Simplify);
    Operands[0].PrettyDrawAtBaseLeft(Canvas, X, Y, false, Simplify);
    X += W1;

    DrawBracket("|", Canvas, X , Y, Height, Depth);
};
//==============================================================================
//==============================================================================
void TFormulaPlotter::DerivGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    int W1, H1, D1;
    int W2, H2, D2;
    Width = Height = Depth = 0;
    Operands[0].PrettyGetTextRectangle(Canvas, W1, H1, D1, true, Simplify);
    Height = H1;
    Depth = D1;
    Width = W1;

    H2 = Canvas->TextHeight("'");
    D2 = Canvas->TextDepth("'");
    W2 = Canvas->TextWidth("'");

    Width += W2;
    if(WithBrackets)
    {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
};
void TFormulaPlotter::DerivDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(Width);
    int W1, H1, D1;
    int W2, H2, D2;
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
    };
    Operands[0].PrettyGetTextRectangle(Canvas, W1, H1, D1, true, Simplify);
    Operands[0].PrettyDrawAtBaseLeft(Canvas, X, Y, true, Simplify);
    X+=W1;

    H2 = Canvas->TextHeight("'");
    D2 = Canvas->TextDepth("'");
    W2 = Canvas->TextWidth("'");
    Canvas->TextOutA(X, Y - H1 + H2, "'");
    X += W2;


    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X , Y, Height, Depth);
};
//==============================================================================
//==============================================================================
void TFormulaPlotter::SubIndexGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    int W, H, D;
    Width = Height = Depth = 0;
    Operands[0].PrettyGetTextRectangle(Canvas, W, H, D, true, Simplify);
    Height = H;
    Depth = D;
    Width = W;
    QFont Font = DecreaseFont(Canvas);
    Operands[1].PrettyGetTextRectangle(Canvas, W, H, D, true, Simplify);
    Canvas->Font = Font;
    if(H + D > 2*Depth)
        Depth = (H + D)/2 + 1;

    /*int IndexBaseLine = - Height/2 + H;
    if(Depth > IndexBaseLine)
        IndexBaseLine = Depth - H/2;
    if(IndexBaseLine + D  > Depth)
        Depth = IndexBaseLine + D; */
    Width += W;
    if(WithBrackets)
    {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
};
void TFormulaPlotter::SubIndexDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(Width);
    int W1, H1, D1;
    int W2, H2, D2;
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
    };
    Operands[0].PrettyGetTextRectangle(Canvas, W1, H1, D1, true, Simplify);
    Operands[0].PrettyDrawAtBaseLeft(Canvas, X, Y, true, Simplify);
    X += W1;

    QFont Font = DecreaseFont(Canvas);
    Operands[1].PrettyGetTextRectangle(Canvas, W2, H2, D2, true, Simplify);
    /*    int IndexBaseLine = - H1/2 + H2;
    if(D1 > IndexBaseLine)
        IndexBaseLine = D1 - H2/2;
    Operands[1].PrettyDrawAtBaseLeft(Canvas, X, Y + IndexBaseLine, true, Simplify);*/
    Operands[1].PrettyDrawAtBaseLeft(Canvas, X, Y + (H2 - D2)/2, true, Simplify);
    X+=W2;
    Canvas->Font = Font;

    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X , Y, Height, Depth);
};
//==============================================================================
//==============================================================================
void TFormulaPlotter::SqrtGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    int W, H, D;
    Operands[0].PrettyGetTextRectangle(Canvas, W, H, D, false, Simplify);
    Width = W;
    Height = H;
    Width = Width + 10;
    Height = Height + 10;
    Depth = D;
    if(WithBrackets)
    {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) +GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
};
void TFormulaPlotter::SqrtDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    Q_UNUSED(Width);
    int W, H, D;
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
    };
    Operands[0].PrettyGetTextRectangle(Canvas, W, H, D, false, Simplify);
    Operands[0].PrettyDrawAtBaseLeft(Canvas, X+10, Y, false, Simplify);
    QColor Color = Canvas->Pen.color();
    Canvas->Line(X, Y, X+2, Y, Color, 1);
    Canvas->Line(X+2, Y, X+5, Y + Depth, Color, 2);
    Canvas->Line(X+5, Y + Depth, X+9, Y - Height + 2, Color, 1);
    Canvas->Line(X+9, Y - Height + 2, X + 10 + W, Y - Height + 2, Color, 1);
    Canvas->Line(X + 10 + W, Y - Height + 2, X + 10 +W, Y - Height + 5, Color, 1);

    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X+10+W, Y, Height, Depth);
};
//==============================================================================
void  TFormulaPlotter::LinesGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, int LeftMargin, bool Simplify) const
//одна или несколько строк, объединенных оператором совокупности или пересечения
{
    Width = 0;
    Height = 0;
    Depth = 0;

    int InterLineSpacing = Canvas->TextHeight("A")/3;
    for(size_t i = 0; i < Operands.size(); i++)
    {
        int W, H, D;
        Operands[i].PrettyGetTextRectangle(Canvas, W, H, D, false, Simplify);
        if(W > Width) Width = W;
        Height += D+H;
        if(i+1 < Operands.size())
            Height += InterLineSpacing;
    };
    Depth = Height/2;
    Height = Height - Depth;
    if(WithBrackets)
    {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };

    Width += LeftMargin;
};
void TFormulaPlotter::LinesDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, int LeftMargin, bool Simplify) const
{
    X += LeftMargin;
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
    };
    int InterLineSpacing = Canvas->TextHeight("A")/3;
    for(size_t i = 0; i < Operands.size(); i++)
    {
        int W, H, D;
        Operands[i].PrettyGetTextRectangle(Canvas, W, H, D, false, Simplify);
        Operands[i].PrettyDrawAtBaseLeft(Canvas, X, Y-Height+H, false, Simplify);
        Y += H+D;
        if(i+1 < Operands.size())
            Y += InterLineSpacing;
    };
    if(NeedBrackets)
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X + Width - GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth), Y, Height, Depth);
};

void TFormulaPlotter::EqSystemGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    LinesGetTextRectangle(Canvas, Width, Height, Depth, WithBrackets, 0, Simplify);
    Width += GetBracketWidth(UniCode2UTF8String(MyLeftCurlyBracket), Canvas, Height*1, Depth);
};
void TFormulaPlotter::EqSystemDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    string bracket = UniCode2UTF8String(MyLeftCurlyBracket);
    DrawBracket(bracket, Canvas, X, Y, Height, Depth);
    int BW = GetBracketWidth(bracket, Canvas, Height, Depth);
    X+=BW;
    LinesDraw(Canvas, X, Y, Width, Height, Depth, NeedBrackets, 0, Simplify);
}
void TFormulaPlotter::EqSetGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    LinesGetTextRectangle(Canvas, Width, Height, Depth, WithBrackets, 0, Simplify);
    Width += GetBracketWidth(UniCode2UTF8String(MyLeftCurlyBracket), Canvas, Height, Depth);
};
void TFormulaPlotter::EqSetDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    string bracket = UniCode2UTF8String(MyLeftCurlyBracket);
    DrawBracket(bracket, Canvas, X, Y, Height, Depth);
    int BW = GetBracketWidth(bracket, Canvas, Height, Depth);
    X+=BW;
    LinesDraw(Canvas, X, Y, Width, Height, Depth, NeedBrackets, 0, Simplify);
}
void TFormulaPlotter::IntegralGetTextRectangle(TNumeric& N, TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, bool WithBrackets, bool Simplify) const
{
    int W1, H1, D1;
    int W2, H2, D2;
    int dW, dH, dD;
    bool Brackets = false;
    if(GetOperatorPriority(Operands[0].Operator) < GetOperatorPriority(OperatorIntegral)) Brackets = true;

    Operands[0].PrettyGetTextRectangle(Canvas, W1, H1, D1, Brackets, Simplify);
    Operands[1].PrettyGetTextRectangle(Canvas, W2, H2, D2, false, Simplify);
    TNumeric d("d");
    d.PrettyGetTextRectangle(Canvas, dW, dH, dD, false, false);
    Width = W1 + W2 + dW;
    Height = max(max(H1, H2), dH);
    Depth = max(max(D1, D2), dD);
    Height += Height/2;
    Depth += Depth/2;
    Width += GetBracketWidth(UniCode2UTF8String(UnicodeIntegral), Canvas, Height, Depth);

    if(WithBrackets)
    {
        Width = Width + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth) + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };

}

void TFormulaPlotter::IntegralDraw(TNumeric& N, TPaintCanvas* Canvas, int X, int Y, int &Width, int &Height, int &Depth, bool NeedBrackets, bool Simplify) const
{
    int W1, H1, D1;
    int W2, H2, D2;
    int dW, dH, dD;
    TNumeric d("d");
    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyLeftParenthesis), Canvas, X, Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyLeftParenthesis), Canvas, Height, Depth);
    };

    bool Brackets = false;
    if(GetOperatorPriority(Operands[0].Operator) < GetOperatorPriority(OperatorIntegral)) Brackets = true;
    Operands[0].PrettyGetTextRectangle(Canvas, W1, H1, D1, Brackets, Simplify);
    Operands[1].PrettyGetTextRectangle(Canvas, W2, H2, D2, false, Simplify);
    d.PrettyGetTextRectangle(Canvas, dW, dH, dD, false, false);


    DrawBracket(UniCode2UTF8String(UnicodeIntegral), Canvas, X, Y, Height, Depth);
    X += GetBracketWidth(UniCode2UTF8String(UnicodeIntegral), Canvas, Height, Depth);

    Operands[0].PrettyDrawAtBaseLeft(Canvas, X, Y, Brackets, Simplify);
    X += W1;

    d.PrettyDrawAtBaseLeft(Canvas, X, Y, false, false);
    X += dW;

    Operands[1].PrettyDrawAtBaseLeft(Canvas, X, Y, false, Simplify);
    X += W2;

    if(NeedBrackets)
    {
        DrawBracket(UniCode2UTF8String(MyRightParenthesis), Canvas, X + Width - GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth), Y, Height, Depth);
        X = X + GetBracketWidth(UniCode2UTF8String(MyRightParenthesis), Canvas, Height, Depth);
    };
}

//===========================================================================

QFont DecreaseFont(TPaintCanvas* Canvas)
{
    QFont Font = Canvas->Font;
    int PixelSize = Canvas->Font.pixelSize();
    if(PixelSize > 0)
        Canvas->Font.setPixelSize(PixelSize*2/3);
    else
    {
        int PointSize = Canvas->Font.pointSize();
        if(PointSize > 6)
            Canvas->Font.setPointSize(PointSize*2/3);
    }
    return Font;
}

