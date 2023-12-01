#include "openglwidget.h"

static float vertex_dp[] = {
//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
     1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
     1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
    -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
    -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
};

static unsigned int texture_dp[] = {
    0, 1, 2,
    0, 3, 2
};

//static float ResizeMatrix[] = {
//    1,0,0,0,
//    0,1,0,0,
//    0,0,1,0,
//    0,0,0,1
//};

openGLWidget::openGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this](){
       update();
    });
}

openGLWidget::~openGLWidget()
{
    if(is_main_stream){
        delete main_us;
        main_us = NULL;
    }
}

void openGLWidget::InitopenGLBuffer(Queue<AVFrame *> *video_que)
{
    que = video_que;
}

void openGLWidget::startOutBuffer()
{
    timer->start(ms);
}

void openGLWidget::stopOutBuffer()
{
    timer->stop();
}

void openGLWidget::initThis(AVStream *stream, uint64_t *us, Queue<AVFrame *> *que, bool is)
{
    this->time_base = (double)stream->time_base.den / stream->time_base.num;

    this->main_us = us;
    this->que = que;
    this->is_main_stream = is;
    play_us = 0;
    this->ms = 1000 / ((double)stream->avg_frame_rate.num / stream->avg_frame_rate.den);
}

void openGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    shader_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/openGL_ver.vertex");
    shader_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/openGL_YUV420P.fragment");

    if(shader_program.link() == false){
        qDebug() << "shader_program: link failed";
    }
    shader_program.bind();

    glGenVertexArrays(1, &VAO); //生成一个顶点数组
    glGenBuffers(1, &VBO); //生成一个顶点缓冲对象

    glBindVertexArray(VAO); //使顶点数组作为被设置对象
    glBindBuffer(GL_ARRAY_BUFFER, VBO); //使当GL_ARRAY_BUFFER作为顶点数组的数据源
    glBufferData(GL_ARRAY_BUFFER, sizeof (vertex_dp), vertex_dp, GL_STATIC_DRAW); //将数据复制到缓冲对象中

    int x = shader_program.attributeLocation("aPos");
    glVertexAttribPointer(x, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (float), (void *)0);
    glEnableVertexAttribArray(x);

    x = shader_program.attributeLocation("aColor");
    glVertexAttribPointer(x, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (float), (void *)(3 * sizeof (float)));
    glEnableVertexAttribArray(x);

    x = shader_program.attributeLocation("aTexture");
    glVertexAttribPointer(x, 2, GL_FLOAT, GL_FALSE, 8 * sizeof (float), (void *)(6 * sizeof (float)));
    glEnableVertexAttribArray(x);

    qDebug() << "glVertexAttribPointer: " << glGetError();

    glGenTextures(3, textureID);
    float rgba[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, rgba);
    textureLocation[0] = glGetUniformLocation(shader_program.programId(), "Texture_y");
    glUniform1i(textureLocation[0], 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureID[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, rgba);
    textureLocation[1] = glGetUniformLocation(shader_program.programId(), "Texture_u");
    glUniform1i(textureLocation[1], 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureID[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, rgba);
    textureLocation[2] = glGetUniformLocation(shader_program.programId(), "Texture_v");
    glUniform1i(textureLocation[2], 2);

    shader_program.setUniformValue("ResizeMatrix", ResizeMatrix);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void openGLWidget::paintGL()
{
    if(timer->isActive() == false) return;
    glBindVertexArray(VAO);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    if(que == NULL) return ;
    AVFrame *frame = NULL;

    while(true){
        frame = NULL;
        if(que->Front(frame) == false){
            if(que->getAbort()){
                emit EmitMediaPlayEnd();
                stopOutBuffer();
            }
            return ;
        }

        play_us = (frame->pts / time_base) * 1000000;
        emit EmitMediaChangedValue(play_us/1000);
        if(is_main_stream){
            *main_us = play_us;
            break;
        } else {
            int64_t us = play_us - *main_us,
                    duration = frame->duration / time_base * 1000000;
            if(us < duration * -1){
                que->Pop(frame, 10);
                av_frame_free(&frame);
                continue;
            } else if(us > duration){
                return ;
            } else break;
        }
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, frame->width, frame->height, 0, GL_RED, GL_UNSIGNED_BYTE, frame->data[0]);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureID[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, frame->width/2, frame->height/2, 0, GL_RED, GL_UNSIGNED_BYTE, frame->data[1]);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureID[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, frame->width/2, frame->height/2, 0, GL_RED, GL_UNSIGNED_BYTE, frame->data[2]);

    que->Pop(frame, 10);
    av_frame_free(&frame);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, &texture_dp);
    glFlush();
}

void openGLWidget::resizeGL(int w, int h)
{
    double w_ration = w / 16.0, h_ration = h / 9.0;
    ResizeMatrix.setToIdentity();
    if(w_ration > h_ration){
        ResizeMatrix.scale(h_ration / w_ration, 1.0f, 0);
    } else {
        ResizeMatrix.scale(1.0f, w_ration / h_ration, 0);
    }
    shader_program.setUniformValue("ResizeMatrix", ResizeMatrix);
    return QOpenGLWidget::resizeGL(w, h);
}
