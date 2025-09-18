#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Base/arithmetic.h"
#include "Base/THTMLWriter.h"

#include "taboutdialog.h"
#include "tfreeaboutdialog.h"
#include "tselectslausize.h"
#include "teuladialog.h"
#include "tsolutionbrowser.h"
#include "ttestfonts.h"


#include "Problems/ObjectsFactory.h"

#include <QLabel>
#include <QMouseEvent>
#include <QTreeView>
#include <QGraphicsScene>
#include <vector>
#include <ttasktypeselector.h>
#include <QErrorMessage>
#include <QtPrintSupport/QPrinter>
#include <QFileDialog>
#include <QMessageBox>
#include <QFontDialog>
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>
#include <QApplication>
#include <QAbstractScrollArea>
#include <QScrollBar>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDir>
#include <QFile>
#include <QtWebView/QtWebView>
//#include <QtWebView/QWebPage>
//#include <QtWebView/QWebFrame>
//#include <QtWebKit/QWebElement>
#include <QGroupBox>
#include <QCheckBox>
#include <QTime>
#include <QSizePolicy>

#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

extern QApplication* qAppl;
extern QString ApplicationDirPath;

#define VERSION "1.88"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setEnabled(true);

    //GViewScene = new QGraphicsScene(0);
    //ui->graphicsView->setScene(GViewScene);


    ui->FormulaEditor->SetArithmeticsVisible(true);
    ui->FormulaEditor->SetFunctionsVisible(true);

    connect(ui->FormulaEditor, SIGNAL(OnSelectionChanged(TNumeric*)), this, SLOT(OnFormulaSelectionChanged(TNumeric*)));

    Language = LangEn;

    //FreeDebugVersion = true;
    FreeDebugVersion = false;

    //Offline = true;
    Offline = false;

    Opt = new TOptions;
    this->loadLanguage(Opt->Language);
    SetLanguage(Language);
    //Phrases = new TMathstylePhrases(Language); //Phrases - глобальная переменная
    ui->FormulaEditor->TranslateButtons();

    /*Item = new QTreeWidgetItem(ui->treeWidget);
    Item->setText(0, QString::fromUtf8("Графики"));
    CreateTreeItem(Item, new TPolynomialGraphic);*/

    MySetIconSize(32);


    Problem = 0;
    SelectedProblem = -1;
    CurrentItem = 0;


    AccessManager = new QNetworkAccessManager(this);
    connect(AccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));


    this->setWindowTitle(QString::fromUtf8("MathStyle (C) Pro v.") + VERSION);
    ui->pushButton_5->setVisible(false);

    CreateProblems();
    CreateBook();
    DrawConditions();


   ui->DeleteBtn->setEnabled(false);
   ui->pushButton_8->setEnabled(false);
   ui->pushButton_3->setEnabled(false);
   //this->setWindowIcon(QIcon(""));

   DirCount = 0;

   TemporaryRootAbsPath = QDir().absoluteFilePath("./").toLocal8Bit().data();  

   SelectedKeywordsLayout = new QVBoxLayout(0);
   ui->Frame2Layout->addLayout(SelectedKeywordsLayout);

   //KeywordsGlobeView and KeywordsButtons may be uninitialized
   KeywordsGlobeView = 0;
   KeywordsButtons = 0;

   //KeywordsGlobeView = new TGlobeView(ui->frame_2);
   KeywordsButtons = new TButtonsSheetView(ui->frame_2);

   if(KeywordsGlobeView)
   {
       connect(KeywordsGlobeView, SIGNAL(Picked(QString)), this, SLOT(OnKeywordPicked(QString)));
       ui->Frame2Layout->addWidget(KeywordsGlobeView);
   }
   if(KeywordsButtons)
   {
       connect(KeywordsButtons, SIGNAL(Picked(QString)), this, SLOT(OnKeywordPicked(QString)));
       ui->Frame2Layout->addWidget(KeywordsButtons);
   };


   CreateKeywords(Problems);

   //SelectedKeywordsLayout->setMargin(0);
   //SelectedKeywordsLayout->setContentsMargins(5, 1, 5, 1);
//   ui->groupBox->setContentsMargins(0, 0, 0, 0);
   //ui->frame_4->setLayout(SelectedKeywordsLayout);


   SignalMapper = new QSignalMapper(this);
   connect(SignalMapper, SIGNAL(mapped(QWidget*)), this, SLOT(KeywordClicked(QWidget*)));
   InitProblemsSolved();

   if(!FreeDebugVersion)
       ui->actionTest_fonts->setVisible(false);

}

