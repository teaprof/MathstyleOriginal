#include <QMouseEvent>
#include <QInputDialog>
#include <QScrollBar>
#include <stdio.h>
#include <string>
#include <QErrorMessage>
#include <sstream>
#include <QVBoxLayout>
#include <QPainter>
#include <iostream>
#include "mathomatic.h"
#include "Base/tline.h"
#include "Base/formulaplotter.h"
using namespace std;
#include "qformulaeditor.h"
QFormulaArea::QFormulaArea(QWidget *parent) :
    QWidget(parent)
{
    Formula = 0;
    setMouseTracking(true);
    if(parent)parent->setEnabled(true);
    setEnabled(true);

    TextFont = QFont("Courier", 10);
    FormulaFont = QFont("Courier", 10);
    BackgroundColor = Qt::red;
    MinWidth = 100;
    MinHeight = 100;
    CurSaved = 0;
}

QFormulaArea::~QFormulaArea()
{
}


void QFormulaArea::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QWidget *Widget = this;
    int CurWidth = Widget->width();
    int CurHeight = Widget->height();
    if(!Formula)
    {
        if(minimumWidth() != CurWidth || minimumHeight() != CurHeight)
        {
            Widget->resize(Widget->minimumSize());
            //размер успешно поменян, мы должны вернуться в главный цикл, чтобы обработать событие об изменении размера
            Widget->update();
            return;
        } else {
            QPainter painter(Widget);
            painter.fillRect(0, 0, Widget->width(), Widget->height(), QBrush(BackgroundColor));
        };
    } else {
        QPainter painter(Widget);
        QRect br;
        int X = 10;
        int Y = 10;
        int LeftPadding = 20;
        int RightPadding = 20;
        auto Canvas = std::make_shared<TPaintCanvas>(&painter);
        Canvas->TextFont = TextFont;
        Canvas->FormulaFont = FormulaFont;
        Canvas->Font = Canvas->FormulaFont;
        Canvas->FormulaColor = FormulaColor;
        Canvas->EditableColor = EditableColor;
        auto [Width, Height, Depth] = TFormulaPlotter(Canvas).PrettyGetTextRectangle(Formula, false, false);
        int GeomWidth = Width + RightPadding;
        if(GeomWidth < minimumWidth()) GeomWidth = minimumWidth();
        int GeomHeight = Height+Depth+br.bottom()+20;
        if(GeomHeight < minimumHeight()) GeomHeight = minimumHeight();

        painter.setFont(QFont("Sans", 14));
        painter.setPen(QPen(Qt::black));
        painter.drawText(QRect(X, Y, GeomWidth - 20, GeomHeight - 20), Qt::AlignJustify | Qt::TextWordWrap, QString::fromUtf8(Task.c_str()), &br);
        GeomHeight = Height+Depth+br.bottom()+20;
        if(GeomHeight < minimumHeight()) GeomHeight = minimumHeight();

        if(GeomWidth != CurWidth || GeomHeight != CurHeight)
        {
            /*QRect R = Widget->geometry();
            R.setSize(QSize(GeomWidth, GeomHeight));
            Widget->setGeometry(R);*/
            Widget->resize(GeomWidth, GeomHeight);
            //размер успешно поменян, мы должны вернуться в главный цикл, чтобы обработать событие об изменении размера
            Widget->update();
            return;
        } else {
            painter.fillRect(0, 0, Widget->width(), Widget->height(), QBrush(BackgroundColor));
            //int W = Widget->width();

            painter.drawText(QRect(X, Y, GeomWidth - 20, GeomHeight - 20), Qt::AlignJustify | Qt::TextWordWrap, QString::fromUtf8(Task.c_str()), &br);

            X = LeftPadding + (GeomWidth - Width - LeftPadding - RightPadding)/2;
            //Y = (br.height() + height()/2 - (Depth-Height)/2; //рисуем формулу по середине
            Y =  br.bottom() + Height + 20; //рисуем формулу сразу после текста задания
            TFormulaPlotter(Canvas).PrettyDrawAtBaseLeft(Formula, X, Y, false, false);
        };
    }
}

void QFormulaArea::mouseMoveEvent(QMouseEvent *M)
{
    if(Formula)
    {
        EditableFormula->mouse_x = M->position().x();
        EditableFormula->mouse_y = M->position().y();
        repaint();
    };
}

void QFormulaArea::mousePressEvent(QMouseEvent *)
{
    if(Formula)
    {
        /*if(Formula->Active)
        {
            Formula->Selected = Formula->Active;            
            bool CanEraseVar = CanErase(Formula, Formula->Selected);
            bool IsConst = (Formula->Active->operation == OperatorConst);
            emit OnSelectionChanged(Formula->Selected);
            emit OnButtonsChanged(Formula->Selected->EditableFlags, CanEraseVar, IsConst);
        } else {
            Formula->Selected = 0;
            emit OnButtonsChanged(false, false, false);
        }*/
        std::cout<<"todo: mouse press event"<<std::endl;
        repaint();
    }
}

