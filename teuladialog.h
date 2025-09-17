#ifndef TEULADIALOG_H
#define TEULADIALOG_H

#include <QDialog>

namespace Ui {
    class TEulaDialog;
}

class TEulaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TEulaDialog(QWidget *parent = 0);
    ~TEulaDialog();

private slots:
    void on_pushButton_clicked();

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::TEulaDialog *ui;
};

#endif // TEULADIALOG_H
