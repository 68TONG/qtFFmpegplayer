#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H

#include <QObject>
#include <QEvent>
#include <QWidget>
#include <QList>

class EventListener : public QObject
{
    Q_OBJECT
public:
    explicit EventListener(QObject *parent = nullptr);

    bool eventFilter(QObject *obj, QEvent *evt) override;

    static EventListener *getEventListener();

    void addMenuList(QWidget *obj);

private:
    QList<QWidget *> MenuList;

signals:

};

#endif // EVENTLISTENER_H