void QFormulaArea::mouseDoubleClickEvent(QMouseEvent *)
{
    Edit();
}

bool QFormulaArea::event(QEvent *q)
{
    setEnabled(true);
    return QWidget::event(q);
}

/*void QFormulaArea::resizeEvent(QResizeEvent* E)
{
    QWidget::resizeEvent(E);
QRect R = GroupBox->geometry();
    R.setTop(0);
    R.setHeight(this->height());
    GroupBox->setGeometry(R);
};*/

void QFormulaArea::DebugPrintBuffer()
{
#ifdef __DEBUG__
    cout<<"Buffer: "<<endl;
    for(size_t i = 0; i < Saved.size(); i++)
    {
        if(i+1 == CurSaved)cout<<"*";
        else cout<<" ";
        cout<<i<<": "<<Saved[i].CodeBasic()<<endl;
    }
#endif

}

void QFormulaArea::SetFormula(std::shared_ptr<TNumeric> Formula)
{
    this->Formula = Formula;
    EditableFormula = std::make_shared<TEditableFormula>(Formula);
    Saved.clear();
    CurSaved = 0;
    Save();
    update();    
}

void QFormulaArea::SetUndoRedoEnabled()
{
    if(CurSaved == 0) emit OnEnableUndo(false);
    else emit OnEnableUndo(true);
    if(CurSaved == Saved.size() - 1) emit OnEnableRedo(false);
    else emit OnEnableRedo(true);
}

void QFormulaArea::Save()
{
    if(Formula == 0) return;
    while(CurSaved + 1 < Saved.size() && Saved.size() > 0) Saved.erase(Saved.end() - 1);
    Saved.push_back(*Formula);
    while(Saved.size() > 10) Saved.erase(Saved.begin());
    CurSaved = Saved.size() - 1;
    SetUndoRedoEnabled();
    DebugPrintBuffer();
}

void QFormulaArea::Undo()
{
    if(Formula == 0) return;
    DebugPrintBuffer();
    if(CurSaved == 0)
        //достигли последнего сохраненного элемента
        return;
    CurSaved--; //переходим к предыдущей позиции
    *Formula = Saved[CurSaved];
    repaint();
    SetUndoRedoEnabled();
    DebugPrintBuffer();
}

void QFormulaArea::Redo()
{
    if(Formula == 0) return;
    DebugPrintBuffer();
    if(CurSaved == Saved.size() - 1)
    //нельзя сделать Redo
    {
        return;
    } else {
        CurSaved++;
        *Formula = Saved[CurSaved];
    }
    repaint();
    SetUndoRedoEnabled();
    DebugPrintBuffer();
}


void QFormulaArea::InsertSum()
{
    /*if(Formula && Formula->Selected && Formula->Selected->EditableFlags != NoEditable)
    {                
        TNumeric Temp = *Formula->Selected;
        int EditableFlags = Formula->Selected->EditableFlags;
        int OldID = Temp.ID;
        Temp.ID = -1;
        Formula->Selected->Operands.clear();
        Formula->Selected->OperandsPushback(Temp);
        Formula->Selected->OperandsPushback(TNumeric());
        Formula->Selected->operation = OperatorSum;
        Formula->Selected->SetEditableFlags(EditableFlags);
        Formula->Selected->ID = OldID;        
        Save();
        repaint();
    }*/
}
void QFormulaArea::InsertMinus()
{
    /*if(Formula && Formula->Selected && Formula->Selected->EditableFlags != NoEditable)
    {        
        TNumeric Temp = *Formula->Selected;
        int EditableFlags = Formula->Selected->EditableFlags;
        int OldID = Temp.ID;
        Temp.ID = -1;
        Formula->Selected->Operands.clear();
        Formula->Selected->OperandsPushback(Temp);
        Formula->Selected->OperandsPushback(TNumeric());
        Formula->Selected->operation = OperatorMinus;
        Formula->Selected->SetEditableFlags(EditableFlags);
        Formula->Selected->ID = OldID;
        Save();
        repaint();
    }*/
}
void QFormulaArea::InsertProd()
{
    /*if(Formula && Formula->Selected && Formula->Selected->EditableFlags != NoEditable)
    {        
        TNumeric Temp = *Formula->Selected;
        int EditableFlags = Formula->Selected->EditableFlags;
        int OldID = Temp.ID;
        Temp.ID = -1;
        Formula->Selected->Operands.clear();
        Formula->Selected->OperandsPushback(Temp);
        Formula->Selected->OperandsPushback(TNumeric());
        Formula->Selected->operation = OperatorProd;
        Formula->Selected->SetEditableFlags(EditableFlags);
        Formula->Selected->ID = OldID;
        Save();
        repaint();
    }*/
}
void QFormulaArea::InsertFrac()
{
    /*if(Formula && Formula->Selected && Formula->Selected->EditableFlags != NoEditable)
    {        
        TNumeric Temp = *Formula->Selected;
        int EditableFlags = Formula->Selected->EditableFlags;
        int OldID = Temp.ID;
        Temp.ID = -1;
        Formula->Selected->Operands.clear();
        Formula->Selected->OperandsPushback(Temp);
        Formula->Selected->OperandsPushback(TNumeric());
        Formula->Selected->operation = OperatorFrac;
        Formula->Selected->SetEditableFlags(EditableFlags);
        Formula->Selected->ID = OldID;
        Save();
        repaint();
    }*/
}
void QFormulaArea::InsertPower()
{
    /*if(Formula && Formula->Selected && Formula->Selected->EditableFlags != NoEditable)
    {       
        TNumeric Temp = *Formula->Selected;
        int EditableFlags = Formula->Selected->EditableFlags;
        int OldID = Temp.ID;
        Temp.ID = -1;
        Formula->Selected->Operands.clear();
        Formula->Selected->OperandsPushback(Temp);
        Formula->Selected->OperandsPushback(TNumeric());
        Formula->Selected->operation = OperatorPow;
        Formula->Selected->SetEditableFlags(EditableFlags);
        Formula->Selected->ID = OldID;
        Save();
        repaint();
    }*/
}
void QFormulaArea::InsertSqrt()
{
    /*if(Formula && Formula->Selected && Formula->Selected->EditableFlags != NoEditable)
    {      
        TNumeric Temp = *Formula->Selected;
        int EditableFlags = Formula->Selected->EditableFlags;
        int OldID = Temp.ID;
        Temp.ID = -1;
        Formula->Selected->Operands.clear();
        Formula->Selected->OperandsPushback(Temp);
        Formula->Selected->operation = OperatorSqrt;
        Formula->Selected->SetEditableFlags(EditableFlags);
        Formula->Selected->ID = OldID;
        Save();
        repaint();
    }*/
}

