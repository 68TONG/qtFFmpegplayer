#include "progressbar.h"
#include <QMouseEvent>

ProgressBar::ProgressBar(QWidget *parent) : QSlider(parent)
{
    setMaxProgress(100);
    setCursor(Qt::PointingHandCursor);
}

void ProgressBar::InitProgressBar()
{

}

void ProgressBar::setMaxProgress(int time)
{
    setMaximum(time);
    setMinimum(0);
}

void ProgressBar::mousePressEvent(QMouseEvent *evt)
{
    is_press = true;
    double pre_cent = evt->x() * 1.0 / width();
    setPageStep(value() - (pre_cent * maximum()));
    QSlider::mousePressEvent(evt);
}
