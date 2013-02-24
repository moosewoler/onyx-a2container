#include <QtGui/QtGui>

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
    setWindowTitle(QCoreApplication::tr("A2Container"));
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Base);

    //game_widget_ = new GameWidget(this);

    // set up status bar, status_bar_ will cause crash.
    //status_bar_ = new ui::StatusBar(this, ui::MENU | ui::SCREEN_REFRESH | ui::CONNECTION | ui::BATTERY | ui::CLOCK );
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
    
    // set up screen watcher and refresh
    onyx::screen::watcher().addWatcher(this);
}

MwoMainwindow::~MwoMainwindow()
{
}

void MwoMainwindow::keyPressEvent(QKeyEvent *ke)
{
    ke->accept();
}

void MwoMainwindow::keyReleaseEvent(QKeyEvent *ke)
{
    switch (ke->key())
    {
    case ui::Device_Menu_Key:
        ke->accept();
        showMenu();
        break;
    case Qt::Key_Left:
        break;
    case Qt::Key_Right:
        break;
    case Qt::Key_PageDown:
        break;
    case Qt::Key_Down:
        break;
    case Qt::Key_PageUp:
        break;
    case Qt::Key_Up:
        break;
    case Qt::Key_C:
        break;
    case Qt::Key_Escape:
    case Qt::Key_Home:
        ke->accept();
        stop();
    default:
        //qApp->sendEvent(game_widget_, ke);
        break;
    }
}

void MwoMainwindow::showMenu()
{
    PopupMenu menu(this);

    // 增加自定义菜单项
    mwo_actions_.generateActions();
    menu.addGroup(&mwo_actions_);

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
                    repaint();
                    onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GC, onyx::screen::ScreenCommand::WAIT_ALL);
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
    else if(group == mwo_actions_.category())
    {
        // 用户自定义菜单项目的处理
        MwoActionsType index = mwo_actions_.selected();
        switch(index)
        {
        case TEST1:
            mwo_screen_.TestDrawSpot();
            break;
        case TEST2:
            mwo_screen_.TestDrawPicture();
            break;
        case TEST3:
            mwo_screen_.TestAnimation();
            break;
        case DITHER:
            dither();
            break;
        default:
            break;
        }
    }

    // 重画屏幕
    repaint();
    onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GC, onyx::screen::ScreenCommand::WAIT_ALL);
}



void MwoMainwindow::mousePressEvent(QMouseEvent*me)
{
    me->accept();
}

void MwoMainwindow::OnMouseLongPress(QPoint point, QSize size)
{
    showMenu();
}

bool MwoMainwindow::start()
{
    showMaximized();
    onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GC);
    return true;
}

bool MwoMainwindow::stop()
{
    qApp->exit();
    return true;
}


//void MwoMainwindow::OnTimer()
//{
//    logger.log("ENTER MwoMainwindow:OnTimer().");
//    logger.log("LEAVE MwoMainwindow:OnTimer().");
//}



void MwoMainwindow::dither(void)
{
    mwo_screen_.TestDrawSpotDither();
}

void MwoMainwindow::paintEvent(QPaintEvent* pe)
{
}