void QFormulaArea::InsertSin()
{
    /*if(Formula && Formula->Selected && Formula->Selected->EditableFlags != NoEditable)
    {      
        TNumeric Temp = *Formula->Selected;
        int EditableFlags = Formula->Selected->EditableFlags;
        int OldID = Temp.ID;
        Temp.ID = -1;
        Formula->Selected->Operands.clear();
        Formula->Selected->OperandsPushback(Temp);
        Formula->Selected->operation = OperatorSin;
        Formula->Selected->SetEditableFlags(EditableFlags);
        Formula->Selected->ID = OldID;
        Save();
        repaint();
    }*/
}

void QFormulaArea::InsertCos()
{
    /*if(Formula && Formula->Selected && Formula->Selected->EditableFlags != NoEditable)
    {
        TNumeric Temp = *Formula->Selected;
        int EditableFlags = Formula->Selected->EditableFlags;
        int OldID = Temp.ID;
        Temp.ID = -1;
        Formula->Selected->Operands.clear();
        Formula->Selected->OperandsPushback(Temp);
        Formula->Selected->operation = OperatorCos;
        Formula->Selected->SetEditableFlags(EditableFlags);
        Formula->Selected->ID = OldID;
        Save();
        repaint();
    }*/
}

void QFormulaArea::InsertTg()
{
    /*if(Formula && Formula->Selected && Formula->Selected->EditableFlags != NoEditable)
    {
        TNumeric Temp = *Formula->Selected;
        int EditableFlags = Formula->Selected->EditableFlags;
        int OldID = Temp.ID;
        Temp.ID = -1;
        Formula->Selected->Operands.clear();
        Formula->Selected->OperandsPushback(Temp);
        Formula->Selected->operation = OperatorTg;
        Formula->Selected->SetEditableFlags(EditableFlags);
        Formula->Selected->ID = OldID;
        Save();
        repaint();
    }*/
}

void QFormulaArea::InsertArcsin()
{
    /*if(Formula && Formula->Selected && Formula->Selected->EditableFlags != NoEditable)
    {
        TNumeric Temp = *Formula->Selected;
        int EditableFlags = Formula->Selected->EditableFlags;
        int OldID = Temp.ID;
        Temp.ID = -1;
        Formula->Selected->Operands.clear();
        Formula->Selected->OperandsPushback(Temp);
        Formula->Selected->operation = OperatorArcsin;
        Formula->Selected->SetEditableFlags(EditableFlags);
        Formula->Selected->ID = OldID;
        Save();
        repaint();
    }*/
}

void QFormulaArea::InsertArccos()
{
    /*if(Formula && Formula->Selected && Formula->Selected->EditableFlags != NoEditable)
    {
        TNumeric Temp = *Formula->Selected;
        int EditableFlags = Formula->Selected->EditableFlags;
        int OldID = Temp.ID;
        Temp.ID = -1;
        Formula->Selected->Operands.clear();
        Formula->Selected->OperandsPushback(Temp);
        Formula->Selected->operation = OperatorArccos;
        Formula->Selected->SetEditableFlags(EditableFlags);
        Formula->Selected->ID = OldID;
        Save();
        repaint();
    }*/
}

