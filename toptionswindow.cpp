#include <QFontDialog>
#include <QColorDialog>
#include <QLayout>
#include <QString>
#include <QComboBox>
#include "toptionswindow.h"
#include "ui_toptionswindow.h"
#include "Base/tline.h"
#include "Base/arithmetic.h"
#include "Base/tinterval.h"
#include "Base/tphrases.h"

extern QString ApplicationDirPath;


TOptionsWindow::TOptionsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TOptionsWindow)
{
    Options = new TOptions;
    ui->setupUi(this);
    ui->comboBox->addItem(QString::fromUtf8("English"), QVariant(LangEn));
    ui->comboBox->addItem(QString::fromUtf8("Russian"), QVariant(LangRu));
    DrawSolution();
}

TOptionsWindow::~TOptionsWindow()
{    
    delete Options;
    delete ui;    
}

void TOptionsWindow::SetOptions(TOptions* Options)
{
    *this->Options = *Options;
    SetLanguage(Options->Language);
    int Index = ui->comboBox->findData(QVariant(Options->Language));
    if(Index >= 0)
    {
        ui->comboBox->setCurrentIndex(Index);
    };
}

void TOptionsWindow::on_pushButton_clicked()
{
bool ok;
QFont Font = QFontDialog::getFont(&ok, Options->TextFont, this);
    if(ok)
    {
        Options->TextFont = Font;
        DrawSolution();
    }
}

void TOptionsWindow::on_pushButton_2_clicked()
{
bool ok;
QFont Font = QFontDialog::getFont(&ok, Options->FormulaFont, this);
    if(ok)
    {
        Options->FormulaFont = Font;
        DrawSolution();
    }
}

void TOptionsWindow::on_pushButton_3_clicked()
{
    Options->TextColor = QColorDialog::getColor(Options->TextColor, this);
    DrawSolution();
}

void TOptionsWindow::on_pushButton_4_clicked()
{
    Options->FormulaColor = QColorDialog::getColor(Options->FormulaColor, this);
    DrawSolution();
}

void TOptionsWindow::on_pushButton_5_clicked()
{
   Options->BackgroundColor = QColorDialog::getColor(Options->BackgroundColor, this);
   DrawSolution();
}

void TOptionsWindow::DrawSolution()
{
TLines* Solution = new TLines;
TNumeric X("x");
TNumeric A(2); A.SetEditableFlags(ConstAllowed);
TNumeric B(3); B.SetEditableFlags(ConstAllowed);
TNumeric Line1 = MakeEquality(A*X + B*(X^2), TNumeric(0));
TNumeric Line2 = MakeEquality((A/B)*X + B*MakeLog(X, (X^2)+2), TNumeric(0));
TNumeric Line3 = MakeEquality(A*MakeSin(X) + B*(X^2), TNumeric(0));
TNumeric Line4 = MakeEquality(A*X + MakeCos(X^2), TNumeric(0));
TNumeric SystemOfEquations;
    SystemOfEquations.Operator = OperatorEqSystem;
    SystemOfEquations.OperandsPushback(Line1);
    SystemOfEquations.OperandsPushback(Line2);
    SystemOfEquations.OperandsPushback(Line3);
    SystemOfEquations.OperandsPushback(Line4);
    Solution->AddLine(new TNumeric(SystemOfEquations));
    //Solution->AddLine(new TLine(MyTranslator.tr("Solution:")));
    //Solution->AddLine(new TLine(MyTranslator.tr("Examples:")));
    Solution->AddLine(new TNumeric(MakeBelongsTo(X, TInterval(TNumeric(-1), TNumeric(2), true, false).GetNumeric())));

    QGraphicsScene *Scene = new QGraphicsScene;
    Scene->setBackgroundBrush(QBrush(Options->BackgroundColor));
    TPaintCanvas Canvas(Scene);
    Canvas.FormulaFont = Options->FormulaFont;
    Canvas.TextFont = Options->TextFont;
    Canvas.FormulaColor = Options->FormulaColor;
    Canvas.TextColor = Options->TextColor;
    Canvas.EditableColor = Options->EditableColor;
    Solution->Draw(&Canvas, 0, 0, -1);
    ui->graphicsView->setScene(Scene);

    delete Solution;
};


