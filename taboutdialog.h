#ifndef TABOUTDIALOG_H
#define TABOUTDIALOG_H

#include <QDialog>
#include <QUrl>

namespace Ui {
class TAboutDialog;
}

class TAboutDialog : public QDialog {
    Q_OBJECT

  public:
    explicit TAboutDialog(QWidget* parent = 0);
    ~TAboutDialog();

  private slots:
    void on_pushButton_clicked();
    //    void anchorClicked(const QUrl&   Link);

  private:
    Ui::TAboutDialog* ui;
};

#endif  // TABOUTDIALOG_H
