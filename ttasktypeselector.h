#ifndef TTASKTYPESELECTOR_H
#define TTASKTYPESELECTOR_H

#include <QDialog>
#include <QGraphicsView>

#include <map>

#include "Base/arithmetic.h"
using namespace std;

class QFormulaSelector : public QGraphicsView {
    Q_OBJECT
  public:
    explicit QFormulaSelector(QWidget* parent = 0);
    ~QFormulaSelector();
  signals:
    void SelectionChanged();
    void SelectionDone();
  public slots:

  protected:
    //       virtual void paintEvent(QPaintEvent *event);
    //       virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent* event);
    //       virtual bool event(QEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent*);
};

namespace Ui {
class TTaskTypeSelector;
}

class TTaskTypeSelector : public QDialog {
    Q_OBJECT
    map<QGraphicsItem*, size_t> RectToNum;
    QFont TextFont, FormulaFont;
    QColor FormulaColor, EditableColor, BackgroundColor;

  public:
    size_t SelectedNum;
    vector<std::shared_ptr<TNumeric>> Types;
    explicit TTaskTypeSelector(QWidget* parent = 0);
    ~TTaskTypeSelector();

    void SetTypes(vector<std::shared_ptr<TNumeric>> Types);

    void SetFonts(QFont TextFont, QFont FormulaFont);
    void SetColors(QColor FormulaColor, QColor EditableColor, QColor BackgroundColor);

  private:
    Ui::TTaskTypeSelector* ui;
  public slots:
    void SelectionChanged();
    void SelectionCanceled();
    void SelectionDone();
};

#endif  // TTASKTYPESELECTOR_H
