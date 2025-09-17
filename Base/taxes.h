#ifndef TAXES_H
#define TAXES_H
#include "string"
#include "tline.h"

class TScaledCanvas : public TPaintCanvas
{
    public:
        double x1, y1, x2, y2;
        int gx1, gy1, gx2, gy2;
        TScaledCanvas(const TPaintCanvas& C);
        TScaledCanvas(const TScaledCanvas& C);
        TScaledCanvas(QPainter *Painter);
        TScaledCanvas(QGraphicsScene *Scene);

        //некоторые функции перехода от координат на графике к координатам на экране и обратно
        int getgx(double x);
        int getgy(double y);
        double getx(int gx);
        double gety(int gy);

        //преобразование расстояний
        double getdx(int dgx);
        double getdy(int dgy);
        int getdgx(double dx);
        int getdgy(double dy);

        void LineD(double x1, double y1, double x2, double y2,  QColor Color, int width);
        void RectangleD(double x1, double y1, double x2, double y2, QColor Color, int width);
        void TextOutDA(double X, double Y, string Text, QColor Color);
};


class TAxesElement
{
    public:
        TAxesElement() { };
        virtual ~TAxesElement() { };
        virtual void Draw(TScaledCanvas* C) = 0;
        virtual void GetRect(TScaledCanvas* C, double &x1, double &y1, double &x2, double &y2) = 0; // ((top left) - (bottom right) всегда (x1<x2, y1 < y2)
};

class TAxesLine : public TAxesElement
{
    public:
        double x1, y1, x2, y2;
        TAxesLine();
        TAxesLine(double x1, double y1, double x2, double y2);
        virtual void Draw(TScaledCanvas* C);
        virtual void GetRect(TScaledCanvas* C, double &x1, double &y1, double &x2, double &y2);
};


class TAxesLabel : public TAxesElement
{
        double GetXLeft(TScaledCanvas* C);
        double GetYTop(TScaledCanvas* C);
    public:
        double x, y;
        int dx, dy; //в какую сторону рисовать, возможно 8 вариантов: (0, -1), (1, -1) (1, 0) (1, 1) (0, 1) (-1, 1) (-1, 0) (-1, -1)
        string Text;
        TAxesLabel();
        TAxesLabel(double x, double y, string Text, int dx = 1, int dy = 1);
        virtual void Draw(TScaledCanvas* C);
        virtual void GetRect(TScaledCanvas* C, double &x1, double &y1, double &x2, double &y2);
};

class TAxesCurve : public TAxesElement
{
        vector<double> X;
        vector<double> Y;
        double xmin, xmax;
        double ymin, ymax;
    public:
        TAxesCurve();
        ~TAxesCurve();
        virtual void Draw(TScaledCanvas* C);
        virtual void GetRect(TScaledCanvas* C, double &x1, double &y1, double &x2, double &y2); // ((top left) - (bottom right) всегда (x1<x2, y1 < y2)
        void SetXY(vector<double> X, vector<double> Y);
};

class TAxes : public TRectangleElement
{

public:
    double x1, y1, x2, y2; //границы зоны видимости
    bool DrawAxes; //рисовать ли оси
    std::string XLabel, YLabel;
    void Adjust(TScaledCanvas* C); //автоматически устанавливает границы видимости

    int Width, Height;
    vector<TAxesElement*> Elements;
    TAxes();
    ~TAxes();

    virtual void GetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, int MaxWidth = -1) const;
    virtual void DrawAtBaseLeft(TPaintCanvas* Canvas, int X, int Y, int MaxWidth = -1) const;  //X - левая граница, Y - положение основной линии текста

    void AddLine(double x1, double y1, double x2, double y2);
    void AddLabel(double x, double y, string Text, double dx = 1, double dy = 1);
    void AddCurve(vector<double> X, vector<double> Y);

};

#endif // TAXES_H