MainWindow::~MainWindow()
{
    DoneProblemsSolved();
    for(size_t i = 0; i < Problems.size(); i++)
    {
        delete Problems[i];
    }
    delete ui; //нужно ли удалять ui? может его должен удалять его parent?
    delete AccessManager;
    delete Opt;
    //delete SelectedKeywordsLayout;
    //delete Phrases;
    DeleteDirs();
//    if(GViewScene)delete GViewScene;
}

void MainWindow::CreateProblems()
{
    SimplifyProblem = new TSimplifyProblem;
    LinearEquality = new TLinearEquality;
    SquareEquality = new TSquareEquality;
    PolynomialEquality4 = new TPolynomialEquality(4);
    PolynomialEquality8 = new TPolynomialEquality(8);
    RationalSumEquality1 = new TRationalSumEquality(1);
    RationalSumEquality2 = new TRationalSumEquality(2);
    PowerEquality2 = new TPowerEquality(2);
    LinearTrigEquality = new TLinearTrigEquality();
    HomogeneousTrigEquality2 = new THomogeneousTrigEquality(2);
    HomogeneousTrigEquality3 = new THomogeneousTrigEquality(3);
    HomogeneousTrigEquality4 = new THomogeneousTrigEquality(4);
    SystemOfEquations22 = new TSystemOfEquations(2, 2);
    SystemOfEquations33 = new TSystemOfEquations(3, 3);
    SystemOfEquations44 = new TSystemOfEquations(4, 4);
    SystemOfEquations55 = new TSystemOfEquations(5, 5);
    LinearInequality = new TLinearInequality;
    SquareInequality = new TSquareInequality;
    PolynomialInequality8 = new TPolynomialInequality(8);
    SystemOfLinearInequalities = new TSystemOfLinearInequalities;
    SystemOfSquareInequalities = new TSystemOfSquareInequalities;
    SetOfLinearInequalities = new TSetOfLinearInequalities;
    SetOfSquareInequalities = new TSetOfSquareInequalities;
    PolynomDerivative4 = new TPolynomDerivative(4);
    RationalFunctionDerivative44 = new TRationalFunctionDerivative(4, 4);
    PolynomDerivative = new TPolynomDerivative;
    RationalFunctionDerivative = new TRationalFunctionDerivative;
    DerivativeProblem = new TDerivativeProblem;
    PolynomIntegralProblem = new TPolynomIntegralProblem;
    RationalIntegralProblem22 = new TRationalIntegralProblem(2, 2);
    RationalIntegralProblem45 = new TRationalIntegralProblem(4, 5);
    RationalIntegralProblem66 = new TRationalIntegralProblem(6, 6);
    RationalIntegralProblemFactorized = new TRationalIntegralProblem2(4, 2, 4);
    IntegralProblem = new TIntegralProblem;
    MaclaurinSeriesProblem3 = new TMaclaurinSeriesProblem(3);
    MaclaurinSeriesProblem5 = new TMaclaurinSeriesProblem(5);    
    Problems.push_back(SimplifyProblem);
    Problems.push_back(LinearEquality);
    Problems.push_back(SquareEquality);
    Problems.push_back(PolynomialEquality4);
    Problems.push_back(PolynomialEquality8);
    Problems.push_back(RationalSumEquality1);
    Problems.push_back(RationalSumEquality2);
    Problems.push_back(PowerEquality2);
    Problems.push_back(LinearTrigEquality);
    Problems.push_back(HomogeneousTrigEquality2);
    Problems.push_back(HomogeneousTrigEquality3);
    Problems.push_back(HomogeneousTrigEquality4);
    Problems.push_back(SystemOfEquations22);
    Problems.push_back(SystemOfEquations33);
    Problems.push_back(SystemOfEquations44);
    Problems.push_back(SystemOfEquations55);
    Problems.push_back(LinearInequality);
    Problems.push_back(SquareInequality);
    Problems.push_back(PolynomialInequality8);
    Problems.push_back(SystemOfLinearInequalities);
    Problems.push_back(SystemOfSquareInequalities);
    Problems.push_back(SetOfLinearInequalities);
    Problems.push_back(SetOfSquareInequalities);
    Problems.push_back(PolynomDerivative4);
    Problems.push_back(RationalFunctionDerivative44);
    Problems.push_back(PolynomDerivative);
    Problems.push_back(RationalFunctionDerivative);
    Problems.push_back(DerivativeProblem);
    Problems.push_back(PolynomIntegralProblem);
    Problems.push_back(RationalIntegralProblem22);
    Problems.push_back(RationalIntegralProblem45);
    Problems.push_back(RationalIntegralProblem66);
    Problems.push_back(RationalIntegralProblemFactorized);
    Problems.push_back(IntegralProblem);
    Problems.push_back(MaclaurinSeriesProblem3);
    Problems.push_back(MaclaurinSeriesProblem5);
}

