#include <QtGui/QtGui>

#include "onyx/screen/screen_update_watcher.h"
#include "onyx/ui/screen_rotation_dialog.h"
#include "onyx/sys/sys.h"


#include <sys/time.h>
#include "mwo_mainwindow.h"
#include "mwo_logger.h"
#include "game_widget.h"

MwoMainwindow::MwoMainwindow(QWidget *parent)
    : QWidget(0, Qt::FramelessWindowHint)
{
    logger.log("ENTER MwoMainwindow:MwoMainwindow().");

    setWindowTitle(QCoreApplication::tr("Five In Row"));
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Base);

    game_widget_ = new GameWidget(this);

    // FIXME: set up status bar, status_bar_ will cause crash.
    //status_bar_ = new StatusBar(this);
    //status_bar_->setFocusPolicy(Qt::NoFocus);
    //connect(status_bar_, SIGNAL(menuClicked()), this, SLOT(showMenu()));

    // set up vertical layout
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    //layout->addWidget(status_bar_);
    setLayout(layout);

    //// set up popup menu to long press screen
    SysStatus & sys_status = SysStatus::instance();
    connect( &sys_status, SIGNAL( mouseLongPress(QPoint, QSize) ), game_widget_, SLOT( onMouseLongPress(QPoint, QSize) ) );
    connect( game_widget_, SIGNAL( popupMenu() ), this, SLOT( showMenu() ) );
    
    // set up screen watcher and refresh
    onyx::screen::watcher().addWatcher(this);

    logger.log("LEAVE MwoMainwindow:MwoMainwindow().");
}

MwoMainwindow::~MwoMainwindow()
{
    logger.log("ENTER MwoMainwindow:~MwoMainwindow().");
    logger.log("LEAVE MwoMainwindow:~MwoMainwindow().");
}

void MwoMainwindow::keyPressEvent(QKeyEvent *ke)
{
    logger.log("ENTER MwoMainwindow:keyPressEvent().");
    ke->accept();
    logger.log("LEAVE MwoMainwindow:keyPressEvent().");
}

void MwoMainwindow::keyReleaseEvent(QKeyEvent *ke)
{
    logger.log("ENTER MwoMainwindow:keyReleaseEvent().");
    switch (ke->key())
    {
    case ui::Device_Menu_Key:
        ke->accept();
        showMenu();
        logger.log(QString("INFO  Device_Menu_Key pressed."));
        break;
    case Qt::Key_Left:
        logger.log(QString("INFO  Key_Left pressed."));
        break;
    case Qt::Key_Right:
        logger.log(QString("INFO  Key_Right pressed."));
        break;
    case Qt::Key_PageDown:
        logger.log(QString("INFO  Key_PageDown pressed."));
        break;
    case Qt::Key_Down:
        logger.log(QString("INFO  Key_Down pressed."));
        break;
    case Qt::Key_PageUp:
        break;
    case Qt::Key_Up:
        logger.log(QString("INFO  Key_Up pressed."));
        break;
    case Qt::Key_C:
        logger.log(QString("INFO  Key_C pressed."));
        break;
    case Qt::Key_Escape:
    case Qt::Key_Home:
        ke->accept();
        stop();
        logger.log(QString("INFO  Key_Home pressed."));
    default:
        qApp->sendEvent(game_widget_, ke);
        logger.log(QString("INFO  unknown key pressed."));
        break;
    }

    logger.log("LEAVE MwoMainwindow:keyReleaseEvent().");
}

