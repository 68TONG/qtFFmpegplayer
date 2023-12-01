#ifndef CONTROLCENTER_H
#define CONTROLCENTER_H

#include <QWidget>
#include <QEvent>

enum CONTROL_TYPE{
    PLAY_BUTTON ,
    PLAYLIST_BUTTON ,
    PREVIOUS_BUTTON ,
    NEXT_BUTTON ,
    STOP_BUTTON ,
    OPEN_BUTTON ,
    MEDIA_CHANGED_VALUE ,
    VOLUME_CHANGED_VALUE ,

    SET_MEDIA_VALUE ,
};

namespace Ui {
class ControlCenter;
}

class ControlCenter : public QWidget
{
    Q_OBJECT

public:
    explicit ControlCenter(QWidget *parent = nullptr);
    ~ControlCenter();

    bool eventFilter(QObject *obj, QEvent *evt);

    void InitButton();

    void ControlCenterInfo(void *data, CONTROL_TYPE type);

    void ValueChanged_Slider(int duration);

signals:
    void EmitControlInfo(void *data, CONTROL_TYPE type);

private slots:
    void on_VolumeSlider_valueChanged(int value);

private:
    Ui::ControlCenter *ui;
};

#endif // CONTROLCENTER_H