void MainWindow::CreateBook()
{
    QTreeWidget* Parent= ui->treeWidget_2;
    QTreeWidgetItem *Item2;
    //Упростить выражение
    CreateTreeItem(Parent, SimplifyProblem);
    //KeywordToTabsMap.insert(QString::fromUtf8("Упростить"), ui->SimplifyTab);

    //Уравнения
    Parent = ui->treeWidget_3; //родительский узел
    //KeywordToTabsMap.insert(QString::fromUtf8("Уравнения"), ui->EqualitiesTab);

    Item2 = CreateNode(Parent, tr("Polynomial"));
    CreateTreeItem(Item2, LinearEquality);
    CreateTreeItem(Item2, SquareEquality);
    CreateTreeItem(Item2, PolynomialEquality4);
    CreateTreeItem(Item2, PolynomialEquality8);

    Item2 = CreateNode(Parent, tr("Rational"));
    CreateTreeItem(Item2, RationalSumEquality1);
    CreateTreeItem(Item2, RationalSumEquality2);
    Item2 = CreateNode(Parent, tr("Others"));
    CreateTreeItem(Item2, PowerEquality2);
    QTreeWidgetItem* Item3 = CreateNode(Item2, tr("Trigonometric"));
    CreateTreeItem(Item3, LinearTrigEquality);
    Item2 = CreateNode(Parent, tr("Homogeneous trigonometric"));
    CreateTreeItem(Item2, HomogeneousTrigEquality2);
    CreateTreeItem(Item2, HomogeneousTrigEquality3);
    CreateTreeItem(Item2, HomogeneousTrigEquality4);

    //Системы уравнений
    //KeywordToTabsMap.insert(QString::fromUtf8("СЛАУ"), SLAUTab);
    Parent = ui->treeWidget_8; //родительский узел
    Item2 = CreateNode(Parent, tr("Linear square"));
    CreateTreeItem(Item2, SystemOfEquations22);
    CreateTreeItem(Item2, SystemOfEquations33);
    CreateTreeItem(Item2, SystemOfEquations44);
    CreateTreeItem(Item2, SystemOfEquations55);
    CustomSystems = 0;


    //Неравенства
    Parent = ui->treeWidget_4; //родительский узел
    //KeywordToTabsMap.insert(QString::fromUtf8("Неравенства"), ui->InequalitiesTab);
    Item2 = CreateNode(Parent, tr("Single"));
    CreateTreeItem(Item2, LinearInequality);
    CreateTreeItem(Item2, SquareInequality);
    CreateTreeItem(Item2, PolynomialInequality8);

    Item2 = CreateNode(Parent, tr("Systems"));
    CreateTreeItem(Item2, SystemOfLinearInequalities);
    CreateTreeItem(Item2, SystemOfSquareInequalities);

    Item2 = CreateNode(Parent, tr("Sets"));
    CreateTreeItem(Item2, SetOfLinearInequalities);
    CreateTreeItem(Item2, SetOfSquareInequalities);


    //Системы и совокупности
    Parent = ui->treeWidget_6; //родительский узел

    Item2 = CreateNode(Parent, tr("Systems"));
    //KeywordToTabsMap.insert(QString::fromUtf8("Неравенства"), ui->SystemsTab);
    CreateTreeItem(Item2, SystemOfLinearInequalities);
    CreateTreeItem(Item2, SystemOfSquareInequalities);

    Item2 = CreateNode(Parent, tr("Sets"));
    CreateTreeItem(Item2, SetOfLinearInequalities);
    CreateTreeItem(Item2, SetOfSquareInequalities);


    //Производные, Интегралы, Ряды
    Parent = ui->treeWidget_5; //родительский узел
    Item2 = CreateNode(Parent, tr("Derivatives"));
    CreateTreeItem(Item2, PolynomDerivative4);
    CreateTreeItem(Item2, RationalFunctionDerivative44);
    CreateTreeItem(Item2, PolynomDerivative);
    CreateTreeItem(Item2, RationalFunctionDerivative);
    CreateTreeItem(Item2, DerivativeProblem);

    Item2 = CreateNode(Parent, tr("Integrals"));
    CreateTreeItem(Item2, PolynomIntegralProblem);
    CreateTreeItem(Item2, RationalIntegralProblem22);
    CreateTreeItem(Item2, RationalIntegralProblem45);
    CreateTreeItem(Item2, RationalIntegralProblem66);

    Item2 = CreateNode(Parent, tr("Maclaurin polynomial"));
    CreateTreeItem(Item2, MaclaurinSeriesProblem3);
    CreateTreeItem(Item2, MaclaurinSeriesProblem5);

}