void QFormulaArea::InsertArctg()
{
    /*if(Formula && Formula->Selected && Formula->Selected->EditableFlags != NoEditable)
    {
        TNumeric Temp = *Formula->Selected;
        int EditableFlags = Formula->Selected->EditableFlags;
        int OldID = Temp.ID;
        Temp.ID = -1;
        Formula->Selected->Operands.clear();
        Formula->Selected->OperandsPushback(Temp);
        Formula->Selected->operation = OperatorArctg;
        Formula->Selected->SetEditableFlags(EditableFlags);
        Formula->Selected->ID = OldID;
        Save();
        repaint();
    }*/
}

void QFormulaArea::InsertLn()
{
    /*if(Formula && Formula->Selected && Formula->Selected->EditableFlags != NoEditable)
    {
        TNumeric Temp = *Formula->Selected;
        int EditableFlags = Formula->Selected->EditableFlags;
        int OldID = Temp.ID;
        Temp.ID = -1;
        Formula->Selected->Operands.clear();
        Formula->Selected->OperandsPushback(Temp);
        Formula->Selected->operation = OperatorLn;
        Formula->Selected->SetEditableFlags(EditableFlags);
        Formula->Selected->ID = OldID;
        Save();
        repaint();
    }*/
}

void QFormulaArea::InsertLog()
{
    /*if(Formula && Formula->Selected && Formula->Selected->EditableFlags != NoEditable)
    {
        TNumeric Temp = *Formula->Selected;
        int EditableFlags = Formula->Selected->EditableFlags;
        int OldID = Temp.ID;
        Temp.ID = -1;
        Formula->Selected->Operands.clear();
        Formula->Selected->OperandsPushback(Temp);
        Formula->Selected->OperandsPushback(TNumeric());
        Formula->Selected->operation = OperatorLog;
        Formula->Selected->SetEditableFlags(EditableFlags);
        Formula->Selected->ID = OldID;
        Save();
        repaint();
    }*/
}

void QFormulaArea::InsertX()
{
    /*if(Formula && Formula->Selected && Formula->Selected->EditableFlags != NoEditable)
    {
        int EditableFlags = Formula->Selected->EditableFlags;
        Formula->Selected->Operands.clear();
        Formula->Selected->operation = OperatorConst;
        Formula->Selected->K = "x";
        Formula->Selected->SetEditableFlags(EditableFlags);
        Save();
        repaint();
    }*/
}

void QFormulaArea::InsertExp()
{
    /*if(Formula && Formula->Selected && Formula->Selected->EditableFlags != NoEditable)
    {
        TNumeric Temp = *Formula->Selected;
        int EditableFlags = Formula->Selected->EditableFlags;
        int OldID = Temp.ID;
        Temp.ID = -1;
        Formula->Selected->Operands.clear();
        Formula->Selected->OperandsPushback(Temp);
        Formula->Selected->operation = OperatorExp;
        Formula->Selected->SetEditableFlags(EditableFlags);
        Formula->Selected->ID = OldID;
        Save();
        repaint();
    }*/
}

void QFormulaArea::InsertSh()
{
    /*if(Formula && Formula->Selected && Formula->Selected->EditableFlags != NoEditable)
    {
        TNumeric Temp = *Formula->Selected;
        int EditableFlags = Formula->Selected->EditableFlags;
        int OldID = Temp.ID;
        Temp.ID = -1;
        Formula->Selected->Operands.clear();
        Formula->Selected->OperandsPushback(Temp);
        Formula->Selected->operation = OperatorSh;
        Formula->Selected->SetEditableFlags(EditableFlags);
        Formula->Selected->ID = OldID;
        Save();
        repaint();
    }*/
}

void QFormulaArea::InsertCh()
{
    /*if(Formula && Formula->Selected && Formula->Selected->EditableFlags != NoEditable)
    {
        TNumeric Temp = *Formula->Selected;
        int EditableFlags = Formula->Selected->EditableFlags;
        int OldID = Temp.ID;
        Temp.ID = -1;
        Formula->Selected->Operands.clear();
        Formula->Selected->OperandsPushback(Temp);
        Formula->Selected->operation = OperatorCh;
        Formula->Selected->SetEditableFlags(EditableFlags);
        Formula->Selected->ID = OldID;
        Save();
        repaint();
    }*/
}

void QFormulaArea::Clear()
{
    /*if(Formula && Formula->Selected && Formula->Selected->EditableFlags != NoEditable)
    {
        int EditableFlags = Formula->Selected->EditableFlags;
        Formula->Selected->operation = OperatorConst;
        Formula->Selected->Operands.clear();
        Formula->Selected->K ="";
        Formula->Selected->SetEditableFlags(EditableFlags);
        Save();
        repaint();
    }*/
}

