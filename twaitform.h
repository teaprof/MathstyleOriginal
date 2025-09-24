#ifndef TWAITFORM_H
#define TWAITFORM_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class TWaitForm;
}

class TWaitForm : public QDialog {
    Q_OBJECT

  public:
    QTimer* Timer;
    double WaitTime;
    explicit TWaitForm(QWidget* parent = 0);
    ~TWaitForm();
    void SetWaitTime(double Time);
    void UpdateLabel();

    virtual void closeEvent(QCloseEvent* event);

  private:
    Ui::TWaitForm* ui;
  public slots:
    void Update();
    virtual void reject();
    void LinkActivated(const QString& Link);
};

#endif  // TWAITFORM_H
