#include <QGraphicsTextItem>

#include <stdio.h>
#include <iostream>
#include "tline.h"
#include "symbols.h"

#define FormulaFontName "Courier"
#define TextFontName "Courier"

//#define __DEBUG__  //показывает разметку

//---------------------------------------------------------------------------
TPaintCanvas::TPaintCanvas(const TPaintCanvas& C)
{
    Painter = C.Painter;
    Scene = C.Scene;
    Font = C.Font;
    Pen = C.Pen;
    Brush = C.Brush;
    OriginX = 0;
    OriginY = 0;
    TextFont = C.TextFont;
    FormulaFont = C.FormulaFont;
    Font = C.Font;
    FormulaColor = C.FormulaColor;
    TextColor = C.TextColor;
    EditableColor = C.EditableColor;
}

TPaintCanvas::TPaintCanvas(QPainter *Painter)
{
    this->Painter = Painter;
    this->Scene = 0;
    TextFont = QFont(TextFontName, 14);
    FormulaFont = QFont(FormulaFontName, 14);
    FormulaColor = Qt::black;
    TextColor = Qt::blue;
    EditableColor = Qt::blue;

    Font = FormulaFont;
    OriginX = 0;
    OriginY = 0;
}

TPaintCanvas::TPaintCanvas(QGraphicsScene *Scene)
{
    this->Painter = 0;
    this->Scene = Scene;
    TextFont = QFont(TextFontName, 14);
    FormulaFont = QFont(FormulaFontName, 14);
    FormulaColor = Qt::black;
    TextColor = Qt::blue;
    EditableColor = Qt::blue;
    Font = FormulaFont;
    OriginX = 0;
    OriginY = 0;
}

int TPaintCanvas::TextWidth(string Text)
{
    if(Painter)
    {
        QFontMetrics Metrics(Font);
        //QRect R = Metrics.boundingRect(0, 0, 1e6, 1e6, 0, QString::fromUtf8(Text.c_str()));
        //return R.width();
        return Metrics.horizontalAdvance(QString::fromUtf8(Text.c_str()));
    };
    if(Scene)
    {        
        QFontMetrics Metrics(Font);
        //QRect R = Metrics.boundingRect(0, 0, 1e6, 1e6, 0, QString::fromUtf8(Text.c_str()));
        //return R.width();
        return Metrics.horizontalAdvance(QString::fromUtf8(Text.c_str()));

/*        QGraphicsTextItem I(QString::fromUtf8(Text.c_str()));
        I.setFont(Font);
        QRectF R = I.boundingRect();
        return R.width();*/
    }
    throw "TPaintCanvas::TextWidth: Painter or Scene should be set";
}

int TPaintCanvas::TextHeight(string Text)
{
    Q_UNUSED(Text);
    QFontMetrics Metrics(Font);
    return Metrics.ascent();
}

int TPaintCanvas::TextDepth(string Text)
{
    Q_UNUSED(Text);
    QFontMetrics Metrics(Font);
    return Metrics.descent();
}

void TPaintCanvas::TightTextWHD(string Text, int *W, int *H, int *D)
{
    QFontMetrics Metrics(Font);
    QRect R = Metrics.tightBoundingRect(QString::fromUtf8(Text.c_str()));
    if(W)*W = abs(R.right());
    if(H)*H = abs(R.top());
    if(D)*D = abs(R.bottom());
}



int TPaintCanvas::TightTextWidth(string Text)
{
    QFontMetrics Metrics(Font);
    QRect R = Metrics.tightBoundingRect(QString::fromUtf8(Text.c_str()));
    return abs(R.right());
}

int TPaintCanvas::TightTextHeight(string Text)
{
    QFontMetrics Metrics(Font);
    QRect R = Metrics.tightBoundingRect(QString::fromUtf8(Text.c_str()));
    return abs(R.top());
}

int TPaintCanvas::TightTextDepth(string Text)
{
    QFontMetrics Metrics(Font);
    QRect R = Metrics.tightBoundingRect(QString::fromUtf8(Text.c_str()));
    return R.bottom();
}

