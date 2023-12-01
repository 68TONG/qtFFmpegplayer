#include "eventlistener.h"
#include "myfunction.h"
#include <QDebug>

EventListener::EventListener(QObject *parent) : QObject(parent)
{

}

bool EventListener::eventFilter(QObject *obj, QEvent *evt)
{
    if(evt->type() == QEvent::MouseButtonPress){
        for(QWidget *menu : MenuList){
            QMouseEvent *mouseEvt = static_cast<QMouseEvent *>(evt);
            if(menu->isHidden() == false && isPos(mouseEvt->globalPos(), menu->frameGeometry().topLeft(), menu->size()) == false){
                menu->hide();
                return true;
            }
        }
    }
    return QObject::eventFilter(obj, evt);
}

EventListener *EventListener::getEventListener()
{
    static EventListener eventListener;
    return &eventListener;
}

void EventListener::addMenuList(QWidget *menu)
{
    MenuList.append(menu);
}
