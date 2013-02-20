#ifndef MWO_MAINWINDOW_H_
#define MWO_MAINWINDOW_H_

#include <QtGui/QtGui>
#include "onyx/ui/ui.h"
//#include "onyx/ui/status_bar.h"
//#include "onyx/screen/screen_proxy.h"
//#include "onyx/ui/ui_utils.h"
//#include "game_widget.h"
//#include "game_actions.h"

#include "mwo_screen.h"

using namespace ui;

class MwoMainwindow : public QWidget
{
    Q_OBJECT

public:
    MwoMainwindow(QWidget *parent = 0);
    ~MwoMainwindow();

public:
    bool start();
    bool stop();

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *ke);
    virtual void mousePressEvent(QMouseEvent*);
    //virtual void closeEvent(QCloseEvent * event);
    //virtual bool eventFilter(QObject *obj, QEvent *event);
    //virtual void paintEvent(QPaintEvent *);

private Q_SLOTS:
    void showMenu();
    void OnMouseLongPress(QPoint, QSize);
    //void OnTimer();
private:
    SystemActions system_actions_;
    MwoScreen     mwo_screen_;
    //GameWidget* game_widget_;
    //StatusBar   status_bar_;
    //QTimer      timer_;
};


#endif