int TPaintCanvas::SetTextHPlusD(string Text, int H, int D)
{
int Min = 1;
int Max = (H + D)*2;
int Mid;
int NewH = 0, NewD = 0;
    while(Max - Min >= 1)
    {
        Mid = (Max + Min) / 2;
        Font.setPixelSize(Mid);
        //Font.setPointSizeF(Mid);
//        NewH = TextHeight(Text);
//        NewD = TextDepth(Text);
        TightTextWHD(Text, 0, &NewH, &NewD);
        //QRect R = QFontMetrics(Font).tightBoundingRect(QString::fromUtf8(Text.c_str()));
        //size_t L = R.height();
        int L = NewH + NewD;
        if(L > H + D)
        {
            //слишком большой шрифт
            if(Max - Min == 1) Max--;
            else Max = Mid;
        };
        if(L < H + D)
        {
            if(Max - Min == 1) Min++;
            else Min = Mid;
        };
        if(L == H + D)
            break;
    }
    return H - NewH;
}

void TPaintCanvas::Line(int x1, int y1, int x2, int y2, QColor Color, int width)
{
QPen Pen = QPen(QBrush(Color), width);
    Line(x1, y1, x2, y2, Pen);
}

void TPaintCanvas::Rectangle(int x1, int y1, int x2, int y2, QColor Color, int width)
{
QPen Pen(QBrush(Color), width);
QBrush Brush(Color);
    Rectangle(x1, y1, x2, y2, Pen, Brush);
}

void TPaintCanvas::TextOutA(int X, int Y, string Text, QColor Color)
{
    TextOutA(X, Y, Text, Color, Font);
}

void TPaintCanvas::Line(int x1, int y1, int x2, int y2, QColor Color)
{
    QPen Pen = QPen(QBrush(Color), 1);
    Line(x1, y1, x2, y2, Pen);
}

void TPaintCanvas::Rectangle(int x1, int y1, int x2, int y2, QColor Color)
{
    QPen Pen = QPen(QBrush(Color), 1);
    QBrush Brush = QBrush(Qt::NoBrush);
    Rectangle(x1, y1, x2, y2, Pen, Brush);
}


void TPaintCanvas::Line(int x1, int y1, int x2, int y2, QPen Pen)
{
    if(Painter)
    {
        Painter->setPen(Pen);
        Painter->drawLine(x1 - OriginX, y1 - OriginY, x2 - OriginX, y2 - OriginY);
    };
    if(Scene)
    {
        Scene->addLine(x1 - OriginX, y1 - OriginY, x2 - OriginX, y2 - OriginY, Pen);
    }

}

void TPaintCanvas::Rectangle(int x1, int y1, int x2, int y2, QPen Pen, QBrush Brush)
{
    if(Painter)
    {
        QBrush CurBrush = this->Brush;
        QPen CurPen = this->Pen;
        Painter->setBrush(Brush);
        Painter->setPen(Pen);
        Painter->drawRect(x1 - OriginX, y1 - OriginY, x2-x1, y2-y1);
        this->Brush = CurBrush;
        this->Pen = CurPen;
    };
    if(Scene)
    {
        Scene->addRect(x1 - OriginX, y1 - OriginY, x2-x1, y2-y1, Pen, Brush);
    }
}

void TPaintCanvas::TextOutA(int X, int Y, string Text, QColor Color, QFont Font)
{
    if(Painter)        
    {
        Painter->setPen(QPen(Color));
        Painter->setFont(Font);
        Painter->drawText(X - OriginX, Y - OriginY, QString::fromUtf8(Text.c_str()));
#ifdef __DEBUG__
        int W = TextWidth(Text);
        int H = TextHeight(Text);
        int D = TextDepth(Text);
        Painter->drawRect(X - OriginX, Y - H - OriginY, W, H+D);
#endif
    };
    if(Scene)        
    {
        //QGraphicsTextItem *I = Scene->addText(QString::fromUtf8(Text.c_str()), Font);
//        I->setDefaultTextColor(Pen.color());
        QGraphicsSimpleTextItem *I = Scene->addSimpleText(QString::fromUtf8(Text.c_str()), Font);

        I->setFont(Font);
        I->setBrush(QBrush(Color));
//        I->setBrush(QBrush(Qt::green));
        I->setX(X - OriginX);
        I->setY(Y - TextHeight(Text.c_str()) - OriginY);
    }
}

