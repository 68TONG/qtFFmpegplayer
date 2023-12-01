#ifndef MYMENU_H
#define MYMENU_H

#include <QObject>
#include <QDialog>
#include <QPushButton>
#include <QMap>
#include <QHBoxLayout>
#include <QVBoxLayout>

#define VERTICAL 1
#define HORIZONTAL 2

class MyMenu : public QWidget
{
    Q_OBJECT

public:
    MyMenu(QWidget *parent);

    void addButton(QWidget *button);

    void setSize(int VH);

signals:
    void EmitButtonPress(QObject *obj);

private:
    QLayout *layout = nullptr;
};

#endif // MYMENU_H