void MainWindow::CreateKeywords(vector<TProblem*> Problems)
{
    vector<string> KeyWords;
    for(size_t i = 0; i < Problems.size(); i++)
    {
        vector<string> CurKeywords;
        CurKeywords = Problems[i]->GetKeyWords();
        for(size_t j = 0; j < CurKeywords.size(); j++)
            if(std::find(KeyWords.begin(), KeyWords.end(), CurKeywords[j]) == KeyWords.end() &&
                    std::find(SelectedKeywords.begin(), SelectedKeywords.end(), CurKeywords[j]) == SelectedKeywords.end())
                KeyWords.push_back(CurKeywords[j]);
    }
    if(KeywordsGlobeView)
    {
        KeywordsGlobeView->ClearStrings();
        for(size_t i = 0; i < KeyWords.size(); i++)
            KeywordsGlobeView->AddString(QString::fromUtf8(KeyWords[i].c_str()));
    };

    if(KeywordsButtons)
    {
        KeywordsButtons->ClearStrings();
        for(size_t i = 0; i < KeyWords.size(); i++)
            KeywordsButtons->AddString(QString::fromUtf8(KeyWords[i].c_str()));
    };
}

vector<TProblem*> MainWindow::GetProblemsByKeywords()
{
vector<TProblem*> Res;
    for(size_t i = 0; i < Problems.size(); i++)
    {
        vector<string> keywords = Problems[i]->GetKeyWords();
        bool AllKeywordsFound = true;
        for(size_t j = 0; j < SelectedKeywords.size(); j++)
            if(std::find(keywords.begin(), keywords.end(), SelectedKeywords[j]) == keywords.end())
            {
                AllKeywordsFound = false;
                break;
            }
        if(AllKeywordsFound) Res.push_back(Problems[i]);
    };
    return Res;
}

string MainWindow::GetDirectory(size_t DirCount)
{
    stringstream Path;
    Path<<TemporaryRootAbsPath<<"/Temp"<<DirCount;
    return Path.str();
}

void MainWindow::DeleteDirs()
{
    for(size_t i = 0; i < Files.size(); i++)
    {
        QFile(QString::fromLocal8Bit(Files[i].c_str())).remove();
    }
    for(size_t i = 0; i < Directories.size(); i++)
    {
        string dir = Directories[i];
        QDir().rmdir(QString::fromLocal8Bit(dir.c_str()));
    }
}

QTreeWidgetItem* MainWindow::CreateTreeItem(QTreeWidgetItem* I, TProblem* P)
{
QTreeWidgetItem* TreeItem = new QTreeWidgetItem(I);
    TreeItem->setText(0, QString::fromUtf8(P->GetShortTask().c_str()));
    TreeItem->setIcon(0, QIcon(":Style/Pictures/Style/paper&pencil_48.png"));
//    Problems.push_back(P);
    WidgetToProblemMap.insert(TreeItem, P);
    return TreeItem;
}

QTreeWidgetItem* MainWindow::CreateTreeItem(QTreeWidget* I, TProblem* P)
{
QTreeWidgetItem* TreeItem = new QTreeWidgetItem(I);
    TreeItem->setText(0, QString::fromUtf8(P->GetShortTask().c_str()));
    TreeItem->setIcon(0, QIcon(":Style/Pictures/Style/paper&pencil_48.png"));
    //Problems.push_back(P);
    WidgetToProblemMap.insert(TreeItem, P);
    return TreeItem;
}

void MainWindow::ClearTemporaryTree()
{
    for(int i = 0; i < ui->TemporaryTree->topLevelItemCount(); )
    {
        QTreeWidgetItem *Item = ui->TemporaryTree->takeTopLevelItem(i);
        QMap<QTreeWidgetItem*, TProblem*>::iterator it = WidgetToProblemMap.find(Item);
        WidgetToProblemMap.erase(it);
    };
}


QTreeWidgetItem* MainWindow::CreateNode(QTreeWidget* Parent, string name)
{
    QTreeWidgetItem *I = CreateNode(Parent, QString::fromUtf8(name.c_str()));
    return I;
}
QTreeWidgetItem* MainWindow::CreateNode(QTreeWidgetItem* Parent, string name)
{
    QTreeWidgetItem *I = CreateNode(Parent, QString::fromUtf8(name.c_str()));
    return I;
}

