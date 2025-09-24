#include "twaitform.h"

#include <QCloseEvent>
#include <QDesktopServices>
#include <QString>
#include <QUrl>

#include <stdio.h>
#include <string>

#include "Base/tphrases.h"
#include "ui_twaitform.h"

using namespace std;

TWaitForm::TWaitForm(QWidget* parent) : QDialog(parent), ui(new Ui::TWaitForm) {
    ui->setupUi(this);
    Timer = new QTimer(this);
    connect(Timer, SIGNAL(timeout()), this, SLOT(Update()));
    Timer->start(100);
    connect(ui->label_2, SIGNAL(linkActivated(const QString&)), this, SLOT(LinkActivated(const QString&)));
    if (MyTranslator.CheckDictionary("TWaitForm") == false) {
        MyTranslator.AddEng("Wait for %d secs");
        MyTranslator.AddRus("Ждите %d секунд");
        MyTranslator.AddDictionary("TWaitForm");
    };
}

TWaitForm::~TWaitForm() {
    delete Timer;
    delete ui;
}

void TWaitForm::Update() {
    WaitTime -= Timer->interval() / 1000.0;
    UpdateLabel();
    if (WaitTime <= 0) {
        this->close();
    }
}

void TWaitForm::SetWaitTime(double Time) {
    WaitTime = Time;
    UpdateLabel();
}

void TWaitForm::UpdateLabel() {
    char Buf[128];
    sprintf(Buf, MyTranslator.tr("Wait for %lf secs").c_str(), WaitTime);
    ui->label->setText(QString::fromUtf8(Buf));
}

void TWaitForm::closeEvent(QCloseEvent* event) {
    if (WaitTime <= 0)
        QDialog::changeEvent(event);
    else
        event->ignore();
}

void TWaitForm::reject() {}

void TWaitForm::LinkActivated(const QString& Link) {
    QDesktopServices S;
    S.openUrl(QUrl(Link));
}
