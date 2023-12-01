#ifndef MYFUNCTION_H
#define MYFUNCTION_H

#include <QPoint>
#include <QSize>
#include <QDebug>
#include <QTextCodec>

QPoint getPosOffset(QPoint pos, QSize size, QSize parent_size);

bool isPos(QPoint evt_pos, QPoint pos,QSize size);

#endif // MYFUNCTION_H
