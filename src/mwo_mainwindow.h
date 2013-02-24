#ifndef MWO_MAINWINDOW_H_
#define MWO_MAINWINDOW_H_

#include <QtGui/QtGui>
#include "onyx/ui/ui.h"
#include "onyx/ui/status_bar.h"
//#include "onyx/screen/screen_proxy.h"
//#include "onyx/ui/ui_utils.h"
//#include "game_widget.h"
#include "mwo_actions.h"
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
    virtual void paintEvent(QPaintEvent* pe);
    //virtual void closeEvent(QCloseEvent * event);
    //virtual bool eventFilter(QObject *obj, QEvent *event);

private Q_SLOTS:
    void showMenu();
    void OnMouseLongPress(QPoint, QSize);
    //void OnTimer();
private:
    void dither(void);
private:
    SystemActions system_actions_;
    MwoActions    mwo_actions_;
    MwoScreen     mwo_screen_;
    //GameWidget* game_widget_;
    //QTimer      timer_;
    //ui::StatusBar *status_bar_;
};


#endif
