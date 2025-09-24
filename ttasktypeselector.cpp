#include "ttasktypeselector.h"

#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QGraphicsScene>

#include "Base/formulaplotter.h"
#include "Base/tline.h"
#include "ui_ttasktypeselector.h"

TTaskTypeSelector::TTaskTypeSelector(QWidget* parent) : QDialog(parent), ui(new Ui::TTaskTypeSelector) {
    ui->setupUi(this);
    connect(ui->graphicsView, SIGNAL(SelectionChanged()), this, SLOT(SelectionChanged()));
    connect(ui->graphicsView, SIGNAL(SelectionDone()), this, SLOT(SelectionDone()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(SelectionDone()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(SelectionCanceled()));
}

TTaskTypeSelector::~TTaskTypeSelector() {
    delete ui;
}

void TTaskTypeSelector::SetTypes(vector<std::shared_ptr<TNumeric>> Types) {
    QGraphicsScene* Scene = new QGraphicsScene(ui->graphicsView);
    Scene->setBackgroundBrush(QBrush(BackgroundColor));
    auto Canvas = std::make_shared<TPaintCanvas>(Scene);
    Canvas->TextFont = TextFont;
    Canvas->FormulaFont = FormulaFont;
    Canvas->Font = Canvas->FormulaFont;
    Canvas->FormulaColor = FormulaColor;
    Canvas->EditableColor = EditableColor;
    this->Types = Types;
    int Y = 0;
    int X = 0;
    RectToNum.clear();
    QGraphicsItem* GI;
    for (size_t i = 0; i < Types.size(); i++) {
        TConstFormulaPlotter fp(Types[i]);
        auto [W, H, D] = fp.GetTextRectangle(Canvas);
        QGraphicsItem* I = Scene->addRect(X - 5, Y - H - 5, W + 10, H + D + 10, QPen(BackgroundColor));
        fp.Draw(Canvas, X, Y);
        I->setFlag(QGraphicsItem::ItemIsSelectable);
        Y += H + D + (H + D);  // one height spacing
        RectToNum.insert(pair<QGraphicsItem*, size_t>(I, i));
        GI = I;
    }
    (void)GI;
    ui->graphicsView->setScene(Scene);
    ui->graphicsView->ensurePolished();
}

void TTaskTypeSelector::SelectionChanged() {
    map<QGraphicsItem*, size_t>::iterator it;
    for (it = RectToNum.begin(); it != RectToNum.end(); it++) {
        QGraphicsItem* i = it->first;
        if (i->isSelected()) {
            SelectedNum = it->second;
            ui->pushButton->setDisabled(false);
            return;
        };
    };
    ui->pushButton->setDisabled(true);
}

void TTaskTypeSelector::SetFonts(QFont TextFont, QFont FormulaFont) {
    this->TextFont = TextFont;
    this->FormulaFont = FormulaFont;
}

void TTaskTypeSelector::SetColors(QColor FormulaColor, QColor EditableColor, QColor BackgroundColor) {
    this->FormulaColor = FormulaColor;
    this->EditableColor = EditableColor;
    this->BackgroundColor = BackgroundColor;
}

void TTaskTypeSelector::SelectionCanceled() {
    reject();
}

void TTaskTypeSelector::SelectionDone() {
    accept();
}

//******************************************************************************************************

QFormulaSelector::QFormulaSelector(QWidget* parent) : QGraphicsView(parent) {}

QFormulaSelector::~QFormulaSelector() {}

void QFormulaSelector::mouseDoubleClickEvent(QMouseEvent* Event) {
    Q_UNUSED(Event);
    emit SelectionDone();
}

void QFormulaSelector::mousePressEvent(QMouseEvent* event) {
    QGraphicsView::mousePressEvent(event);
    emit SelectionChanged();
}