QTreeWidgetItem* MainWindow::CreateNode(QTreeWidget* Parent, QString name)
{
    QTreeWidgetItem* Item2 = new QTreeWidgetItem(Parent);
    Item2->setText(0, name);
    Item2->setIcon(0, QIcon(":Style/Pictures/Style/folder_48.png"));
    QFont F = Item2->font(0);
    F.setBold(true);
    Item2->setFont(0, F);
    return Item2;
}
QTreeWidgetItem* MainWindow::CreateNode(QTreeWidgetItem* Parent, QString name)
{
    QTreeWidgetItem* Item2 = new QTreeWidgetItem(Parent);
    Item2->setText(0, name);
    Item2->setIcon(0, QIcon(":Style/Pictures/Style/folder_48.png"));
    QFont F = Item2->font(0);
    F.setBold(true);
    Item2->setFont(0, F);
    return Item2;
}


void MainWindow::MySetIconSize(int Size)
{
    ui->treeWidget_2->setIconSize(QSize(Size, Size));
    ui->treeWidget_3->setIconSize(QSize(Size, Size));
    ui->treeWidget_4->setIconSize(QSize(Size, Size));
    ui->treeWidget_5->setIconSize(QSize(Size, Size));
    ui->treeWidget_6->setIconSize(QSize(Size, Size));
    ui->FavoritesTree->setIconSize(QSize(Size, Size));
}


void MainWindow::DrawConditions()
{
    ui->FormulaEditor->SetFonts(Opt->TextFont, Opt->FormulaFont);
    ui->FormulaEditor->SetColors(Opt->FormulaColor, Opt->EditableColor,  Opt->BackgroundColor);
    if(Problem)
    {
        ui->FormulaEditor->SetTask(Problem->GetTask());
        ui->FormulaEditor->SetFormula(Problem->Conditions);
        ui->pushButton_6->setEnabled(Problem->CanRandomize);
    } else  {
        ui->pushButton_6->setEnabled(false);
        ui->FormulaEditor->SetTask("");
        ui->FormulaEditor->SetFormula(0);
    }
    ui->FormulaEditor->repaint();
}
void MainWindow::SelectProblemByIndex(size_t Index)
{
    Q_UNUSED(Index);
    //2del
/*    SelectedProblem = Index;
    Problem = Problems[Index];
    CurrentItem = (QTreeWidgetItem*) Problems[Index]->TreeItem;
    DrawConditions();*/
}

void MainWindow::SelectProblemByWidget(QTreeWidgetItem* item)
{
    if(item == 0)
    {
        Problem = 0;
        CurrentItem = 0;
        SelectedProblem = -1;
    }
    if(WidgetToProblemMap.contains(item))
    {
        TProblem* P = WidgetToProblemMap.value(item);
        for(size_t i = 0; i < Problems.size(); i++)
            if(Problems[i] == P)
            {
                SelectedProblem = i;
                Problem = Problems[i];
                CurrentItem = item;                
                break; //нашли и отрисовали. Больше ничего не ищем.
            };
    };
    DrawConditions();
}

void MainWindow::DeleteProblem(size_t SelectedProblem) // for what?
{
TProblem* P = Problems[SelectedProblem];
    for(QMap<QTreeWidgetItem*, TProblem*>::iterator iterator = WidgetToProblemMap.begin(); iterator != WidgetToProblemMap.end(); )
    {
        if(iterator.value() == P)
            WidgetToProblemMap.erase(iterator);
        //else
            iterator++;
//        cout<<WidgetToProblemMap.size()<<endl;

    };
    Problems.erase(Problems.begin() + SelectedProblem);
    delete P;
}


void MainWindow::OnFormulaSelectionChanged(TNumeric* NewSelection)
//Selecting the type of subproblem (like ax+b<c, or ax+b <= c or > or >=)
{
    if(Problem)
    {
        vector<TNumeric> Types = Problem->GetTypes(NewSelection);
        if(Types.size() > 0)
        {
            TTaskTypeSelector *S = new TTaskTypeSelector(this);
            S->SetFonts(Opt->TextFont, Opt->FormulaFont);
            S->SetColors(Opt->FormulaColor, Opt->EditableColor,  Opt->BackgroundColor);
            S->SetTypes(Types);
            S->exec();
            if(S->result() == S->Accepted)
            {
                Problem->SetType(NewSelection, S->SelectedNum);
                DrawConditions();
            }
            delete S;
        }
    }
}

void MainWindow::on_treeWidget_3_itemPressed(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    SelectProblemByWidget(item);
}

void MainWindow::on_treeWidget_2_itemPressed(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    SelectProblemByWidget(item);
}

void MainWindow::on_treeWidget_4_itemPressed(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    SelectProblemByWidget(item);
}

void MainWindow::on_treeWidget_5_itemPressed(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    SelectProblemByWidget(item);
}

void MainWindow::on_treeWidget_6_itemPressed(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    SelectProblemByWidget(item);
}
void MainWindow::on_TemporaryTree_itemPressed(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    SelectProblemByWidget(item);
}