void MwoMainwindow::showMenu()
{
    PopupMenu menu(this);
    //gomoku_actions_.generateActions();
    //menu.addGroup(&gomoku_actions_);
    std::vector<int> all;
    all.push_back(ROTATE_SCREEN);
    all.push_back(MUSIC);
    all.push_back(RETURN_TO_LIBRARY);
    system_actions_.generateActions(all);
    menu.setSystemAction(&system_actions_);
    if(menu.popup() != QDialog::Accepted)
    {
        QApplication::processEvents();
        repaint();
        onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GC);
        return;
    }

    QAction * group = menu.selectedCategory();

    if(group == system_actions_.category())
    {
        SystemAction system_action = system_actions_.selected();
        switch(system_action)
        {
            case RETURN_TO_LIBRARY:
                {
                    qApp->quit();
                }
                break;
            case ROTATE_SCREEN:
                {
                    ScreenRotationDialog dialog(this);
                    dialog.popup();
                    repaint();
                    onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::GU, onyx::screen::ScreenCommand::WAIT_ALL);
                }
                break;
            case SCREEN_UPDATE_TYPE:
                {
#ifndef BUILD_FOR_FB
                    onyx::screen::instance().updateWidget(0, onyx::screen::ScreenProxy::GU);
                    onyx::screen::instance().toggleWaveform();
#endif
                }
                break;
            case MUSIC:
                {
                    // Start or show music player.
#ifndef BUILD_FOR_FB
                    onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GU);
#endif
                    sys::SysStatus::instance().requestMusicPlayer(sys::START_PLAYER);
                }
                break;
            default:
                break;
        }
    }
    //else if(group == gomoku_actions_.category())
    //{
    //    GomokuActionsType index = gomoku_actions_.selected();
    //    switch(index)
    //    {
    //    case NEW:
    //        gomoku->newGame();
    //        update();
    //        break;
    //    case ABOUT:
    //        about();
    //        repaint();
    //        onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::GU, onyx::screen::ScreenCommand::WAIT_ALL);
    //        break;
    //    default:
    //        break;
    //    }
    //}
    repaint();
    onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GC, onyx::screen::ScreenCommand::WAIT_ALL);
}



//void MwoMainwindow::mousePressEvent(QMouseEvent*me)
//{
//    logger.log("ENTER MwoMainwindow:mousePressEvent().");
//    //me->accept();
//    logger.log("LEAVE MwoMainwindow:mousePressEvent().");
//}
//
//bool MwoMainwindow::eventFilter(QObject *obj, QEvent *e)
//{
//    logger.log("ENTER MwoMainwindow:eventFilter().");
//
//    qDebug("Select event:%d", e->type());
//    if (e->type() == QEvent::MouseButtonRelease && obj->isWidgetType())
//    {
//        onyx::screen::instance().updateWidget(0, onyx::screen::ScreenProxy::GU);
//    }
//
//    logger.log("LEAVE MwoMainwindow:eventFilter().");
//    return QObject::eventFilter(obj, e);
//}
//
//void MwoMainwindow::paintEvent(QPaintEvent *e)
//{
//    logger.log("ENTER MwoMainwindow:paintEvent().");
//
//    QPainter painter(this);
//    QFont font = QApplication::font();
//    font.setPointSize(24);
//    painter.setFont(font);
//    QFontMetrics fm(font);
//    painter.drawText(QRect(0,0, width(), 200), Qt::AlignLeft, "Five in A Row by Moose W. Oler");
//    onyx::screen::instance().updateWidget(0, onyx::screen::ScreenProxy::A2, true, onyx::screen::ScreenCommand::WAIT_ALL);
//
//    logger.log("LEAVE MwoMainwindow:paintEvent().");
//}

bool MwoMainwindow::start()
{
    logger.log("ENTER MwoMainwindow:start().");

    showMaximized();
    onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GC);

    logger.log("LEAVE MwoMainwindow:start().");
    return true;
}

bool MwoMainwindow::stop()
{
    logger.log("ENTER MwoMainwindow:stop().");
    qApp->exit();
    logger.log("LEAVE MwoMainwindow:stop().");
    return true;
}


//void MwoMainwindow::OnTimer()
//{
//    logger.log("ENTER MwoMainwindow:OnTimer().");
//    logger.log("LEAVE MwoMainwindow:OnTimer().");
//}
