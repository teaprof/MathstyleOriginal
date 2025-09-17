#include <QGraphicsScene>
#include "ttestfonts.h"
#include "ui_ttestfonts.h"
#include <Base/arithmetic.h>


TTestFonts::TTestFonts(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TTestFonts)
{
    ui->setupUi(this);
    DrawScene();
    DrawScene_2();
}

TTestFonts::~TTestFonts()
{
    delete ui;
}

void TTestFonts::DrawOnCanvas(TPaintCanvas& Canvas)
{

    string str = "{}=+-1234567890";
        int Y = 40;
        int X = 0;
        int W, H, D;
        for(size_t i = 0; i < str.size(); i++)
        {
            string substr = str.substr(i, 1);
            Canvas.TightTextWHD(substr, &W, &H, &D);
            Canvas.TextOutA(X, Y, substr);
            Canvas.Rectangle(X, Y-H, X+W+1, Y+D, Qt::blue);

            D = Canvas.TextDepth(substr);
            H = Canvas.TextHeight(substr);
            W = Canvas.TextWidth(substr);
            Canvas.Rectangle(X, Y-H, X+W+1, Y+D, Qt::gray);

            X += W+10;
        };

        X = 0;
        Y += D+100;

        for(size_t i = 0; i < str.size(); i++)
        {
            string substr = str.substr(i, 1);
            H = 40; D = 40;
            Canvas.SetTextHPlusD(substr, H, D);

            Canvas.TightTextWHD(substr, &W, &H, &D);
            Canvas.TextOutA(X, Y-D, substr);
            Canvas.Rectangle(X, Y-H-D, X+W+1, Y, Qt::blue);

            X += W+10;
        };
        Canvas.Line(0, Y, X, Y, Qt::yellow);

}

void TTestFonts::NumericOnCanvas(TPaintCanvas& Canvas)
{
    TNumeric Var("x");
    TNumeric A(2); A.SetEditableFlags(ConstAllowed);
    TNumeric B(3); B.SetEditableFlags(ConstAllowed);
    TNumeric Line1 = MakeEquality(A*Var + B*(Var^2), TNumeric(0));
    TNumeric Line2 = MakeEquality((A/B)*Var + B*MakeLog(Var, (Var^2)+2), TNumeric(0));
    TNumeric Line3 = MakeEquality(A*MakeSin(Var) + B*(Var^2), TNumeric(0));
    TNumeric Line4 = MakeEquality(A*Var + MakeCos(Var^2), TNumeric(0));
    TNumeric SystemOfEquations;
    SystemOfEquations.Operator = OperatorEqSystem;
    SystemOfEquations.OperandsPushback(Line1);
    SystemOfEquations.OperandsPushback(Line2);
    SystemOfEquations.OperandsPushback(Line3);
    SystemOfEquations.OperandsPushback(Line4);
    Canvas.Font = FormulaFont;
    SystemOfEquations.PrettyDrawAtBaseLeft(&Canvas, 0, 0, false, false);
}


void TTestFonts::DrawScene()
{
    if(ui->graphicsView->scene())
        ui->graphicsView->scene()->clear();
    QGraphicsScene *Scene = new QGraphicsScene(ui->graphicsView);
    TPaintCanvas Canvas(Scene);

    Canvas.TextFont = TextFont;
    Canvas.FormulaFont = FormulaFont;
    Canvas.FormulaColor = FormulaColor;
    Canvas.EditableColor = EditableColor;

    Canvas.Font = Canvas.FormulaFont;

    //DrawOnCanvas(Canvas);
    NumericOnCanvas(Canvas);

    ui->graphicsView->setScene(Scene);
}
void TTestFonts::DrawScene_2()
{
    if(ui->graphicsView_2->scene())
        ui->graphicsView_2->scene()->clear();
    QGraphicsScene *Scene = new QGraphicsScene(ui->graphicsView_2);
    TPaintCanvas Canvas(Scene);

    Canvas.TextFont = TextFont;
    Canvas.FormulaFont = FormulaFont;
    Canvas.FormulaColor = FormulaColor;
    Canvas.EditableColor = EditableColor;

    Canvas.Font = Canvas.FormulaFont;

    //P.setBrush(Qt::white);
    //P.fillRect(0, 0, ui->widget->width(), ui->widget->height(), Qt::white);
    DrawOnCanvas(Canvas);

    ui->graphicsView_2->setScene(Scene);

}
void TTestFonts::OnZoomIn()
{
    ui->graphicsView->scale(1.5, 1.5);
}

void TTestFonts::OnZoomOut()
{
    ui->graphicsView->scale(0.6666, 0.6666);
}
void TTestFonts::OnChangeFont(const QFont& F)
{
    FormulaFont = F;
    DrawScene();
    DrawScene_2();
}
