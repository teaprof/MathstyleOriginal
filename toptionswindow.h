#ifndef TOPTIONSWINDOW_H
#define TOPTIONSWINDOW_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class TOptionsWindow;
}

class TOptions {
  public:
    QFont TextFont;
    QFont FormulaFont;
    QColor TextColor;
    QColor FormulaColor;
    QColor BackgroundColor;
    QColor EditableColor;
    int Language;

    TOptions();
    ~TOptions();

    void GetDefaults();

    void Load();
    void Save();
    void SaveColor(QSettings& S, QString Key, QColor Color);
    void LoadColor(QSettings& S, QString Key, QColor& Color);
};

class TOptionsWindow : public QDialog {
    Q_OBJECT

  public:
    explicit TOptionsWindow(QWidget* parent = 0);
    ~TOptionsWindow();

    TOptions* Options;

    void SetOptions(TOptions* Options);

    void DrawSolution();

    void SetLanguage(int Language);

  private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_checkBox_clicked();

    void on_pushButton_7_clicked();

    void on_comboBox_currentIndexChanged(int index);

  private:
    Ui::TOptionsWindow* ui;
};

#endif  // TOPTIONSWINDOW_H