void TPaintCanvas::TextOutRect(int X1, int Y1, int X2, int Y2, string Text, QColor Color, QFont Font)
{
    //Text = Text +"1";
    Text = Text; // + "│";
    if(Painter)
    {
        QRect br;
        Painter->setPen(QPen(Color));
        Painter->setFont(Font);
        Painter->drawText(X1 - OriginX, Y1 - OriginY, X2-X1, Y2-Y1, Qt::AlignCenter | Qt::AlignHCenter, QString::fromUtf8(Text.c_str()));
#ifdef __DEBUG__
        Painter->drawRect(X1 - OriginX, Y1 - OriginY, X2-X1, Y2-Y1);
#endif
    };
    if(Scene)
    {
        //QGraphicsTextItem *I = Scene->addText(QString::fromUtf8(Text.c_str()), Font);
        //I->setFlag(QGraphicsItem::ItemIsSelectable);
        //QRectF R = I->boundingRect();
        //I->setDefaultTextColor(Pen.color());
        //I->setX(X1 - OriginX);
        //I->setY((Y1+Y2)/2 - R.height()/2 - OriginY);


        QGraphicsSimpleTextItem *I = Scene->addSimpleText(QString::fromUtf8(Text.c_str()), Font);
        //QRectF R = I->boundingRect();
        I->setX(X1 - OriginX);
        QFontMetrics M(this->Font);        
        int MidLine = (Y1+Y2)/2;
        I->setY(MidLine - M.ascent() + M.strikeOutPos() - OriginY );
        I->setFont(Font);
        I->setBrush(QBrush(Color));
    }
}

void TPaintCanvas::Line(int x1, int y1, int x2, int y2)
{
    Line(x1, y1, x2, y2, Pen);
}

void TPaintCanvas::Rectangle(int x1, int y1, int x2, int y2)
{
    Rectangle(x1, y1, x2, y2, Pen, Brush);
}

void TPaintCanvas::TextOutA(int X, int Y, string Text)
{
    TextOutA(X, Y, Text, Pen.color(), Font);
}

void TPaintCanvas::TextOutRect(int X1, int Y1, int X2, int Y2, string Text)
{
    TextOutRect(X1, Y1, X2, Y2, Text, Pen.color(), Font);
}

//***************************************************************************
//***************************************************************************
//***************************************************************************
//***************************************************************************
//int TRectangleElementCount = 0;
TRectangleElement::TRectangleElement()
{
    PaddingLeft = 0;
    ColorIndex = 0;
//    TRectangleElementCount++;
}

TRectangleElement::~TRectangleElement()
{
//    TRectangleElementCount--;
 //   std::cout<<"TRectangleElement::~TRectangleElement: "<<TRectangleElementCount<<std::endl;
}


int TRectangleElement::GetPageBottom(TPaintCanvas* Canvas, int Y, int CurPageBottom, int MaxWidth)
{
int W, H, D;
    GetTextRectangle(Canvas, W, H, D, MaxWidth);
    if(Y + D > CurPageBottom)
    {
#ifdef __DEBUG__
        Canvas->Line(0, Y, 1000, Y, Qt::green, 4);
#endif
        if(Y - H - 1 < CurPageBottom)
            return Y - H - 1;
        else
            return CurPageBottom;
    }
    else
        return CurPageBottom;
}
void TRectangleElement::Draw(TPaintCanvas* Canvas, int X, int Y, int MaxWidth) const
{
    if(MaxWidth != -1)
    {
        MaxWidth -= PaddingLeft;
        if(MaxWidth < 0) MaxWidth = -1;
    }
    X+=PaddingLeft;
    DrawAtBaseLeft(Canvas, X, Y, MaxWidth);
}

void TRectangleElement::DrawAtTopLeft(TPaintCanvas* Canvas, int X, int Y, int MaxWidth) const
{
    int W, H, D;
    GetTextRectangle(Canvas, W, H, D, MaxWidth);
    Draw(Canvas, X, Y + H, MaxWidth);
}

void TRectangleElement::operator=(const TRectangleElement& R)
{
    PaddingLeft = R.PaddingLeft;
    ColorIndex = R.ColorIndex;
}

QColor TRectangleElement::GetColor() const
{
    switch(ColorIndex % 2)
    {
        case 0: return Qt::blue;
        default:
            return Qt::black;
    }    
}

//***************************************************************************

TLine::TLine()
{
}


TLine::TLine(TRectangleElement* E)
{
    push_back(E);
}

TLine::TLine(TRectangleElement* E1, TRectangleElement* E2)
{
    push_back(E1);
    push_back(E2);
}

