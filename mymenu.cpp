#include "mymenu.h"

MyMenu::MyMenu(QWidget *parent)
{
    setParent(parent);
}

void MyMenu::addButton(QWidget *button)
{
    button->setParent(this);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(button);
}

void MyMenu::setSize(int VH)
{
    if(layout != nullptr){
        layout->deleteLater();
        layout = nullptr;
    }
    if(VH == VERTICAL){
        layout = new QVBoxLayout();
    } else if(VH == HORIZONTAL){
        layout = new QHBoxLayout();
    }
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);
}