void QFormulaArea::Edit()
{
    /*if(Formula && Formula->Selected && Formula->Selected->operation == OperatorConst)
    {
        if(Formula->Selected->EditableFlags == NoEditable)
            return;
        bool ok;
        double newval;
        double oldval;
        sscanf(Formula->Selected->K.c_str(), "%lf", &oldval);
        newval = QInputDialog::getDouble(this, tr("New value"), tr("Enter value"), oldval, -1e5, 1e5, 4, &ok);
        if(ok)
        {
            char Data[100];
            sprintf(Data, "%g", newval);
            Formula->Selected->K = Data;
            Save();
        };
        repaint();
    }*/
}


void QFormulaArea::Erase()
{
    /*if(Formula && Formula->Selected && Formula->Selected->EditableFlags != NoEditable)
    {
        EraseNumeric(Formula, Formula->Selected);
        Save();
        repaint();
    }*/
}


void QFormulaArea::Simplify()
{
    /*if(Formula && Formula->Selected && Formula->Selected->EditableFlags != NoEditable)
    {
            int EditableFlags = Formula->Selected->EditableFlags;
            TNumeric *S = Formula->Selected; //это сделано на случай, если Formula будет совпадать с Selected. Тогда Formula->Selected=N будет вести себя неадекватно
            int OldID = S->ID;
            TNumeric Res = S->Simplify();
            if(Res.IsEqual(*(Formula->Selected)) == false)
                //упростить удалось
            {
                *(Formula->Selected) = Res;
                Formula->Selected = S;
                Formula->Selected->SetEditableFlags(EditableFlags);
                Formula->Selected->ID = OldID;
                Save();
                repaint();
            } else {
                //упростить не удалось
                QErrorMessage EM;
                EM.showMessage(tr("Can not simplify"));
                EM.exec();
            }
    } else {
        QErrorMessage EM;
        EM.showMessage(tr("The equation to simplify is not selected or can not simplify selected equation."));
        EM.exec();
    }*/
}


//***********************************************************************************
//          Q Formula Editor
//***********************************************************************************

