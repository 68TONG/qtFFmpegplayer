#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QTimer>
#include "queue.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#include "libavformat/avformat.h"
}

class openGLWidget : public QOpenGLWidget, QOpenGLFunctions_4_5_Core
{
    Q_OBJECT
public:    
    uint64_t *main_us = NULL;
    uint64_t play_us = 0;
    double time_base = 0;
    bool is_main_stream = false;

    openGLWidget(QWidget *parent = nullptr);

    ~openGLWidget();

    void InitopenGLBuffer(Queue<AVFrame *> *video_que);

    void startOutBuffer();

    void stopOutBuffer();

    void initThis(AVStream *stream, uint64_t *us, Queue<AVFrame *> *que, bool is);

protected:

    void initializeGL() override;

    void paintGL() override;

    void resizeGL(int w, int h) override;

    uint VBO = 1, VAO = 1;

    QMatrix4x4 ResizeMatrix;

    QOpenGLShaderProgram shader_program;

    GLuint textureID[3];

    GLint textureLocation[3];

    QTimer *timer = NULL;

    Queue<AVFrame *> *que = NULL;

    unsigned int ms = 0;
signals:

    void EmitMediaPlayEnd();

    void EmitMediaChangedValue(unsigned int ms);
};

#endif // OPENGLWIDGET_H
