#include "mainwindow.h"
#include "eventlistener.h"

#include <QApplication>
#include <QObject>

#undef main
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.installEventFilter(EventListener::getEventListener());

    MainWindow w;
    w.show();
    return a.exec();
}
