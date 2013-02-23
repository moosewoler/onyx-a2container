#include <QtGui/QtGui>
#include <QLabel>

#include "onyx/screen/screen_update_watcher.h"
#include "onyx/ui/screen_rotation_dialog.h"
#include "onyx/sys/sys.h"


//#include <sys/time.h>
#include "mwo_mainwindow.h"
#include "mwo_logger.h"
//#include "game_widget.h"

MwoMainwindow::MwoMainwindow(QWidget *parent)
    : QWidget(0, Qt::FramelessWindowHint)
{
    logger.log("ENTER MwoMainwindow:MwoMainwindow().");

    setWindowTitle(QCoreApplication::tr("Five In Row"));
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Base);

    //game_widget_ = new GameWidget(this);

    // FIXME: set up status bar, status_bar_ will cause crash.
    //status_bar_ = new StatusBar(this);
    //status_bar_->setFocusPolicy(Qt::NoFocus);
    //connect(status_bar_, SIGNAL(menuClicked()), this, SLOT(showMenu()));

    // set up vertical layout
    //QVBoxLayout *layout = new QVBoxLayout;
    //layout->setMargin(0);
    //layout->addWidget(status_bar_);
    //setLayout(layout);

    // set up popup menu to long press screen
    SysStatus & sys_status = SysStatus::instance();
    connect( &sys_status, SIGNAL(mouseLongPress(QPoint, QSize) ), this, SLOT(OnMouseLongPress(QPoint, QSize) ) );
    //connect( game_widget_, SIGNAL( popupMenu() ), this, SLOT( showMenu() ) );
    
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
        {
            logger.log(QString("INFO  Key_PageDown pressed."));
            drawtext();
        }
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
        //qApp->sendEvent(game_widget_, ke);
        logger.log(QString("INFO  unknown key pressed."));
        break;
    }

    logger.log("LEAVE MwoMainwindow:keyReleaseEvent().");
}

void MwoMainwindow::showMenu()
{
    logger.log("ENTER MwoMainwindow:showMenu().");
    PopupMenu menu(this);

    // 增加自定义菜单项
    //gomoku_actions_.generateActions();
    //menu.addGroup(&gomoku_actions_);

    // 增加系统菜单项
    std::vector<int> all;
    all.push_back(ROTATE_SCREEN);
    all.push_back(MUSIC);
    all.push_back(RETURN_TO_LIBRARY);
    system_actions_.generateActions(all);
    menu.setSystemAction(&system_actions_);

    // 未知
    if(menu.popup() != QDialog::Accepted)
    {
        QApplication::processEvents();
        repaint();
        onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GC);
        return;
    }

    // 选择的项目
    QAction * group = menu.selectedCategory();

    // 根据选择的项目，执行相应的动作
    if(group == system_actions_.category())
    {
        SystemAction system_action = system_actions_.selected();
        switch(system_action)
        {
            // 退出程序
            case RETURN_TO_LIBRARY:
                {
                    qApp->quit();
                }
                break;
            // 旋转屏幕
            case ROTATE_SCREEN:
                {
                    ScreenRotationDialog dialog(this);
                    dialog.popup();
                    repaint();
                    onyx::screen::watcher().enqueue(this, onyx::screen::ScreenProxy::GU, onyx::screen::ScreenCommand::WAIT_ALL);
                }
                break;
            // 屏幕更新类型，应该是开/关局部刷新的那个按钮
            case SCREEN_UPDATE_TYPE:
                {
#ifndef BUILD_FOR_FB
                    onyx::screen::instance().updateWidget(0, onyx::screen::ScreenProxy::GU);
                    onyx::screen::instance().toggleWaveform();
#endif
                }
                break;
            // 音乐
            case MUSIC:
                {
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
    // 用户自定义菜单项目的处理
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

    // 重画屏幕
    repaint();
    onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GC, onyx::screen::ScreenCommand::WAIT_ALL);
    logger.log("LEAVE MwoMainwindow:showMenu().");
}



void MwoMainwindow::mousePressEvent(QMouseEvent*me)
{
    logger.log("ENTER MwoMainwindow:mousePressEvent().");
    //mwo_screen_.TestDrawSpot();
    //mwo_screen_.TestDrawPicture();
    mwo_screen_.TestAnimation();
    me->accept();
    logger.log("LEAVE MwoMainwindow:mousePressEvent().");
}

void MwoMainwindow::OnMouseLongPress(QPoint point, QSize size)
{
    logger.log("ENTER MwoMainwindow:mouseLongPressEvent().");
    showMenu();
    logger.log("LEAVE MwoMainwindow:mouseLongPressEvent().");
}

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

