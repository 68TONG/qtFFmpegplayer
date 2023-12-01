#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QMenu>
#include <QDialog>
#include <QListWidget>
#include <QTimer>

#include "mymenu.h"
#include "playlists.h"
#include "controlcenter.h"
#include "playmedia.h"
#include "openglwidget.h"
#include "sdlaudio.h"
#include "eventlistener.h"

#define NOT_SET 0
#define SET_WIDGET_POS 1
#define SET_SIZE_LEFT 2
#define SET_SIZE_RIGHT 3

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void mousePressEvent(QMouseEvent *evt);

    void mouseReleaseEvent(QMouseEvent *evt);

    void mouseMoveEvent(QMouseEvent *evt);

    bool eventFilter(QObject *obj, QEvent *evt);

    void InitMenu();

    void OpenFile();

    void ControlType(void *data, CONTROL_TYPE type);

    void clickedPlayList(QListWidgetItem *item);

signals:
    void EmitControlCenter(void *data, CONTROL_TYPE type);

private:
    void showFullscreenOpenGL();

    Ui::MainWindow *ui;

    PlayMedia play_media;

    MyMenu *MainMenu;

    QPoint top_left, bottom_right, evt_pos;

    QSize this_size;

    bool is_fullscreen = false;

    int sic_type;

    SDLAudio outAudio;

    double frame_rate = 0;
};
#endif // MAINWINDOW_H
