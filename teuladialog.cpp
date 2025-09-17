#include "teuladialog.h"
#include "ui_teuladialog.h"

TEulaDialog::TEulaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TEulaDialog)
{
    ui->setupUi(this);
    //ui->textBrowser->setSource(QUrl(QString::fromUtf8("qrc:/EULA/Eula/EULA_english.html")));
    ui->comboBox->setCurrentIndex(1);

}

TEulaDialog::~TEulaDialog()
{
    delete ui;
}

void TEulaDialog::on_pushButton_clicked()
{
    this->accept();
}

void TEulaDialog::on_comboBox_currentIndexChanged(int index)
{
    if(index == 0)
    {
        ui->textBrowser->setSource(QUrl(QString::fromUtf8("qrc:/EULA/Eula/EULA_russian.html")));        
    }
    else
    {
        ui->textBrowser->setSource(QUrl(QString::fromUtf8("qrc:/EULA/Eula/EULA_english_translated.html")));
    };
}
