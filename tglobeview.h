#ifndef TGLOBEVIEW_H
#define TGLOBEVIEW_H

#include <QString>
#include <QVector>
#include <QWidget>

struct TXYZ {
    double x, y, z;
    int gx, gy, textwidth;
    double phi, theta;
    int FontHeight;
};

class TGlobeView : public QWidget {
    Q_OBJECT
  public:
    double phi, theta;
    QVector<QString> Words;

    QVector<TXYZ> Coords;
    QVector<QRect> Rects;
    TGlobeView(QWidget* parent);
    ~TGlobeView();
    void AlignStrings();
    void ClearStrings();
    void AddString(QString Word);  // adds string if Words does not contain it

    int GetItemUnderMouse();

  signals:
    void Picked(QString Keyword);

  protected:
    int old_mouse_x, old_mouse_y;
    virtual void paintEvent(QPaintEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    /*    virtual bool event(QEvent *);
        virtual void mouseDoubleClickEvent(QMouseEvent *);*/
};

#endif  // TGLOBEVIEW_H
