#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "myfunction.h"
#include "filelistwidget.h"

#include <QDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMouseTracking(true);
    ui->centralwidget->setMouseTracking(true);
    setWindowFlags(Qt::FramelessWindowHint);
    resize(1280, 720);

    InitMenu();

    //初始化控件
    connect(ui->opengl, &openGLWidget::EmitMediaPlayEnd, this, [this](){
        outAudio.Stop();
        ui->opengl->stopOutBuffer();
        play_media.releaseThis();
    });
    connect(ui->opengl, &openGLWidget::EmitMediaChangedValue, this, [this](unsigned int ms){
        emit EmitControlCenter((void *)ms, MEDIA_CHANGED_VALUE);
    });

    connect(ui->Controler, &ControlCenter::EmitControlInfo, this, &MainWindow::ControlType);
    connect(this, &MainWindow::EmitControlCenter, ui->Controler, &ControlCenter::ControlCenterInfo);

    FileListWidget *play_list = static_cast<FileListWidget *>(ui->PlayStackedWidget->getObject("PlayList"));
    connect(play_list, &QListWidget::itemClicked, this, &MainWindow::clickedPlayList);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *evt)
{
    if(evt->button() == Qt::RightButton){
        MainMenu->move(getPosOffset(evt->globalPos(), MainMenu->size(), QSize(1920,1080)));
        MainMenu->show();
        return ;
    }
    QPoint pos = evt->pos();
    evt_pos = evt->globalPos();
    this_size = size();
    top_left = frameGeometry().topLeft();
    bottom_right = frameGeometry().bottomRight();
    if(pos.x() < 10 || pos.y() < 10){
        sic_type = SET_SIZE_LEFT;
    } else if(pos.x() > width() - 10 || pos.y() > height() - 10){
        sic_type = SET_SIZE_RIGHT;
    } else {
        sic_type = SET_WIDGET_POS;
    }
    return QMainWindow::mousePressEvent(evt);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *evt)
{
    sic_type = NOT_SET;
    return QMainWindow::mouseReleaseEvent(evt);
}
void MainWindow::mouseMoveEvent(QMouseEvent *evt)
{
    if(evt->buttons() == Qt::LeftButton){
        if(is_fullscreen){
            sic_type = SET_WIDGET_POS;
            showFullscreenOpenGL();
        }
        if(sic_type == SET_SIZE_LEFT) {
            QPoint pos = evt_pos - evt->globalPos(),
                    min_pos = QPoint(bottom_right.x() - minimumWidth(), bottom_right.y() - minimumHeight()),
                    new_pos = top_left - pos;
            setGeometry(qMin(new_pos.x(), min_pos.x()), qMin(new_pos.y(), min_pos.y()), qMax(this_size.width() + pos.x(),1), qMax(this_size.height() + pos.y(), 1));
        } else if(sic_type == SET_SIZE_RIGHT){
            QPoint pos = evt_pos - evt->globalPos();
            setGeometry(top_left.x(), top_left.y(), qMax(this_size.width() - pos.x(), 1), qMax(this_size.height() - pos.y(), 1));
        } else if(sic_type == SET_WIDGET_POS) {
            QPoint pos = evt_pos - evt->globalPos();
            move(top_left - pos);
        }
        return ;
    }
    QPoint pos = evt->pos();
    int scope = 10;
    if(pos.x() < scope || pos.y() < scope || pos.x() > width() - scope || pos.y() > height() - scope){
        setCursor(Qt::ClosedHandCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
    if(is_fullscreen){
        if(pos.y() < ui->MenuBar->height()){
            ui->MenuBar->show();
        } else if(pos.y() > height() - ui->Controler->height()){
            ui->Controler->show();
        } else {
            if(ui->MenuBar->isVisible()) ui->MenuBar->hide();
            if(ui->Controler->isVisible()) ui->Controler->hide();
        }
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *evt)
{
    if(evt->type() == QEvent::MouseButtonPress){
        if(obj == ui->CloseButton){
            close();
        } else if(obj == ui->Fullscreen){
            showFullscreenOpenGL();
        }
    }
    return QObject::eventFilter(obj, evt);
}

void MainWindow::InitMenu()
{
    MainMenu = new MyMenu(this);
    MainMenu->setSize(VERTICAL);
    MainMenu->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    EventListener::getEventListener()->addMenuList(MainMenu);
    QList<QString> list_button_name = {"打开文件", "打开", "关闭", "播放"};
    MainMenu->resize(200, list_button_name.size() * 20);
    QList<QPushButton*> list_button;
    for(int i = 0;i < list_button_name.size();i++){
        QPushButton *button = new QPushButton();
        button->setText(list_button_name[i]);
        MainMenu->addButton(button);
        list_button.append(button);
    }
    int index = 0;
    connect(ui->Menu, &QPushButton::clicked, this, [this](){
       QPoint pos = ui->Menu->mapToGlobal(QPoint(0,0));
       MainMenu->move(pos.x(), pos.y() + ui->Menu->height());
       if(MainMenu->isHidden()) MainMenu->show();
       else MainMenu->hide();
    });
    connect(list_button[index++], &QPushButton::clicked, this, &MainWindow::OpenFile);
    index++;
    connect(list_button[index++], &QPushButton::clicked, this, [this](){
        outAudio.Stop();
        ui->opengl->stopOutBuffer();
        play_media.releaseThis();
    });

    ui->CloseButton->installEventFilter(this);
    ui->Fullscreen->installEventFilter(this);
}

void MainWindow::OpenFile()
{
    QList<QUrl> list_file_url = QFileDialog::getOpenFileUrls(this, "打开文件", QUrl(), tr("视频(*.mp4 *.mkv);;音乐(*.mp3 *.aac)"));
    for(int i = 0;i < list_file_url.size();i++){
        QFileInfo info(list_file_url[i].toString());
        FileListWidget *List = static_cast<FileListWidget *>(ui->PlayStackedWidget->getObject("PlayList"));
        QListWidgetItem *item = new QListWidgetItem();
        item->setData(0, QVariant(info.baseName()));
        item->setData(1, list_file_url[i].toLocalFile());
        List->addItem(item);
    }
}

void MainWindow::ControlType(void *data, CONTROL_TYPE type)
{
    if(type == PLAY_BUTTON){
        outAudio.Strat();
        ui->opengl->startOutBuffer();
    } else if(type == PLAYLIST_BUTTON){
        if(ui->PlayStackedWidget->isVisible()){
            ui->PlayStackedWidget->hide();
            if(is_fullscreen == false) resize(width() - ui->PlayStackedWidget->width(), height());
        } else {
            ui->PlayStackedWidget->show();
            if(is_fullscreen == false) resize(width() + ui->PlayStackedWidget->width(), height());
        }
    } else if(type == PREVIOUS_BUTTON){

    } else if(type == NEXT_BUTTON){

    } else if(type == STOP_BUTTON){
        outAudio.Stop();
        ui->opengl->stopOutBuffer();
    } else if(type == OPEN_BUTTON){

    } else if(type == MEDIA_CHANGED_VALUE){
        play_media.seekMedia((int)data * 1000);
    } else if(type == VOLUME_CHANGED_VALUE){
        outAudio.setVolume((unsigned int)data);
    }
}

void MainWindow::clickedPlayList(QListWidgetItem *item)
{
    outAudio.Stop();
    ui->opengl->stopOutBuffer();
    play_media.releaseThis();

    play_media.setPlayMedia(item->data(1).toString().toUtf8().data());

    emit EmitControlCenter((void *)(play_media.format_context->duration/1000), SET_MEDIA_VALUE);
    play_media.startDemuxtThread({});
    play_media.startDeCodeThread();

    uint64_t *us = new uint64_t(0);

    if(play_media.defaultAudioId >= 0){
        outAudio.initThis(play_media.streamHash[play_media.defaultAudioId], us, &play_media.frame_que[play_media.defaultAudioId], true);
        outAudio.InitAudio_SDL(play_media.codec_ctxs[play_media.defaultAudioId]);
        outAudio.Strat();
    }

    if(play_media.defaultVideoId >= 0){
        ui->opengl->initThis(play_media.streamHash[play_media.defaultVideoId], us, &play_media.frame_que[play_media.defaultVideoId], false);
        ui->opengl->InitopenGLBuffer(&play_media.frame_que[play_media.defaultVideoId]);
        ui->opengl->startOutBuffer();
    }
}

void MainWindow::showFullscreenOpenGL()
{
    if(is_fullscreen){
        showNormal();
        ui->MenuBar->show();
        ui->PlayStackedWidget->show();
        ui->Controler->show();
        ui->centralLayout->setContentsMargins(8, 0, 8, 8);
    } else {
        ui->MenuBar->hide();
        ui->PlayStackedWidget->hide();
        ui->Controler->hide();
        ui->centralLayout->setContentsMargins(0, 0, 0, 0);
        showFullScreen();
    }
    is_fullscreen = !is_fullscreen;
}

