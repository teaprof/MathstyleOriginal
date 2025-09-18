#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QtNetwork>
#include <QMap>
#include <QMultiMap>
#include <QHBoxLayout>
#include <QSignalMapper>

#include "Problems/tproblem.h"
//#include "Problems/GraphicsProblems.h"
#include "toptionswindow.h"

#include "tglobeview.h"
#include "tbuttonssheetview.h"

namespace Ui {
    class MainWindow;
}

typedef TProblem* PTProblem;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    PTProblem SimplifyProblem, LinearEquality, SquareEquality, PolynomialEquality4,
    PolynomialEquality8, RationalSumEquality1, RationalSumEquality2, PowerEquality2,
    LinearTrigEquality, HomogeneousTrigEquality2, HomogeneousTrigEquality3, HomogeneousTrigEquality4,
    SystemOfEquations22, SystemOfEquations33, SystemOfEquations44, SystemOfEquations55, LinearInequality,
    SquareInequality, PolynomialInequality8, SystemOfLinearInequalities, SystemOfSquareInequalities, SetOfLinearInequalities,
    SetOfSquareInequalities,
    PolynomDerivative4, RationalFunctionDerivative44, PolynomDerivative, RationalFunctionDerivative, DerivativeProblem,
    PolynomIntegralProblem, RationalIntegralProblem22, RationalIntegralProblem45, RationalIntegralProblem66,
    IntegralProblem, MaclaurinSeriesProblem3, MaclaurinSeriesProblem5,
    RationalIntegralProblemFactorized;
    QMap<QTreeWidgetItem*, TProblem*> WidgetToProblemMap;

    QSignalMapper *SignalMapper;
    QVBoxLayout *SelectedKeywordsLayout;
    vector<string> SelectedKeywords;
    void CreateProblems();
    void CreateBook();
    void CreateKeywords(vector<TProblem*> Problems);
    vector<TProblem*> GetProblemsByKeywords();    
    vector<size_t> ProblemsSolved;

    void SelectProblem(); //анализирует текущее состояние GUI и выбирает и отрисовывает проблему

    void InitProblemsSolved();
    void DoneProblemsSolved();
public:           
    TButtonsSheetView *KeywordsButtons;
    TGlobeView *KeywordsGlobeView;

    std::mt19937 rng;
    size_t DirCount; //счетчик созданных директорий с решениями
    QGraphicsScene *GViewScene;
    bool FreeDebugVersion; //true в случае бесплатной версии, используемой во время отладки
    bool Offline; //если true, то обращение к сайту не производится
    int Language; //язык    
    QNetworkAccessManager *AccessManager;
    QTreeWidgetItem *CustomSystems;
    QTreeWidgetItem *CurrentItem;

    vector<TProblem*> Problems;
    int SelectedProblem; //выбранная и отрисованная задача

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    TProblem *Problem;

    void DrawConditions();
    //void BuildTree(QTreeWidgetItem& StartItem, vector<TRectangleElement*> Elements); //2del

    QTreeWidgetItem* CreateNode(QTreeWidget* Parent, QString name);
    QTreeWidgetItem* CreateNode(QTreeWidgetItem* Parent, QString name);
    QTreeWidgetItem* CreateNode(QTreeWidget* Parent, string name);
    QTreeWidgetItem* CreateNode(QTreeWidgetItem* Parent, string name);
    QTreeWidgetItem* CreateTreeItem(QTreeWidget* I, TProblem* P);
    QTreeWidgetItem* CreateTreeItem(QTreeWidgetItem* I, TProblem* P);
    void ClearTemporaryTree();
    void DeleteProblem(size_t SelectedProblem);

    void MySetIconSize(int Size);
    TOptions* Opt;


    vector<string> Directories; //созданные директории
    vector<string> Files; //созданные файлы
    void DeleteDirs(); //удаляет директории с решениями
    string TemporaryRootAbsPath; //корневая директория для поддиректорий с решениями
    string GetDirectory(size_t DirCount); //выдает директорию вида Temp%d, где %d = DirCount


    /** loads a language by the given language shortcur (e.g. de, en, ...)
      */
    void loadLanguage(const QString& rLanguage);
    void loadLanguage(int Language);

    void SetLanguage(int Language); //устанавливает язык у всех TProblem*
private:
    QTranslator m_translator;
    Ui::MainWindow *ui;
protected:
//    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void changeEvent(QEvent* event);
    virtual void resizeEvent(QResizeEvent* E);
private slots:
    void SelectProblemByWidget(QTreeWidgetItem* item);
    void SelectProblemByIndex(size_t Index);

    void OnFormulaSelectionChanged(std::shared_ptr<TNumeric> NewSelection);
    void OnKeywordPicked(QString Keyword);

    void on_treeWidget_3_itemPressed(QTreeWidgetItem* item, int column);
    void on_treeWidget_2_itemPressed(QTreeWidgetItem* item, int column);
    void on_treeWidget_4_itemPressed(QTreeWidgetItem* item, int column);
    void on_treeWidget_5_itemPressed(QTreeWidgetItem* item, int column);
    void on_treeWidget_6_itemPressed(QTreeWidgetItem* item, int column);
    void on_FavoritesTree_itemPressed(QTreeWidgetItem* item, int column);
    void on_treeWidget_8_itemPressed(QTreeWidgetItem* item, int column);

    void on_CreateNewBtn_clicked();

    void on_DeleteBtn_clicked();

    void on_pushButton_8_clicked();  

    void on_TemporaryTree_itemPressed(QTreeWidgetItem* item, int column);

    void on_pushButton_9_clicked();

    void on_pushButton_3_clicked();

public slots:
    void FileOpen();
    void FileSave();
    void Exit();
    void Solve();
    void Options();
    void About();
    void ShowEULA();
    void MoveToFavorites();
    void RandomizeConditions();
    void KeywordClicked(QWidget* Widget);    
    void TestFonts();
};

#endif // MAINWINDOW_H