TLine::TLine(TRectangleElement* E1, TRectangleElement* E2, TRectangleElement *E3)
{
    push_back(E1);
    push_back(E2);
    push_back(E3);
}
TLine::TLine(TRectangleElement* E1, TRectangleElement* E2, TRectangleElement *E3, TRectangleElement *E4)
{
    push_back(E1);
    push_back(E2);
    push_back(E3);
    push_back(E4);
}

TLine::~TLine()
{
    for(size_t i = 0; i < size(); i++)
        delete at(i);
}

int TLine::GetPageBottom(TPaintCanvas* Canvas, int Y, int CurPageBottom, int MaxWidth )
{
    for(size_t i = 0; i < size(); i++)
        CurPageBottom = at(i)->GetPageBottom(Canvas, Y, CurPageBottom, MaxWidth);
    return CurPageBottom;

}

void TLine::GetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, int MaxWidth) const
{
    Width = 0;
    Height = 0;
    Depth = 0;
    for(size_t i = 0; i < size(); i++)
    {
        int W, H, D;
        int MaxWidth2 = MaxWidth;
        if(MaxWidth2 != -1) MaxWidth2 -= Width;
        at(i)->GetTextRectangle(Canvas, W, H, D, MaxWidth2);
        Width += W;
        if(Height < H)
            Height = H;
        if(Depth < D)
            Depth = D;
    };
    Width += PaddingLeft;
//    Height = Depth = 100;

};

void TLine::DrawAtBaseLeft(TPaintCanvas* Canvas, int X, int Y, int MaxWidth) const
{    
#ifdef __DEBUG__
    int W, H, D;
    GetTextRectangle(Canvas, W, H, D, MaxWidth);
//    Canvas->Rectangle(X, Y-H, X+W, Y+D, QPen(Qt::red), QBrush(Qt::yellow));
#endif
    for(size_t i = 0; i < size(); i++)
    {
        int W, H, D;
        //at(i)->ColorIndex = this->ColorIndex;
        at(i)->GetTextRectangle(Canvas, W, H, D);
        int MaxWidth2 = MaxWidth;
        if(MaxWidth2 != -1) MaxWidth2 -= X;
        at(i)->Draw(Canvas, X, Y, MaxWidth2);
        X += W;
    };
}

void TLine::SetEditableFlags(int Flags)
{
    for(size_t i = 0; i < size(); i++)
        at(i)->SetEditableFlags(Flags);
}

//*************************************************************************************
//*************************************************************************************
TLines::TLines()
{

}
TLines::~TLines()
{
    for(size_t i = 0; i < size(); i++)
        delete operator[](i);
}

void TLines::AddLine(TLine* Line)
{
    push_back(Line);
}

void TLines::AddLine(TRectangleElement* E)
{
TLine* L = new TLine(E);
    push_back(L);
}


#define InterlineDistance Canvas->TextHeight("1");

void TLines::GetXYSize(TPaintCanvas* Canvas, int &XSize, int &YSize, int MaxWidth) const
{
    XSize = 0;
    YSize = 0;
    for(size_t i = 0; i < size(); i++)
    {
        int W, H, D;
        at(i)->GetTextRectangle(Canvas, W, H, D, MaxWidth);
        YSize += H+D;
        YSize += InterlineDistance;
        if(XSize < W)
            XSize = W;
    };
    XSize += PaddingLeft;
}

void TLines::DrawAtBaseLeft(TPaintCanvas* Canvas, int X, int Y, int MaxWidth) const
{
int W, H, D;
    GetTextRectangle(Canvas, W, H, D, MaxWidth);
#ifdef __DEBUG__
    Canvas->Rectangle(X+1, Y-H + 1, X + W - 1, Y - D + 1, Qt::green);
#endif
    Y -= H;
    for(size_t i = 0; i < size(); i++)
    {
        int W1, H1, D1;
        at(i)->GetTextRectangle(Canvas, W1, H1, D1, MaxWidth);
        at(i)->Draw(Canvas, X, Y + H1, MaxWidth);
#ifdef __DEBUG__
        char Str[20];
        sprintf(Str, "%d", i);
        Canvas->TextOutA(X, Y, Str);
#endif
        Y += H1+D1;
        Y += InterlineDistance;
    };
}

