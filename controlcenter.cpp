#include "controlcenter.h"
#include "ui_controlcenter.h"

ControlCenter::ControlCenter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlCenter)
{
    ui->setupUi(this);
    InitButton();
}

ControlCenter::~ControlCenter()
{
    delete ui;
}

bool ControlCenter::eventFilter(QObject *obj, QEvent *evt)
{
    if(evt->type() == QEvent::MouseButtonPress){
        CONTROL_TYPE type;
        void *data = nullptr;
        if(obj == ui->PlayButton){
            type = PLAY_BUTTON;
        } else if(obj == ui->PlayListButton){
            type = PLAYLIST_BUTTON;
        } else if(obj == ui->PreviousButton){
            type = PREVIOUS_BUTTON;
        } else if(obj == ui->NextButton){
            type = NEXT_BUTTON;
        } else if(obj == ui->StopButton){
            type = STOP_BUTTON;
        } else if(obj == ui->OpenButton){
            type = OPEN_BUTTON;
        }
        emit EmitControlInfo(data, type);
        return true;
    }
    return QObject::eventFilter(obj, evt);
}

void ControlCenter::InitButton()
{
    ui->PlayButton->installEventFilter(this);
    ui->PlayListButton->installEventFilter(this);
    ui->PreviousButton->installEventFilter(this);
    ui->NextButton->installEventFilter(this);
    ui->StopButton->installEventFilter(this);
    ui->OpenButton->installEventFilter(this);

    connect(ui->MediaSlider, &ProgressBar::valueChanged, this, &ControlCenter::ValueChanged_Slider);
}

void ControlCenter::ControlCenterInfo(void *data, CONTROL_TYPE type)
{
    if(type == MEDIA_CHANGED_VALUE){
        ui->MediaSlider->setValue((unsigned int)data);
    } else if(type == SET_MEDIA_VALUE){
        ui->MediaSlider->setMaxProgress((int)data);
        ui->MediaSlider->setValue(0);
    }
}

void ControlCenter::ValueChanged_Slider(int duration)
{
    if(ui->MediaSlider->is_press){
        emit EmitControlInfo((void *)duration, MEDIA_CHANGED_VALUE);
        ui->MediaSlider->is_press = false;
    }
}

void ControlCenter::on_VolumeSlider_valueChanged(int value)
{
    emit EmitControlInfo((void *)value, VOLUME_CHANGED_VALUE);
}
