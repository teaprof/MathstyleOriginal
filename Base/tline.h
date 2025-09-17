#ifndef TLINE_H
#define TLINE_H
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QGraphicsView>
#include <QWidget>
#include <QAbstractItemModel>

#include <vector>

using namespace std;


class TPaintCanvas
//для реализации c QT
{   
public:
    int OriginX, OriginY; //при рисовании эти величины вычитаются из координат
    QPainter *Painter;
    QGraphicsScene *Scene;

    QFont TextFont; //шрифт для текста
    QFont FormulaFont; //шрифт для формул
    QColor TextColor;
    QColor FormulaColor;
    QColor EditableColor;


    QFont Font; //текущий шрифт
    QPen Pen;
    QBrush Brush;
    TPaintCanvas(const TPaintCanvas& C);
    TPaintCanvas(QPainter *Painter);
    TPaintCanvas(QGraphicsScene *Scene);

    int TextWidth(string Text);  
    int TextHeight(string Text);  //значение высоты (одно и то же для всех букв)
    int TextDepth(string Text);

    void TightTextWHD(string Text, int *W, int *H, int *D); //ширина, высота, глубина
    int TightTextWidth(string Text);
    int TightTextHeight(string Text); //точное значение высоты (разное для разных букв)
    int TightTextDepth(string Text);

    int SetTextHPlusD(string Text, int H, int D); //методом половинного деления устанавливает шрифт, чтобы у него высота плюс глубина была бы равна H + D
                                                //возвращает смещение базовой линии, с которой нужно рисовать Text

    void Line(int x1, int y1, int x2, int y2, QColor Color, int width);
    void Rectangle(int x1, int y1, int x2, int y2, QColor Color, int width);
    void TextOutA(int X, int Y, string Text, QColor Color, int width); //Y is a baseline

    void Line(int x1, int y1, int x2, int y2, QColor Color);
    void Rectangle(int x1, int y1, int x2, int y2, QColor Color);
    void TextOutA(int X, int Y, string Text, QColor Color); //Y is a baseline

    void Line(int x1, int y1, int x2, int y2);
    void Rectangle(int x1, int y1, int x2, int y2);
    void TextOutA(int X, int Y, string Text);
    void TextOutRect(int X1, int Y1, int X2, int Y2, string Text);

    void Line(int x1, int y1, int x2, int y2, QPen Pen);
    void Rectangle(int x1, int y1, int x2, int y2, QPen Pen, QBrush Brush);
    void TextOutA(int X, int Y, string Text, QColor Color, QFont Font);
    void TextOutRect(int X1, int Y1, int X2, int Y2, string Text, QColor Color, QFont Font);
};


class TRectangleElement
{
    public:
        int ColorIndex; //цвет шрифта
        int PaddingLeft; //расстояние до левой границы
        TRectangleElement();
        virtual ~TRectangleElement();
        virtual int GetPageBottom(TPaintCanvas* Canvas, int Y, int CurPageBottom, int MaxWidth = -1);
        virtual void GetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, int MaxWidth = -1) const = 0;
        void Draw(TPaintCanvas* Canvas, int X, int Y, int MaxWidth = -1) const;
        virtual void DrawAtBaseLeft(TPaintCanvas* Canvas, int X, int Y, int MaxWidth = -1) const = 0;//X - левая граница, Y - положение основной линии текста
        virtual void DrawAtTopLeft(TPaintCanvas* Canvas, int X, int Y, int MaxWidth = -1) const; //(X, Y) - верхний левый угол текста
        void operator=(const TRectangleElement& R);

        virtual void SetEditableFlags(int Editable) { Q_UNUSED(Editable); }; //устанавливает поле Editable, если оно есть

        QColor GetColor() const;
};

class THSpace : public TRectangleElement
{
    public:
        int W;
        THSpace(int W) {this->W = W;}
        THSpace() {this->W = 20;}
        virtual void GetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, int MaxWidth = -1) const
        {
            Q_UNUSED(Canvas);
            Q_UNUSED(MaxWidth);
            Width = W; Height = Depth = 0;
        }

        virtual void DrawAtBaseLeft(TPaintCanvas* Canvas, int X, int Y, int MaxWidth = -1) const
        {
            Q_UNUSED(Canvas);
            Q_UNUSED(X);
            Q_UNUSED(Y);
            Q_UNUSED(MaxWidth);
            return;
        }
};


class TText : public TRectangleElement
{
    public:
        string Text;
        TText(string Text2);
        TText();
        virtual void GetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, int MaxWidth = -1) const;
        virtual void DrawAtBaseLeft(TPaintCanvas* Canvas, int X, int Y, int MaxWidth = -1) const;
};

class TLine: public TRectangleElement, public vector<TRectangleElement*>
//сам является TRectangleElement и состоит из TRectangleElement
{
public:

    TLine();
    TLine(TRectangleElement* E);
    TLine(TRectangleElement* E1, TRectangleElement* E2);
    TLine(TRectangleElement* E1, TRectangleElement* E2, TRectangleElement *E3);
    TLine(TRectangleElement* E1, TRectangleElement* E2, TRectangleElement *E3, TRectangleElement *E4);
    ~TLine();

    virtual int GetPageBottom(TPaintCanvas* Canvas, int Y, int CurPageBottom, int MaxWidth = -1);
    virtual void GetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, int MaxWidth = -1) const;
    virtual void DrawAtBaseLeft(TPaintCanvas* Canvas, int X, int Y, int MaxWidth = -1) const;
    virtual void SetEditableFlags(int Flags); //устанавливает поле Editable, если оно есть
};

class TLines : public TRectangleElement, public vector<TRectangleElement*>
{
    public:
        TLines();
        ~TLines();
        void AddLine(TLine* Line);
        void AddLine(TRectangleElement* E);

        virtual int GetPageBottom(TPaintCanvas* Canvas, int Y, int CurPageBottom, int MaxWidth = -1);


        virtual void GetXYSize(TPaintCanvas* Canvas, int &XSize, int &YSize, int MaxWidth = -1) const;
        virtual void DrawAtBaseLeft(TPaintCanvas* Canvas, int X, int Y, int MaxWidth = -1) const;
        virtual void GetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, int MaxWidth = -1) const
        {
            GetXYSize(Canvas, Width, Height, MaxWidth);
            Depth = Height/2;
            Height = Height - Depth;
        }
        void Add(TRectangleElement* R, int NTabs);
        virtual void SetEditableFlags(int Flags); //устанавливает поле Editable, если оно есть
};


#endif // TLINE_H
