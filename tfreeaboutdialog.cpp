#include "tfreeaboutdialog.h"

#include "ui_tfreeaboutdialog.h"

TFreeAboutDialog::TFreeAboutDialog(QWidget* parent) : QDialog(parent), ui(new Ui::TFreeAboutDialog) {
    ui->setupUi(this);
}

TFreeAboutDialog::~TFreeAboutDialog() {
    delete ui;
}

void TFreeAboutDialog::on_pushButton_clicked() {
    accept();
}