QFormulaEditor::QFormulaEditor(QWidget *parent) :
    QWidget(parent)
{
    //Formula = new TNumeric(TNumeric("23.03.1984")+TNumeric("23")/TNumeric("03"));    
    setMouseTracking(true);
    parent->setEnabled(true);
    setEnabled(true);

    ButtonWidth = 32;
    ButtonHeight = 32;
    ButtonPadding = 10;

    FormulaArea = new QFormulaArea(this);
    FormulaArea->setMinimumSize(100, 100);
    FormulaArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    connect(FormulaArea, SIGNAL(OnButtonsChanged(int, int, bool)), this, SLOT(OnButtonsChanged(int, int, bool)));
    connect(FormulaArea, SIGNAL(OnSelectionChanged(TNumeric*)), this, SLOT(OnSelectionChangedSlot(TNumeric*)));
    connect(FormulaArea, SIGNAL(OnEnableRedo(bool)), this, SLOT(OnEnableRedo(bool)));
    connect(FormulaArea, SIGNAL(OnEnableUndo(bool)), this, SLOT(OnEnableUndo(bool)));

    ArithmeticsBox = new QGroupBox(this);
    SetBoxGeometry(ArithmeticsBox, 2*ButtonHeight + 3*ButtonPadding);
    ArithmeticsBox->setMaximumWidth(5*ButtonWidth + 6*ButtonPadding);
    ScrollArea = new QScrollArea;

    //FormulaArea->setGeometry(0, 0, 2000, 2000);
    ScrollArea->setWidget(FormulaArea);
    ScrollArea->setWidgetResizable(false);
    ScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);    


    BPlus = RegisterButton(":formulas/Pictures/Formulas/plus.png",     tr("sum"),    ArithmeticsBox, 0, 0, SLOT(InsertSum()));
    BMinus = RegisterButton(":formulas/Pictures/Formulas/minus.png",     tr("difference"),    ArithmeticsBox, 0, 1, SLOT(InsertMinus()));
    BProd = RegisterButton(":formulas/Pictures/Formulas/prod.png",     tr("product"),    ArithmeticsBox, 1, 0, SLOT(InsertProd()));
    BFrac = RegisterButton(":formulas/Pictures/Formulas/frac.png",     tr("fraction"),    ArithmeticsBox, 1, 1, SLOT(InsertFrac()));
    BPower = RegisterButton(":formulas/Pictures/Formulas/power.png",     tr("Power"),    ArithmeticsBox, 2, 0, SLOT(InsertPower()));
    BSqrt = RegisterButton(":formulas/Pictures/Formulas/sqrt.png",     tr("square root"),    ArithmeticsBox, 2, 1, SLOT(InsertSqrt()));
    BEdit = RegisterButton(":formulas/Pictures/Formulas/edit.png",     tr("edit"),    ArithmeticsBox, 3, 0, SLOT(Edit()));
    BSimplify = RegisterButton(":formulas/Pictures/Formulas/simplify.png",     tr("simplify"),    ArithmeticsBox, 3, 1, SLOT(Simplify()));
    BClear = RegisterButton(":formulas/Pictures/Formulas/clear.png",     tr("clear"),    ArithmeticsBox, 4, 0, SLOT(Clear()));
    BErase = RegisterButton(":formulas/Pictures/Formulas/erase.png",     tr("erase"),    ArithmeticsBox, 4, 1, SLOT(Erase()));

    FunctionBox = new QGroupBox(this);
    SetBoxGeometry(FunctionBox, 2*ButtonHeight + 3*ButtonPadding);
    //FunctionBox->setMaximumWidth(7*ButtonWidth + 8*ButtonPadding);

    BSin = RegisterButton(":formulas/Pictures/Formulas/sin.png",     tr("sine"),    FunctionBox, 0, 0, SLOT(InsertSin()));
    BCos = RegisterButton(":formulas/Pictures/Formulas/cos.png",     tr("cosine"),    FunctionBox, 0, 1, SLOT(InsertCos()));
    BTan = RegisterButton(":formulas/Pictures/Formulas/tg.png",      tr("tangent"),    FunctionBox, 1, 0, SLOT(InsertTg()));
    BArcsin = RegisterButton(":formulas/Pictures/Formulas/asin.png", tr("arcsine"), FunctionBox, 1, 1, SLOT(InsertArcsin()));
    BArccos = RegisterButton(":formulas/Pictures/Formulas/acos.png", tr("arccosine"), FunctionBox, 2, 0, SLOT(InsertArccos()));
    BArctan = RegisterButton(":formulas/Pictures/Formulas/atg.png",  tr("arctangent"),  FunctionBox, 2, 1, SLOT(InsertArctg()));
    BLog = RegisterButton(":formulas/Pictures/Formulas/log.png",     tr("decimal logarithm"),    FunctionBox, 3, 0, SLOT(InsertLog()));
    BLn = RegisterButton(":formulas/Pictures/Formulas/ln.png",       tr("natural logarithm"),     FunctionBox, 3, 1, SLOT(InsertLn()));
    BX = RegisterButton(":formulas/Pictures/Formulas/x.png",       tr("variable x"),     FunctionBox, 4, 0, SLOT(InsertX()));
    BExp = RegisterButton(":formulas/Pictures/Formulas/exp.png",     tr("exponent"),    FunctionBox, 4, 1, SLOT(InsertExp()));
    BCh = RegisterButton(":formulas/Pictures/Formulas/ch.png",     tr("chosinus"),    FunctionBox, 5, 0, SLOT(InsertCh()));
    BSh = RegisterButton(":formulas/Pictures/Formulas/sh.png",     tr("shinus"),    FunctionBox, 5, 1, SLOT(InsertSh()));

    NavigationBox = new QGroupBox(this);
    SetBoxGeometry(NavigationBox, ButtonHeight*2 + 3*ButtonPadding);
    NavigationBox->setMaximumWidth(2*ButtonPadding + ButtonWidth);
    //NavigationBox->setMaximumWidth(1*ButtonWidth + 2*ButtonPadding);
    BUndo = RegisterButton(":formulas/Pictures/Formulas/arrow_left_48.png",       tr("undo"),     NavigationBox, 0, 0, SLOT(Undo()));
    BRedo = RegisterButton(":formulas/Pictures/Formulas/arrow_right_48.png",       tr("redo"),    NavigationBox, 0, 1, SLOT(Redo()));


    stringstream NBStyle;
    NBStyle<<"QGroupBox {";
    NBStyle<<"background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,";
    NBStyle<<"               stop: 0 #FFE1E1, stop: 0.4 #DDDDDD,";
    NBStyle<<"  stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);";
    NBStyle<<"border: 2px solid #C4C4C3;";
    NBStyle<<"border-radius: 2px;";
    NBStyle<<"border-bottom-color: #C2C7CB; /* same as the pane color */";
    NBStyle<<"padding: 0px;";
    NBStyle<<"}";



    NavigationBox->setStyleSheet(QString(NBStyle.str().c_str()));


    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);
    //layout->addWidget(NavigationBox);
    layout->addWidget(ScrollArea);

    QHBoxLayout *layout2 = new QHBoxLayout;
    layout2->addWidget(NavigationBox);
    layout2->addWidget(ArithmeticsBox);
    layout2->addWidget(FunctionBox);
    layout->addLayout(layout2);
    //layout2->setSpacing(0);
    layout->setSpacing(0);
    layout2->setSpacing(0);
    this->layout()->setSpacing(0);

    layout->setContentsMargins(0, 0, 0, 0);


    SetArithmeticsEnabled(true);
    SetFunctionsEnabled(false);

    stringstream StyleSheet;
    StyleSheet<<"QToolButton {";
    StyleSheet<<"border: 1px solid #8f8f91;";
    StyleSheet<<"border-radius: 6px;";
    StyleSheet<<"background-color: rgb(220, 220, 220);}";
    StyleSheet<<"QToolButton:hover {";
    StyleSheet<<"border: 1px solid #8f8f91;";
    StyleSheet<<"border-radius: 6px;";
    StyleSheet<<"background-color: rgb(208, 208, 220);}";



    StyleSheet<<"QGroupBox {";
    StyleSheet<<"background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,";
    StyleSheet<<"               stop: 0 #E1E1E1, stop: 0.4 #DDDDDD,";
    StyleSheet<<"  stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);";
    StyleSheet<<"border: 2px solid #C4C4C3;";
    StyleSheet<<"border-radius: 2px;";
    StyleSheet<<"border-bottom-color: #C2C7CB; /* same as the pane color */";
    //StyleSheet<<"border-top-left-radius: 0px;";
    //StyleSheet<<"border-bottom-left-radius: 0px;";
    StyleSheet<<"padding: 0px;";
    StyleSheet<<"}";

    this->setStyleSheet(QString(StyleSheet.str().c_str()));

}

