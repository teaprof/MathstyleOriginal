#ifndef TSELECTSLAUSIZE_H
#define TSELECTSLAUSIZE_H

#include <QDialog>

namespace Ui {
    class TSelectSLAUsize;
}

class TSelectSLAUsize : public QDialog
{
    Q_OBJECT

public:
    int VarsCount, EqCount;
    explicit TSelectSLAUsize(QWidget *parent = 0);
    ~TSelectSLAUsize();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::TSelectSLAUsize *ui;
};

#endif // TSELECTSLAUSIZE_H