int TLines::GetPageBottom(TPaintCanvas* Canvas, int Y, int CurPageBottom, int MaxWidth)
{
int W, H, D;
    GetTextRectangle(Canvas, W, H, D, MaxWidth);
    Y -= H; //теперь Y указывает на верхнюю границу
    for(size_t i = 0; i < size(); i++)
    {
        int W1, H1, D1;
        at(i)->GetTextRectangle(Canvas, W1, H1, D1, MaxWidth);
        CurPageBottom = at(i)->GetPageBottom(Canvas, Y + H1, CurPageBottom, MaxWidth);
        Y += H1 + D1 + InterlineDistance;
        if(Y > CurPageBottom)
            return CurPageBottom;        
    };
    return CurPageBottom;
}

//*************************************************************************************
void TLines::Add(TRectangleElement* R, int NTabs)
{
    R->PaddingLeft = NTabs*10;
    R->ColorIndex = NTabs;
    push_back(R);
}

void TLines::SetEditableFlags(int Flags)
{
    for(size_t i = 0; i < size(); i++)
        at(i)->SetEditableFlags(Flags);
}

//*************************************************************************************
//*************************************************************************************
TText::TText(string Text2)
{
    this->Text = Text2;
}

TText::TText()
{
}
void TText::GetTextRectangle(TPaintCanvas* Canvas, int &Width, int &Height, int &Depth, int MaxWidth) const
{
    Canvas->Font = Canvas->TextFont;
    Canvas->Pen.setColor(Canvas->TextColor);
    string RText = Recognize(Text);
    if(MaxWidth == -1 || Canvas->TextWidth(RText) < MaxWidth)
    {
        Width =Canvas->TextWidth(RText);
        Height = Canvas->TextHeight(RText);
        Depth = Canvas->TextDepth(RText);
    } else {
        int H = 0;
        string EstimatedStr = RText;
        Width = 0;
        while(EstimatedStr.empty() == false)
        {
            size_t split_pos = 0;
            for(size_t i = 0; i < EstimatedStr.length(); i++)
                if(EstimatedStr[i] == ' ' || i == EstimatedStr.length()-1)
                {
                    if(Canvas->TextWidth(EstimatedStr.substr(0, i)) < MaxWidth || split_pos == 0)
                        split_pos = i;
                }
            string substr;
            if(split_pos == 0 || split_pos == EstimatedStr.length() - 1)
            {
                substr = EstimatedStr;
                EstimatedStr.clear();
            } else {
                substr = EstimatedStr.substr(0, split_pos);
                EstimatedStr = EstimatedStr.substr(split_pos+1);
            }
            int H1, D1;
            H1 = Canvas->TextHeight(substr);
            D1 = Canvas->TextHeight(substr);
            if(Canvas->TextWidth(substr) > Width)
                Width = Canvas->TextWidth(substr);
            H += H1+D1;
        };
        Height = H/2;
        Depth = H - Height;
    };
    Width += PaddingLeft;
}

void TText::DrawAtBaseLeft(TPaintCanvas* Canvas, int X, int Y, int MaxWidth) const
{
QString S = QString::fromUtf8(Text.c_str());
    Canvas->Font = Canvas->TextFont;
    Canvas->Pen.setColor(GetColor());
    string RText = Recognize(Text);
    if(MaxWidth == -1 || Canvas->TextWidth(RText) < MaxWidth)
    {
        Canvas->TextOutA(X, Y, RText, GetColor(), Canvas->Font);
    } else {
        int Height, Depth, Width;
        GetTextRectangle(Canvas, Width, Height, Depth, MaxWidth);
        Y -= Height;

        string EstimatedStr = RText;
        while(EstimatedStr.empty() == false)
        {
            size_t split_pos = 0;
            for(size_t i = 0; i < EstimatedStr.length(); i++)
                if(EstimatedStr[i] == ' ' || i == EstimatedStr.length()-1)
                {
                    if(Canvas->TextWidth(EstimatedStr.substr(0, i)) < MaxWidth || split_pos == 0)
                        split_pos = i;
                }
            string substr;
            if(split_pos == 0 || split_pos == EstimatedStr.length() - 1)
            {
                substr = EstimatedStr;
                EstimatedStr.clear();
            } else {
                substr = EstimatedStr.substr(0, split_pos);
                EstimatedStr = EstimatedStr.substr(split_pos+1);
            }
            int H1, D1;
            H1 = Canvas->TextHeight(substr);
            D1 = Canvas->TextHeight(substr);
            Y += H1 + D1;
            Canvas->TextOutA(X, Y - H1, substr, GetColor(), Canvas->Font);
        };
    }
}


