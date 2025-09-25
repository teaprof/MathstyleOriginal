#include "tglobeview.h"

#include <QFont>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>

#include <iostream>
#include <qmath.h>
#include <random>
using namespace std;

TGlobeView::TGlobeView(QWidget* parent) : QWidget(parent) {
    setMinimumSize(100, 100);
    old_mouse_x = 0;
    old_mouse_y = 0;
    phi = 0;
    theta = 0;
    this->setMouseTracking(true);
}

TGlobeView::~TGlobeView() {}

double globe_random()  // returns random in [0...1]
{
    static mt19937 rand;
    std::uniform_real_distribution<double> u01(0, 1);
    return u01(rand);
}

void TGlobeView::AlignStrings() {
    size_t Count = Words.size();
    Coords.clear();
    for (size_t i = 0; i < Count; i++) {
        double theta = asin(globe_random() * 2.0L - 1.0L);
        double phi = 2.0L * M_PI * globe_random();
        TXYZ XYZ;
        XYZ.x = cos(theta) * cos(phi);
        XYZ.y = cos(theta) * sin(phi);
        XYZ.z = sin(theta);
        XYZ.phi = phi;
        XYZ.theta = theta;
        Coords.push_back(XYZ);
    }
    repaint();
}

void TGlobeView::ClearStrings() {
    Words.clear();
    repaint();
}

void TGlobeView::AddString(QString Word) {
    if (Words.contains(Word) == false) {
        Words.push_back(Word);
        AlignStrings();
    };
}

void TGlobeView::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter Painter(this);
    Painter.setBrush(QBrush(Qt::darkGreen));
    QSize Size = this->size();
    if (Size.width() == 0 || Size.height() == 0)
        return;
    QRect Rect(0, 0, Size.width() - 1, Size.height() - 1);
    Painter.drawRect(Rect);

    QPoint Center = Rect.center();
    Rects.fill(QRect(), Words.size());
    for (int i = 0; i < Words.size(); i++) {
        double curphi = Coords[i].phi + phi / 100.0L;
        double curtheta = Coords[i].theta + theta / 100.0L;
        Coords[i].x = cos(curtheta) * cos(curphi);
        Coords[i].y = cos(curtheta) * sin(curphi);
        Coords[i].z = sin(curtheta);

        Coords[i].gx = Center.x() + Coords[i].x * Rect.width() / 2;
        Coords[i].gy = Center.y() + Coords[i].y * Rect.height() / 2;
        Coords[i].FontHeight = (Coords[i].z * 4.0L + 12.0L);
        if (Coords[i].FontHeight <= 0)
            Coords[i].FontHeight = 1;

        QFont F = Painter.font();
        F.setPixelSize(Coords[i].FontHeight);
        Painter.setFont(F);
        QFontMetrics Metrics(Painter.font());
        Coords[i].textwidth = Metrics.horizontalAdvance(Words[i]);
        Rects[i] = Metrics.boundingRect(Words[i]);
        Rects[i].translate(Coords[i].gx - Coords[i].textwidth / 2, Coords[i].gy);
    }
    int Index = GetItemUnderMouse();
    for (int i = 0; i < Words.size(); i++) {
        QFont F = Painter.font();
        F.setPixelSize(Coords[i].FontHeight);
        F.setWeight(QFont::Bold);
        Painter.setFont(F);
        if (Index == i)
            Painter.setPen(QPen(Qt::yellow));
        else
            Painter.setPen(QPen(Qt::white));
        Painter.drawText(Coords[i].gx - Coords[i].textwidth / 2, Coords[i].gy, Words[i]);
    };
}

void TGlobeView::mouseMoveEvent(QMouseEvent* event) {
    Qt::MouseButtons Buttons = event->buttons();
    int x = event->position().x();
    int y = event->position().y();
    if (Buttons & Qt::LeftButton) {
        theta += (x - old_mouse_x);
        phi += (y - old_mouse_y);
    };
    old_mouse_x = x;
    old_mouse_y = y;
    repaint();
}

void TGlobeView::mousePressEvent(QMouseEvent* event) {
    old_mouse_x = event->position().x();
    old_mouse_y = event->position().y();
    int Index = GetItemUnderMouse();
    if (Index < Words.size())
        emit Picked(Words[Index]);
}

int TGlobeView::GetItemUnderMouse() {
    double Z = 0;
    int FoundIndex = Words.size();
    for (int i = 0; i < Words.size(); i++) {
        if (Rects[i].contains(old_mouse_x, old_mouse_y)) {
            if (FoundIndex == Words.size() || Coords[i].z > Z) {
                FoundIndex = i;
                Z = Coords[i].z;
            }
        }
    }
    return FoundIndex;
}

// void TGlobeView::mouseDoubleClickEvent(QMouseEvent *);
