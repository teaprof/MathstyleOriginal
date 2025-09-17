#include "taxes.h"

TScaledCanvas::TScaledCanvas(const TPaintCanvas& C) : TPaintCanvas(C)
{

}

TScaledCanvas::TScaledCanvas(const TScaledCanvas& C) : TPaintCanvas(C)
{

}

TScaledCanvas::TScaledCanvas(QPainter *Painter): TPaintCanvas(Painter)
{

}
TScaledCanvas::TScaledCanvas(QGraphicsScene *Scene) : TPaintCanvas(Scene)
{

}

int TScaledCanvas::getgx(double x)
{
    return gx1+(x-x1)*(gx2-gx1)/(x2-x1);
}

int TScaledCanvas::getgy(double y)
{
    return gy1+(y-y1)*(gy2-gy1)/(y2-y1);
}

double TScaledCanvas::getx(int gx)
{
    return x1 + (x2-x1)*(gx-gx1)/(gx2-gx1);
}

double TScaledCanvas::gety(int gy)
{
    return y1 + (y2-y1)*(gy-gy1)/(gy2-gy1);
}

//преобразование расстояний
double TScaledCanvas::getdx(int dgx)
{
    return (x2-x1)*dgx/(gx2-gx1);
}

double TScaledCanvas::getdy(int dgy)
{
    return (y2-y1)*dgy/(gy2-gy1);
}

int TScaledCanvas::getdgx(double dx)
{
    return dx*(gx2-gx1)/(x2-x1);
}

int TScaledCanvas::getdgy(double dy)
{
    return dy*(gy2-gy1)/(y2-y1);
}

void TScaledCanvas::LineD(double x1, double y1, double x2, double y2,  QColor Color, int width)
{
    Line(getgx(x1), getgy(y1), getgx(x2), getgy(y2), Color, width);
}

void TScaledCanvas::RectangleD(double x1, double y1, double x2, double y2, QColor Color, int width)
{
    Rectangle(getgx(x1), getgy(y1), getgx(x2), getgy(y2), Color, width);
}

void TScaledCanvas::TextOutDA(double X, double Y, string Text, QColor Color)
{
    TextOutA(getgx(X), getgy(Y), Text, Color);
}

//**********************************************************************************

TAxesLine::TAxesLine()
{
    x1 = y1 = x2 = y2 = 0;
}

TAxesLine::TAxesLine(double x1, double y1, double x2, double y2)
{
    this->x1 = x1;
    this->x2 = x2;
    this->y1 = y1;
    this->y2 = y2;
}

void TAxesLine::Draw(TScaledCanvas* C)
{
    C->Line(C->getgx(x1), C->getgy(y1), C->getgx(x2), C->getgy(y2), Qt::black, 1);
}

void TAxesLine::GetRect(TScaledCanvas* C, double &x1, double &y1, double &x2, double &y2)
{
    Q_UNUSED(C);
    if(this->x1 < this->x2)
    {
        x1 = this->x1;
        x2 = this->x2;
    } else {
        x1 = this->x2;
        x2 = this->x1;
    };
    if(this->y1 < this->y2)
    {
        y1 = this->y1;
        y2 = this->y2;
    } else {
        y1 = this->y2;
        y2 = this->y1;
    };
}


TAxesLabel::TAxesLabel()
{
    x = y = 0;
    Text = "";
}

TAxesLabel::TAxesLabel(double x, double y, string Text, int dx, int dy)
{
    this->x = x;
    this->y = y;
    this->dx = dx;
    this->dy = dy;
    this->Text = Text;
}

double TAxesLabel::GetXLeft(TScaledCanvas* C)
{
    int W = C->TextWidth(Text);
    int XLeft;
    XLeft = C->getgx(x);
    switch(dx)
    {
        case -1: XLeft = XLeft - W; break;
        case 0: XLeft = XLeft - W/2; break;
        case 1: break;
    };
    return C->getx(XLeft);
}

double TAxesLabel::GetYTop(TScaledCanvas* C)
{
    int H, D;
    H = C->TextHeight(Text);
    D = C->TextDepth(Text);
    int YMid;

    YMid = C->getgy(y);
    switch(dy)
    {
        case -1: YMid = YMid - H - D; break;
        case 0: YMid = YMid - (H+D)/2; break;
        case 1: YMid = YMid + H; break;
    };
    return C->gety(YMid - H);
}

void TAxesLabel::Draw(TScaledCanvas* C)
{
int W, H, D;
    W = C->TextWidth(Text);
    H = C->TextHeight(Text);
    D = C->TextDepth(Text);
int XLeft, YMid;

    XLeft = C->getgx(x);
    switch(dx)
    {
        case -1: XLeft = XLeft - W; break;
        case 0: XLeft = XLeft - W/2; break;
        case 1: break;
    };


    YMid = C->getgy(y);
    switch(dy)
    {
        case -1: YMid = YMid - H - D; break;
        case 0: YMid = YMid - (H+D)/2; break;
        case 1: YMid = YMid + H; break;
    };
    C->TextOutA(XLeft, YMid, Text);
}

