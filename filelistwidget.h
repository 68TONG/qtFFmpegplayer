#ifndef FILELISTWIDGET_H
#define FILELISTWIDGET_H

#include <QObject>
#include <QListWidget>

class FileListWidget : public QListWidget
{
    Q_OBJECT
public:
    FileListWidget(QWidget *parent);

    void addWidget(QListWidgetItem *item, QWidget *widget);
};

#endif // FILELISTWIDGET_H