void MainWindow::on_FavoritesTree_itemPressed(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    if(item != 0)
    {
        ui->pushButton_8->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
    }
    else
    {
        ui->pushButton_8->setEnabled(false);
        ui->pushButton_3->setEnabled(false);
    };
    SelectProblemByWidget(item);
}

void MainWindow::on_treeWidget_8_itemPressed(QTreeWidgetItem* item, int column)
{
    Q_UNUSED(column);
    if(item != 0 && item->parent() == CustomSystems)
        ui->DeleteBtn->setEnabled(true);
    else
        ui->DeleteBtn->setEnabled(false);
    SelectProblemByWidget(item);
}


void MainWindow::FileOpen()
{
QString fileName = QFileDialog::getOpenFileName(this,tr("Open..."),"", QString::fromUtf8(("MathStyle Files (*.mst)")));
    if(fileName.isEmpty())
        return;
    ifstream f(fileName.toUtf8().data(), ios::in);
    if(!f)
    {
           QErrorMessage EM;
           EM.setWindowTitle(tr("Error"));
           EM.showMessage(tr("Can not open file for reading."));
           EM.exec();
    } else {
        try
        {
            TProblem *P;
            char MsgBuf[256];
            P = LoadFromFile(f, MsgBuf, 255);
            f.close();
            if(P == 0)
            {
                QErrorMessage EM;
                EM.setWindowTitle(tr("Error"));
                EM.showMessage(QString::fromUtf8(MsgBuf));
                EM.exec();
            } else {
                CreateTreeItem(ui->FavoritesTree, P);
                Problems.push_back(P);


                //устанавливаем загруженную проблему в качестве текущей
                Problem = P;
                //ui->treeWidget_7->setCurrentItem(); //2do
                DrawConditions();
                ui->tabWidget_2->setCurrentWidget(ui->FavoritesTab);
            }
         } catch (...)
        {
            QErrorMessage EM;
            EM.setWindowTitle(tr("Error"));
            EM.showMessage(tr("Can not open file."));
            EM.exec();
        }
    }
}

void MainWindow::FileSave()
{
    if(Problem == 0 || Problem->Conditions == 0) return;
QString fileName = QFileDialog::getSaveFileName(this, tr("Save as..."), "", QString::fromUtf8(("MathStyle Files (*.mst)")));
    if(fileName.isEmpty())
            return;
ofstream f(fileName.toUtf8().data(), ios::out | ios::trunc);
    if(!f)
    {
            QErrorMessage EM;
            EM.setWindowTitle(tr("Error"));
            EM.showMessage(tr("Can not open file for writing."));
            EM.exec();
    } else {
            try
            {
                SaveToFile(f, Problem);
                f.close();
            } catch (...)
            {
                QErrorMessage EM;
                EM.setWindowTitle(tr("Error"));
                EM.showMessage(tr("Can not save to file"));
                EM.exec();
            }
    }
}

void MainWindow::Exit()
{
    close();
}

void MainWindow::Solve()
{
    try
    {
        if(Problem)
        {
            ProblemsSolved[SelectedProblem]++;

            QElapsedTimer Time;
            Time.start();
            THTMLWriter *Writer = new THTMLWriter;
            string Dir = GetDirectory(DirCount++);
            string Path = Dir + "/index.html";

            Writer->FormulaFont = Opt->FormulaFont;
            Writer->TextFont = Opt->TextFont;
            Writer->FormulaColor = Opt->FormulaColor;
            Writer->TextColor = Opt->TextColor;
            Writer->EditableColor = Opt->EditableColor;

            Writer->BeginWrite(Path.c_str());
            if(Problem->Solve(Writer))
            {
                /*if(Problem->Solution && !FreeVersion && !LicenseChecker->IsValid())
                {
                    Problem->Solution->AddLine(new TText(tr("Unregistered copy!").toUtf8().data()));
                    Problem->Solution->AddLine(new TText(tr("Registered copy is faster!").toUtf8().data()));
                };*/
            } else {
                    QErrorMessage EM;
                EM.setWindowTitle(tr("Error"));
                EM.showMessage(tr("Can not solve using selected method"));
                EM.exec();
            }
            Writer->EndWrite();
            Files.insert(Files.end(), Writer->created_files.begin(), Writer->created_files.end());
                Directories.push_back(Dir);
            int Milliseconds = Time.elapsed();
            stringstream str;
            str<<Problem->GetShortTask()<<string(tr(" (elapsed ").toUtf8())<<double(Milliseconds)/1000.0L<<string(tr(" secs)").toUtf8());
            TSolutionBrowser *B = new TSolutionBrowser(this);
            B->SetCaption(QString::fromUtf8(str.str().c_str()));
            B->show();
            B->activateWindow();
            B->Load(QString::fromLocal8Bit(Writer->GetFolder().c_str()), QString::fromLocal8Bit(Writer->GetFileName().c_str()));

            delete Writer;
        };
    } catch(const char* str) {
        //ошибка скорее всего произошла из-за некорректного ввода
        QMessageBox MB;
        MB.setText(tr("Error. Can't solve. Check if the problem is typed correctly."));
        MB.exec();
    } catch(...)
    {
        //неизвестная ошибка
        QMessageBox MB;
        MB.setText(tr("Error. Сan't solve."));
        MB.exec();
    }

}

