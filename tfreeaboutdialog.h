#ifndef TFREEABOUTDIALOG_H
#define TFREEABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class TFreeAboutDialog;
}

class TFreeAboutDialog : public QDialog {
    Q_OBJECT

  public:
    explicit TFreeAboutDialog(QWidget* parent = 0);
    ~TFreeAboutDialog();

  private slots:
    void on_pushButton_clicked();

  private:
    Ui::TFreeAboutDialog* ui;
};

#endif  // TFREEABOUTDIALOG_H
