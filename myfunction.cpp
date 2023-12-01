#include "myfunction.h"

QPoint getPosOffset(QPoint pos, QSize size, QSize parent_size){
    QSize difference = parent_size - QSize(size.width() + pos.x(), size.height() + pos.y());
    return QPoint(pos.x() + qMin(difference.width(),0), pos.y() + qMin(difference.height(), 0));
}

bool isPos(QPoint evt_pos, QPoint pos,QSize size){
    QPoint max_pos = QPoint(pos.x() + size.width(), pos.y() + size.height());
    return evt_pos.x() > pos.x() && evt_pos.y() > pos.y() && evt_pos.x() < max_pos.x() && evt_pos.y() < max_pos.y();
}