void MainWindow::Options()
{    
TOptionsWindow *OptionsWindow = new TOptionsWindow(this);
    OptionsWindow->SetOptions(Opt);
    OptionsWindow->exec();
    *Opt = *OptionsWindow->Options;
    if(this->Language != Opt->Language)
    {
        QMessageBox MB;
        MB.setText("Restart application to change the language!");
        MB.exec();
    }
    //this->loadLanguage(Opt->Language);
    SetLanguage(Opt->Language);
    DrawConditions();
}

void MainWindow::About()
{
    if(FreeDebugVersion)
    {
        TFreeAboutDialog *FreeAboutDialog = new TFreeAboutDialog(this);
        FreeAboutDialog->exec();
        delete FreeAboutDialog;
    } else {
        TAboutDialog *AboutDialog = new TAboutDialog(this);
        AboutDialog->exec();
        delete AboutDialog;
    };
}

void MainWindow::ShowEULA()
{
    TEulaDialog *EulaDialog = new TEulaDialog(this);
    EulaDialog->exec();
    delete EulaDialog;
}

void MainWindow::changeEvent(QEvent* event)
{
    /*if(0 != event)
    {
        switch(event->type())
        {
        // this event is send if a translator is loaded
        case QEvent::LanguageChange:
        {
                ui->retranslateUi(this);
                QMessageBox MB;
                MB.setText("Restart application to change the language!");
                MB.exec();
         }
            break;
        // this event is send, if the system, language changes
        case QEvent::LocaleChange:
            {
                QString locale = QLocale::system().name();
                locale.truncate(locale.lastIndexOf('_'));
                loadLanguage(locale);
            }
            break;
        }
    }*/
    QMainWindow::changeEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent* E)
{
    QMainWindow::resizeEvent(E);
}

void switchTranslator(QTranslator& translator, const QString& filename)
{
    // remove the old translator
    qAppl->removeTranslator(&translator);

    // load the new translator
    if(translator.load(filename))
        qAppl->installTranslator(&translator);
    if(translator.load(filename, ApplicationDirPath))
        qAppl->installTranslator(&translator);
}

void MainWindow::loadLanguage(const QString& rLanguage)
{
/*    if(m_currLang != rLanguage)
    {
        m_currLang = rLanguage;*/
        QLocale locale = QLocale(rLanguage);
        QLocale::setDefault(locale);
        //QString languageName = QLocale::languageToString(locale.language());
        switchTranslator(m_translator, QString("lang_%1.qm").arg(rLanguage));
        //switchTranslator(m_translatorQt, QString("qt_%1.qm").arg(rLanguage));
    //}
    ui->retranslateUi(this);
}

void MainWindow::loadLanguage(int Language)
{
    QString rLanguage;
    switch(Language)
    {
    case LangRu: rLanguage = "ru"; break;
    case LangEn:
    default:
        rLanguage = "en"; break;
    };
    this->Language = Language;
    loadLanguage(rLanguage);
}


void MainWindow::SetLanguage(int Language)
{
    MyTranslator.Language = Language;
}

void MainWindow::on_CreateNewBtn_clicked()
{
    TSelectSLAUsize *S = new TSelectSLAUsize(0);
    S->exec();
    if(S->result() == S->Accepted)
    {
        int MaxDim = 10;
        if(S->VarsCount > MaxDim || S->EqCount > MaxDim)
        {
            QMessageBox MB;
            //MB.setTextFormat(Qt::TextH);
            QString Num;
            Num.setNum(MaxDim);
            QString Str = tr("The dimensions must be less then ") + Num;
            MB.setText(Str);
            MB.exec();
        } else {
            if(CustomSystems == 0)
                CustomSystems = CreateNode(ui->treeWidget_8, tr("Custom size"));
            TSystemOfEquations *E = new TSystemOfEquations(S->VarsCount, S->EqCount);
            Problems.push_back(E);

            CreateTreeItem(CustomSystems, E);
            CustomSystems->setExpanded(true);
       };
    }
    delete S;
}

