#include "taboutdialog.h"

#include <QDesktopServices>

#include "ui_taboutdialog.h"

TAboutDialog::TAboutDialog(QWidget* parent) : QDialog(parent), ui(new Ui::TAboutDialog) {
    ui->setupUi(this);
    //    connect(ui->textBrowser, SIGNAL(anchorClicked(const QUrl&)), this, SLOT(anchorClicked(const QUrl&)));
}

TAboutDialog::~TAboutDialog() {
    delete ui;
}

void TAboutDialog::on_pushButton_clicked() {
    accept();
}

/*void TAboutDialog::anchorClicked(const QUrl& Link)
{
QDesktopServices S;
   S.openUrl(Link);
};*/
