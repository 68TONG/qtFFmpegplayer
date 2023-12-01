#ifndef PLAYLISTS_H
#define PLAYLISTS_H

#include <QWidget>

namespace Ui {
class PlayLists;
}

class PlayLists : public QWidget
{
    Q_OBJECT

public:
    explicit PlayLists(QWidget *parent = nullptr);
    ~PlayLists();

    void InitMenu();

    QObject *getObject(QString obj_name);

private:
    Ui::PlayLists *ui;
};

#endif // PLAYLISTS_H