void TAxesLabel::GetRect(TScaledCanvas* C, double &x1, double &y1, double &x2, double &y2)
{
    x1 = GetXLeft(C);
    y1 = GetYTop(C);
    x2 = x1 + C->getdx(C->TextWidth(Text));
    y2 = y1 + C->getdy(C->TextHeight(Text) + C->TextDepth(Text));
}

TAxesCurve::TAxesCurve()
{

}
TAxesCurve::~TAxesCurve()
{

}

void TAxesCurve::Draw(TScaledCanvas* C)
{
    if(X.size() == 0) return;
    for(size_t i = 1; i < X.size(); i++)
    {
        C->LineD(X[i-1], Y[i-1], X[i], Y[i], Qt::blue, 1);
    }
}

void TAxesCurve::GetRect(TScaledCanvas* C, double &x1, double &y1, double &x2, double &y2)
{
    Q_UNUSED(C);
    x1 = xmin; y1 = ymin; x2 = xmax; y2 = ymax;
}

void TAxesCurve::SetXY(vector<double> X, vector<double> Y)
{
    if(X.size() != Y.size()) throw "TAxesCurve::SetXY: X.size() != Y.size()";
    this->X = X;
    this->Y = Y;
    xmin = ymin = xmax = ymax = 0;
    if(X.size()>0)
    {
        xmin = X[0]; xmax = xmin;
        ymin = Y[0]; ymax = ymin;
        for(size_t i = 1; i < X.size(); i++)
        {
            double TempX = X[i];
            xmin = TempX<xmin?TempX:xmin;
            xmax = TempX>xmax?TempX:xmax;
            double TempY = Y[i];
            ymin = TempY<ymin?TempY:ymin;
            ymax = TempY>ymax?TempY:ymax;
        }
    }
}


TAxes::TAxes()
{
    Width = 200;
    Height = 200;    
    XLabel = "x";
    YLabel = "y";
    DrawAxes = true;
}

TAxes::~TAxes()
{
    for(size_t i = 0; i < Elements.size(); i++)
        delete Elements[i];
}


void TAxes::GetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, int MaxWidth) const
{
    Q_UNUSED(Canvas);
    Q_UNUSED(MaxWidth);
    Width = this->Width;
    Height = this->Height/2;
    Depth = this->Height - Height;

}

void TAxes::Adjust(TScaledCanvas* C)
{
    x1 = y1 = x2 = y2 = 0;
    for(size_t i = 0; i < Elements.size(); i++)
    {
        double tx1, ty1, tx2, ty2;
        Elements[i]->GetRect(C, tx1, ty1, tx2, ty2);
        if(tx1 < x1) x1 = tx1;
        if(ty1 < y1) y1 = ty1;
        if(tx2 > x2) x2 = tx2;
        if(ty2 > y2) y2 = ty2;
    };
    if(y1 == y2)
    {
        y1 -= 1;
        y2 += 1;
    };
    if(x1 == x2)
    {
        x1 -= 1;
        x2 += 1;
    }
    C->x1 = x1; C->y1 = y1; C->x2 = x2; C->y2 = y2;
}

void TAxes::DrawAtBaseLeft(TPaintCanvas* Canvas, int X, int Y, int MaxWidth)  const //X - левая граница, Y - положение основной линии текста
{
int Width, Height, Depth;
TScaledCanvas C(*Canvas);
    GetTextRectangle(Canvas, Width, Height, Depth, MaxWidth);
    Y -= Height; //теперь (X, Y) - верхняя левая точка


    C.gx1 = X;
    C.gy1 = Y;
    C.gx2 = X + Width;
    C.gy2 = Y + Height+Depth;
    ((TAxes*)this) ->Adjust(&C);

    C.RectangleD(C.x1, C.y1, C.x2, C.y2, QColor(255, 255, 240), 1);
    Canvas->Rectangle(C.gx1, C.gy1, C.gx2, C.gy2);

    for(size_t i = 0; i < Elements.size(); i++)
        Elements[i]->Draw(&C);
    if(DrawAxes)
    {
        TAxesLine L(x1, 0, x2, 0);
        L.Draw(&C);
        TAxesLabel XL(x2, 0, XLabel, -1, 1);
        XL.Draw(&C);

    }
}

void TAxes::AddLine(double x1, double y1, double x2, double y2)
{
TAxesLine *L = new TAxesLine(x1, y1, x2, y2);
    Elements.push_back(L);
}

void TAxes::AddLabel(double x, double y, string Text, double dx, double dy)
{    
TAxesLabel* L = new TAxesLabel(x, y, Text, dx, dy);
    Elements.push_back(L);
}

void TAxes::AddCurve(vector<double> X, vector<double> Y)
{
TAxesCurve *C = new TAxesCurve;
    C->SetXY(X, Y);
    Elements.push_back(C);
}
