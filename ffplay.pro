QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/bin/SDL2/include \
               $$PWD/bin/ffmpeg_32/include

LIBS += -L$$PWD/bin/ffmpeg_32/lib -lavcodec \
        -L$$PWD/bin/ffmpeg_32/lib -lavdevice \
        -L$$PWD/bin/ffmpeg_32/lib -lavfilter \
        -L$$PWD/bin/ffmpeg_32/lib -lavformat \
        -L$$PWD/bin/ffmpeg_32/lib -lavutil \
        -L$$PWD/bin/ffmpeg_32/lib -lswresample \
        -L$$PWD/bin/ffmpeg_32/lib -lswscale

LIBS += -L$$PWD/bin/SDL2/lib/x86 -lSDL2

SOURCES += \
    controlcenter.cpp \
    eventlistener.cpp \
    filelistwidget.cpp \
    itemwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    myfunction.cpp \
    mymenu.cpp \
    openglwidget.cpp \
    playlists.cpp \
    playmedia.cpp \
    progressbar.cpp \
    sdlaudio.cpp

HEADERS += \
    controlcenter.h \
    eventlistener.h \
    filelistwidget.h \
    itemwidget.h \
    mainwindow.h \
    myfunction.h \
    mymenu.h \
    openglwidget.h \
    playlists.h \
    playmedia.h \
    progressbar.h \
    queue.h \
    sdlaudio.h

FORMS += \
    controlcenter.ui \
    mainwindow.ui \
    playlists.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    shaders.qrc \
    src.qrc