void MainWindow::on_DeleteBtn_clicked()
{
    if(Problem && CurrentItem && CurrentItem->parent() == CustomSystems)
    {
        DeleteProblem(SelectedProblem);
        Problem = 0;
        if(CurrentItem != 0)
            delete CurrentItem;
        SelectProblemByWidget(0);
    };
}

void MainWindow::on_pushButton_8_clicked()
//Delete saved task
{
    if(Problem && CurrentItem)
    {
        DeleteProblem(SelectedProblem);
        Problem = 0;
        if(CurrentItem != 0)
            delete CurrentItem;
        SelectProblemByWidget(0);
    }
}
void MainWindow::OnKeywordPicked(QString Key)
{
    ClearTemporaryTree();

    ui->tabWidget_2->setCurrentWidget(ui->Temporary_tab);
    SelectedKeywords.push_back(Key.toUtf8().data());
    vector<TProblem*> P = GetProblemsByKeywords();


    for(size_t i = 0; i < P.size(); i++)
        CreateTreeItem(ui->TemporaryTree, P[i]);
    CreateKeywords(P);

    QPushButton *B = new QPushButton(this);
    B->setText(Key);
    B->setMinimumHeight(20);
    SelectedKeywordsLayout->addWidget(B);

    SignalMapper->setMapping(B, B);
    connect(B, SIGNAL(clicked()), SignalMapper, SLOT(map()));
}
void MainWindow::KeywordClicked(QWidget* Widget)
{
    SelectedKeywordsLayout->removeWidget(Widget);
    string Str = ((QAbstractButton*)Widget)->text().toUtf8().data();
    vector<string>::iterator it = std::find(SelectedKeywords.begin(), SelectedKeywords.end(), Str);
    if(it != SelectedKeywords.end())
    {
        SelectedKeywords.erase(it);
        vector<TProblem*> P = GetProblemsByKeywords();
        ClearTemporaryTree();
        for(size_t i = 0; i < P.size(); i++)
            CreateTreeItem(ui->TemporaryTree, P[i]);
        CreateKeywords(P);        
    }
    delete Widget;
}
void MainWindow::MoveToFavorites()
{
    if(Problem != 0 && CurrentItem && CurrentItem->treeWidget() != ui->FavoritesTree)
    {
        TProblem *NewProblem = MakeCopy(Problem);
        Problems.push_back(NewProblem);
        QTreeWidgetItem* I = CreateTreeItem(ui->FavoritesTree, NewProblem);
        ui->tabWidget_2->setCurrentWidget(ui->FavoritesTab);

        ui->FavoritesTree->setCurrentItem(I);
        SelectProblem();
    };
}

void MainWindow::on_pushButton_9_clicked()
{
    FileOpen();
    SelectProblem();
}

void MainWindow::on_pushButton_3_clicked()
{
    FileSave();
}

void MainWindow::SelectProblem()
{
QWidget *W = ui->tabWidget_2->currentWidget();
QList<QObject*> Objects = W->children();
    for(int i = 0; i < Objects.size(); i++)
    {
        QObject* Obj = Objects[i];
        QTreeWidget* W = dynamic_cast<QTreeWidget*>(Obj);
        if(W != 0)
        {
            QTreeWidgetItem *I = W->currentItem();
            SelectProblemByWidget(I);
            return;
        }
    }

}

void MainWindow::RandomizeConditions()
{
    if(Problem && Problem->CanRandomize)
    {        
        Problem->Randomize(&Rng);
        DrawConditions();
    };
}


void MainWindow::InitProblemsSolved()
{
QSettings S(QString::fromUtf8("problems.hst"), QSettings::IniFormat);
    ProblemsSolved.assign(Problems.size(), 0);
    for(size_t i = 0; i < Problems.size(); i++)
    {
        TProblem *P = Problems[i];
        QString Str = QString::fromUtf8(P->GetClassName().c_str())+QString::number(i);
        ProblemsSolved[i] = S.value(Str, QVariant(0)).toInt();
    }
}
void MainWindow::DoneProblemsSolved()
{
QSettings S(QString::fromUtf8("problems.hst"), QSettings::IniFormat);
    for(size_t i = 0; i < Problems.size(); i++)
    {
        TProblem *P = Problems[i];
        QString Str = QString::fromUtf8(P->GetClassName().c_str())+QString::number(i);
        int N = ProblemsSolved[i];
        S.setValue(Str, QVariant(N));
    }
    S.sync();
}

void MainWindow::TestFonts()
{
    TTestFonts* T = new TTestFonts(0);
    T->FormulaFont = Opt->FormulaFont;
    T->TextFont = Opt->TextFont;
    T->FormulaColor = Opt->FormulaColor;
    T->TextColor = Opt->TextColor;
    T->EditableColor = Opt->EditableColor;
    T->exec();
    delete T;
}
