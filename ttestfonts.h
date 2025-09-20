#ifndef TTESTFONTS_H
#define TTESTFONTS_H

#include <QFont>
#include <QColor>
#include <QDialog>
#include <Base/tline.h>


namespace Ui {
    class TTestFonts;
}


class TTestFonts : public QDialog
{
    Q_OBJECT

public:
    QFont TextFont, FormulaFont;
    QColor FormulaColor, EditableColor, TextColor;

    explicit TTestFonts(QWidget *parent = 0);
    ~TTestFonts();
    void DrawScene();
    void DrawScene_2();
    void DrawOnCanvas(std::shared_ptr<TPaintCanvas> Canvas);
    void NumericOnCanvas(std::shared_ptr<TPaintCanvas> Canvas);

private:
    Ui::TTestFonts *ui;
public slots:
    void OnZoomIn();
    void OnZoomOut();
    void OnChangeFont(const QFont& F);
};

#endif // TTESTFONTS_H