//============================================================================
/*TRASH

void MainWindow::ExportToPdf()
//Printing....
{
    if(Problem == 0) return;
    //if(Problem== 0 || Problem->Solution == 0) return;
    if(FreeVersion)
    {
        QErrorMessage EM;
        EM.setWindowTitle(tr("Error"));
        EM.showMessage(tr("Please, install full version to solve this type of problems"));
        EM.exec();
        return;
    }
QString fileName = QFileDialog::getSaveFileName(this,tr("Export as Pdf.."),"", tr("PDF files (*.pdf)"));

    QString fileName = "1.pdf";
    if (fileName.isEmpty())
        return;
 QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setFullPage(false);
    printer.setOrientation(QPrinter::Portrait);
    printer.setPaperSize(QPrinter::A4);
    QPainter painter(&printer);
    TPaintCanvas Canvas(&painter);
    Canvas.FormulaFont = Opt->FormulaFont;
    Canvas.TextFont = Opt->TextFont;

    painter.begin(&printer);
    QRect Margins = printer.pageRect();

    int PageTop = 0;
    int MaxWidth = Margins.width();
    int W, H, D;
    Problem->Solution->GetTextRectangle(&Canvas, W, H, D, MaxWidth);
    int MidLine = H;
    while(1)
    {
        int PageBottom = PageTop + Margins.height()+1;
        //пытаемся определить координаты верха и низа страницы
        PageBottom = Problem->Solution->GetPageBottom(&Canvas, MidLine, PageBottom, MaxWidth);
        if(PageBottom == PageTop)
            //это на случай, когда элемент по высоте превосходит размер страницы
            PageBottom = PageTop + Margins.height();

        //выводим на печать только те объекты, которые попадают в заданный интервал
        QRect PageR;
        PageR = Margins;
        PageR.setBottom(PageBottom + Margins.top());
        painter.setClipRect(PageR);
        Canvas.OriginX = -PageR.left() - 20;
        Canvas.OriginY = PageTop - PageR.top();
        Problem->Solution->DrawAtTopLeft(&Canvas, 0, 0, MaxWidth);


        //переходим к следующей странице
        PageTop = PageBottom + 1;
        if(PageBottom >= H + D) break;
        printer.newPage();
    }

    painter.end();
}
*/

/*void MainWindow::DrawSolution()
{
    if(SelectedProblem<0 || SelectedProblem >= SolutionFileNames.size()) return;
    if(SolutionFileNames[SelectedProblem].empty() == false)
    {
        QString Str = QString::fromUtf8(SolutionFileNames[SelectedProblem].c_str());
        QUrl URL = QUrl::fromLocalFile(Str);
        ui->webView->load(URL);
    };
     if(Problem && Problem->Solution != 0)
     {
//        QGraphicsScene *Scene = new QGraphicsScene(this);
//        GViewScene = new QGraphicsScene();
        GViewScene->clear();
        GViewScene->setBackgroundBrush(QBrush(Opt->BackgroundColor));
        TPaintCanvas Canvas(GViewScene);
        Canvas.FormulaFont = Opt->FormulaFont;
        Canvas.TextFont = Opt->TextFont;
        Canvas.FormulaColor = Opt->FormulaColor;
        Canvas.TextColor = Opt->TextColor;
        Canvas.EditableColor = Opt->EditableColor;
        Problem->Solution->Draw(&Canvas, 0, 0, -1);
        QRectF R = GViewScene->itemsBoundingRect();
        GViewScene->setSceneRect(R);
//        ui->graphicsView->setScene(Scene);

        if(SolutionFileNames[SelectedProblem].empty() == false)
        {
            QString Str = QString::fromUtf8(SolutionFileNames[SelectedProblem].c_str());
            QUrl URL = QUrl::fromLocalFile(Str);
            ui->webView->load(URL);
        };

     } else {
         //QGraphicsScene *Scene = new QGraphicsScene(this);
         //Scene = new QGraphicsScene();
         GViewScene->clear();
         GViewScene->setBackgroundBrush(QBrush(Opt->BackgroundColor));
  //       ui->graphicsView->setScene(Scene);
     }
} */