void QFormulaEditor::SetBoxGeometry(QGroupBox *Box, int Height)
{
    Box->setMinimumHeight(Height);
    Box->setMaximumHeight(Height);
}

QFormulaEditor::~QFormulaEditor()
{
    /*    if(Formula)
        delete Formula;*/
    //    delete PlusIcon;
    /*delete BPlus;
    delete BMinus;
    delete BProd;
    delete BFrac;
    delete BPower;
    delete BSqrt;
    delete BClear;
    delete BSimplify;*/
}

QToolButton* QFormulaEditor::RegisterButton(const char* Img, QString ToolTip, QWidget* Parent, int Col, int Row, const char* Slot)
{
    QIcon *Icon = new QIcon(Img);
    QToolButton *B = new QToolButton(Parent);
    SetPos(B, Col, Row);
    //B->setToolButtonStyle(Qt::ToolButtonTextOnly);
    B->setIcon(*Icon);
    B->setIconSize(QSize(ButtonWidth, ButtonHeight));
    B->setToolTip(ToolTip);
    connect(B, SIGNAL(clicked()), FormulaArea, Slot);
    delete Icon;
    return B;
}


void QFormulaEditor::SetPos(QToolButton *Button, int Col, int Row)
{    
    Button->setGeometry(ButtonPadding + (ButtonWidth+ButtonPadding)*Col, ButtonPadding+(ButtonHeight+ButtonPadding)*Row, ButtonWidth, ButtonHeight);
};


void QFormulaEditor::SetArithmeticsVisible(bool Visible)
{
    BPlus->setVisible(Visible);
    BMinus->setVisible(Visible);
    BProd->setVisible(Visible);
    BFrac->setVisible(Visible);
    BPower->setVisible(Visible);
    BClear->setVisible(Visible);
    BSqrt->setVisible(Visible);
    BSimplify->setVisible(Visible);    
}

void QFormulaEditor::SetArithmeticsEnabled(bool Enabled)
{
    BPlus->setEnabled(Enabled);
    BMinus->setEnabled(Enabled);
    BProd->setEnabled(Enabled);
    BFrac->setEnabled(Enabled);
    BPower->setEnabled(Enabled);
    BClear->setEnabled(Enabled);
    BSqrt->setEnabled(Enabled);
    BSimplify->setEnabled(Enabled);
}

void QFormulaEditor::SetFunctionsVisible(bool Visible)
{
    BSin->setVisible(Visible);
    BCos->setVisible(Visible);
    BTan->setVisible(Visible);
    BArctan->setVisible(Visible);
    BArcsin->setVisible(Visible);
    BArccos->setVisible(Visible);
    BLog->setVisible(Visible);
    BLn->setVisible(Visible);
    BX->setVisible(Visible);
    BExp->setVisible(Visible);
    BCh->setVisible(Visible);
    BSh->setVisible(Visible);
    FunctionBox->setVisible(Visible);
}

void QFormulaEditor::SetFunctionsEnabled(bool Enabled)
{
    BSin->setEnabled(Enabled);
    BCos->setEnabled(Enabled);
    BTan->setEnabled(Enabled);
    BArctan->setEnabled(Enabled);
    BArcsin->setEnabled(Enabled);
    BArccos->setEnabled(Enabled);
    BLog->setEnabled(Enabled);
    BLn->setEnabled(Enabled);
    BX->setEnabled(Enabled);
    BExp->setEnabled(Enabled);
    BCh->setEnabled(Enabled);
    BSh->setEnabled(Enabled);
}

void QFormulaEditor::OnEnableUndo(bool Enabled)
{
    BUndo->setEnabled(Enabled);
}