void TOptionsWindow::on_pushButton_6_clicked()
{
    Options->EditableColor = QColorDialog::getColor(Options->EditableColor, this);
    DrawSolution();
}

void TOptionsWindow::on_checkBox_clicked()
{
    Options->GetDefaults();
    DrawSolution();
    ui->checkBox->setChecked(false);
}

void TOptionsWindow::on_pushButton_7_clicked()
{
    this->accept();
}

void TOptionsWindow::SetLanguage(int Language)
{
    if(Options==0) return;
    Options->Language = Language;
    //Phrases->SetLanguage(Language);
    MyTranslator.Language = Language;
    DrawSolution();
}

void TOptionsWindow::on_comboBox_currentIndexChanged(int index)
{
int Language = LangEn;
    if(index >= 0)
        Language = ui->comboBox->itemData(index).toInt();
    SetLanguage(Language);
}

//*******************************************************************************
TOptions::TOptions()
{
    GetDefaults();
    Load();
}
TOptions::~TOptions()
{
    Save();
}

void TOptions::SaveColor(QSettings& S, QString Key, QColor Color)
{
    S.beginWriteArray(Key);
    int r, g, b, a;
    Color.getRgb(&r, &g, &b, &a);
    S.setValue("red", r);
    S.setValue("green", g);
    S.setValue("blue", b);
    S.setValue("alpha", a);
    S.endArray();
}

void TOptions::LoadColor(QSettings& S, QString Key, QColor &Color)
{
    S.beginReadArray(Key);
    int r, g, b, a;
    r = S.value("red", QVariant(0x88)).toInt();
    g = S.value("green", QVariant(0x88)).toInt();
    b = S.value("blue", QVariant(0x88)).toInt();
    a = S.value("alpha", QVariant(0xFF)).toInt();
    Color.setRgba(qRgba(r, g, b, a));
    S.endArray();
}

void TOptions::Load()
{
QString FileName = ApplicationDirPath + "/settings.ini";
QSettings S(FileName.toLocal8Bit().data(), QSettings::IniFormat);
    GetDefaults();
    if(S.contains("TextFont"))
    {
        TextFont.fromString(S.value("TextFont").toString());
    };
    if(S.contains("FormulaFont"))
    {
        FormulaFont.fromString(S.value("FormulaFont").toString());        
    };
    if(S.contains("TextColor"))
        LoadColor(S, "TextColor", TextColor);
    if(S.contains("FormulaColor"))
        LoadColor(S, "FormulaColor", FormulaColor);
    if(S.contains("EditableColor"))
        LoadColor(S, "EditableColor", EditableColor);
    if(S.contains("BackgroundColor"))
        LoadColor(S, "BackgroundColor", BackgroundColor);
    if(S.contains("language"))
    {
        Language = S.value("language", QVariant(LangEn)).toInt();
    }
}

void TOptions::Save()
{
QString FileName = ApplicationDirPath + "/settings.ini";
QSettings S(FileName.toLocal8Bit().data(), QSettings::IniFormat);
    S.setValue("language", QVariant(Language));
    S.setValue("TextFont", TextFont.toString());
    S.setValue("FormulaFont", FormulaFont.toString());
    SaveColor(S, "TextColor", TextColor);
    SaveColor(S, "FormulaColor", FormulaColor);
    SaveColor(S, "EditableColor", EditableColor);
    SaveColor(S, "BackgroundColor", BackgroundColor);
}


void TOptions::GetDefaults()
{
    TextFont = QFont("Helvetica", 14);
    FormulaFont = QFont("Serif", 14);
    TextColor = Qt::black;
    FormulaColor = Qt::darkBlue;
    BackgroundColor = QColor(0xFF, 0xFF, 0xAA);
    EditableColor = Qt::blue;
    Language = LangEn;
}
