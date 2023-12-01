#include "playlists.h"
#include "ui_playlists.h"

PlayLists::PlayLists(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayLists)
{
    ui->setupUi(this);
    InitMenu();
}

PlayLists::~PlayLists()
{
    delete ui;
}

void PlayLists::InitMenu()
{
    ui->MenuBar->setSize(HORIZONTAL);
    QList<QString> list_button_name = {"浏览器", "播放列表"};
    QList<QPushButton*> list_button;
    for(int i = 0;i < list_button_name.size();i++){
        QPushButton *button = new QPushButton();
        button->setText(list_button_name[i]);
        ui->MenuBar->addButton(button);
        list_button.append(button);
    }

    ui->ListPlayList->setSize(HORIZONTAL);
    list_button_name = {"默认专辑", "此电脑"};
    for(int i = 0;i < list_button_name.size();i++){
        QPushButton *button = new QPushButton();
        button->setText(list_button_name[i]);
        ui->ListPlayList->addButton(button);
        list_button.append(button);
    }
}

QObject *PlayLists::getObject(QString obj_name)
{
    return findChild<QObject *>(obj_name);
}
