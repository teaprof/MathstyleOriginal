#ifndef QFORMULAEDITOR_H
#define QFORMULAEDITOR_H

#include "Base/arithmetic.h"
#include "Base/editableformula.h"

#include <QToolButton>
#include <QPushButton>
#include <QWidget>
#include <QFrame>
#include <QIcon>
#include <QGroupBox>
#include <QScrollArea>
#include <string>
using namespace std;

class QFormulaArea : public QWidget
{
    Q_OBJECT
public:
    vector<TNumeric> Saved; //для команды Undo
    size_t CurSaved; //индекс в массиве Saved, используется командой Undo

    int MinWidth, MinHeight;
    QWidget* FormulaArea;
    QScrollArea* ScrollArea;
    QFont TextFont, FormulaFont;
    QColor FormulaColor, EditableColor, BackgroundColor;
    explicit QFormulaArea(QWidget *parent = 0);
    ~QFormulaArea();

    void DebugPrintBuffer();
    void SetFormula(std::shared_ptr<TNumeric> Formula);

    string Task;
    std::shared_ptr<TNumeric> Formula;
    std::shared_ptr<TEditableFormula> EditableFormula;
signals:
    void OnSelectionChanged(TNumeric *NewSelection);
    void OnButtonsChanged(int EditableFlags, int CanEraseFlag, bool IsConst);
    void OnEnableUndo(bool Enabled);
    void OnEnableRedo(bool Enabled);

public slots:
    void Save(); //для команды Undo
    void Undo();
    void Redo();
    void SetUndoRedoEnabled();

    void InsertSum();
    void InsertMinus();
    void InsertProd();
    void InsertFrac();
    void InsertPower();
    void InsertSqrt();
    void Clear();
    void Simplify();
    void Erase();

    void InsertSin();
    void InsertCos();
    void InsertTg();
    void InsertArcsin();
    void InsertArccos();
    void InsertArctg();
    void InsertLn();
    void InsertLog();
    void InsertX();        
    void InsertExp();
    void InsertSh();
    void InsertCh();
    void Edit();

protected:

       virtual void paintEvent(QPaintEvent *event);
       virtual void mouseMoveEvent(QMouseEvent *event);
       virtual void mousePressEvent(QMouseEvent *event);
       virtual bool event(QEvent *);
       virtual void mouseDoubleClickEvent(QMouseEvent *);
//       virtual void resizeEvent(QResizeEvent* E);
};

class QFormulaEditor : public QWidget
{
        Q_OBJECT
public:    
    QFormulaArea* FormulaArea;
    QScrollArea* ScrollArea;
    explicit QFormulaEditor(QWidget *parent = 0);
    ~QFormulaEditor();
    void SetArithmeticsVisible(bool Visible);
    void SetArithmeticsEnabled(bool Enabled);

    void SetFunctionsVisible(bool Visible);
    void SetFunctionsEnabled(bool Enabled);

    void SetPos(QToolButton *Button, int Col, int Row);

    void SetFonts(const QFont& TextFont, const QFont& FormulaFont);
    void SetColors(const QColor& FormulaColor, const QColor& EditableColor, const QColor& BackgroundColor);
    void SetTask(const string& Task);
    void SetFormula(std::shared_ptr<TNumeric> Formula);

    void TranslateButtons();

    QToolButton* RegisterButton(const char* Img, QString ToolTip, QWidget* Parent, int Col, int Row, const char* Slot);
    void SetBoxGeometry(QGroupBox *Box, int Height);

signals:
    void OnSelectionChanged(TNumeric *NewSelection);

public slots:
    void OnButtonsChanged(int EditableFlags, int CanEraseFlag, bool IsConst);
    void OnSelectionChangedSlot(TNumeric* NewSelection);

    void OnEnableUndo(bool Enabled);
    void OnEnableRedo(bool Enabled);
protected:      
      int ButtonWidth;
      int ButtonHeight;
      int ButtonPadding; //расстояние между кнопками

      QToolButton* BPlus;
      QToolButton* BMinus;
      QToolButton* BProd;
      QToolButton* BFrac;
      QToolButton* BPower;
      QToolButton* BClear;
      QToolButton* BEdit;
      QToolButton* BSqrt;
      QToolButton* BSimplify;
      QGroupBox* ArithmeticsBox;
      QToolButton* BSin;
      QToolButton* BCos;
      QToolButton* BTan;
      QToolButton* BArctan;
      QToolButton* BArcsin;
      QToolButton* BArccos;
      QToolButton* BLog;
      QToolButton* BLn;
      QToolButton* BX;
      QToolButton* BExp;
      QToolButton* BCh;
      QToolButton* BSh;
      QToolButton* BErase;
      QGroupBox* FunctionBox;
      QToolButton* BUndo;
      QToolButton* BRedo;
      QGroupBox* NavigationBox;

       virtual void paintEvent(QPaintEvent *event);
/*       virtual void mouseMoveEvent(QMouseEvent *event);
       virtual void mousePressEvent(QMouseEvent *event);*/
       virtual bool event(QEvent *);
/*       virtual void mouseDoubleClickEvent(QMouseEvent *);*/
       virtual void resizeEvent(QResizeEvent* E);
};

#endif // QFORMULAEDITOR_H
