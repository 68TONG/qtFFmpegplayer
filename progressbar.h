#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QSlider>

class ProgressBar : public QSlider
{
    Q_OBJECT
public:
    explicit ProgressBar(QWidget *parent = nullptr);

    void InitProgressBar();

    void setMaxProgress(int time);

    void mousePressEvent(QMouseEvent *evt) override;

    bool is_press = false;

signals:

};

#endif // PROGRESSBAR_H