void QFormulaEditor::OnEnableRedo(bool Enabled)
{
    BRedo->setEnabled(Enabled);
}


void QFormulaEditor::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    /*    QPainter painter(this);
    painter.setBrush(Qt::yellow);
    QRect R = this->rect();
    painter.fillRect(R, QBrush(Qt::blue));*/
}



/*void QFormulaEditor::mouseMoveEvent(QMouseEvent *M)
{
    if(Formula)
    {
        Formula->MouseX = M->x();
        Formula->MouseY = M->y();
        Formula->DrawMouse = true;
        repaint();
    };
}

void QFormulaEditor::mousePressEvent(QMouseEvent *)
{
    if(Formula)
    {
        if(Formula->Active)
        {
            Formula->Selected = Formula->Active;
            SetButtonsEnabled(Formula->Selected->Editable);
            emit OnSelectionChanged(Formula->Selected);
        } else {
            Formula->Selected = 0;
            SetButtonsEnabled(false);
        }
        repaint();
    }
}*/

/*void QFormulaEditor::mouseDoubleClickEvent(QMouseEvent *)
{
    if(Formula && Formula->Selected && Formula->Selected->operation == OperatorConst)
    {
        if(Formula->Selected->Editable == false)
            return;
        bool ok;
        double newval;
        double oldval;
        sscanf(Formula->Selected->K.c_str(), "%lf", &oldval);
        newval = QInputDialog::getDouble(this, QString::fromUtf8("Новое значение"), QString::fromUtf8("Введите число"), oldval, -1e3, 1e3, 4, &ok);
        if(ok)
        {
            char Data[100];
            sprintf(Data, "%g", newval);
            Formula->Selected->K = Data;
        };
        repaint();
    }

}*/

bool QFormulaEditor::event(QEvent *q)
{
    setEnabled(true);
    return QWidget::event(q);
}

void QFormulaEditor::resizeEvent(QResizeEvent* E)
{
    /*QRect R = ArithmeticsBox->geometry();
    R.setTop(0);
    R.setHeight(this->height());
    ArithmeticsBox->setGeometry(R);*/

    /*    FormulaArea->MinWidth = ScrollArea->width();
    FormulaArea->MinHeight = ScrollArea->height();*/
    FormulaArea->setMinimumSize(ScrollArea->size());
    FormulaArea->update();

    QWidget::resizeEvent(E);
};



void QFormulaEditor::SetFonts(const QFont& TextFont, const QFont& FormulaFont)
{
    FormulaArea->TextFont = TextFont;
    FormulaArea->FormulaFont = FormulaFont;
}

void QFormulaEditor::SetColors(const QColor& FormulaColor, const QColor& EditableColor, const QColor& BackgroundColor)
{
    FormulaArea->FormulaColor = FormulaColor;
    FormulaArea->EditableColor = EditableColor;
    FormulaArea->BackgroundColor = BackgroundColor;
    FormulaArea->update();
}

void QFormulaEditor::SetTask(const string& Task)
{
    FormulaArea->Task = Task;
    FormulaArea->update();
}

void QFormulaEditor::SetFormula(std::shared_ptr<TNumeric> Formula)
{
    FormulaArea->SetFormula(Formula);
}

void QFormulaEditor::OnButtonsChanged(int EditableFlags, int CanEraseFlag, bool IsConst)
{
    SetArithmeticsEnabled(EditableFlags & ConstAllowed);
    SetFunctionsEnabled(EditableFlags & FunctionsAllowed);
    BErase->setEnabled(CanEraseFlag);
    BEdit->setEnabled(IsConst & (EditableFlags != NoEditable));
}
void QFormulaEditor::OnSelectionChangedSlot(TNumeric* NewSelection)
{
    emit OnSelectionChanged(NewSelection);
}

void QFormulaEditor::TranslateButtons()
{
    BPlus->setToolTip(tr("sum"));
    BMinus->setToolTip(tr("difference"));
    BProd->setToolTip(tr("product"));
    BFrac->setToolTip(tr("fraction"));
    BPower->setToolTip(tr("power"));
    BSqrt->setToolTip(tr("square root"));
    BClear->setToolTip(tr("clear"));
    BSimplify->setToolTip(tr("simpify"));

    BSin->setToolTip(tr("sine"));
    BCos->setToolTip(tr("cosine"));
    BTan->setToolTip(tr("tangent"));
    BArctan->setToolTip(tr("arctangent"));
    BArcsin->setToolTip(tr("arcsine"));
    BArccos->setToolTip(tr("arccosine"));
    BLog->setToolTip(tr("decimal logarithm"));
    BLn->setToolTip(tr("natural logarithm"));
    BX->setToolTip(tr("X"));
    BExp->setToolTip(tr("exponent"));
    BSh->setToolTip(tr("shinus"));
    BCh->setToolTip(tr("chosinus"));
}
