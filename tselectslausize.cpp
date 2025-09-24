#include "tselectslausize.h"

#include "ui_tselectslausize.h"

TSelectSLAUsize::TSelectSLAUsize(QWidget* parent) : QDialog(parent), ui(new Ui::TSelectSLAUsize) {
    ui->setupUi(this);
}

TSelectSLAUsize::~TSelectSLAUsize() {
    delete ui;
}

void TSelectSLAUsize::on_pushButton_clicked() {
    this->reject();
}

void TSelectSLAUsize::on_pushButton_2_clicked() {
    VarsCount = ui->spinBox->value();
    EqCount = ui->spinBox_2->value();
    this->accept();
}
