#ifndef MWO_APPLICATION_H_
#define MWO_APPLICATION_H_

#include "onyx/base/base.h"
#include "onyx/ui/ui.h"
#include "onyx/base/dbus.h"

// 布局
#include "mwo_mainwindow.h"


class MwoApplication : public QApplication
{
    Q_OBJECT;
public:
    MwoApplication(int &argc, char **argv);
    ~MwoApplication(void);
public Q_SLOTS:
    bool            start();
    bool            stop();
private:
    MwoMainwindow   main_window_;
    NO_COPY_AND_ASSIGN(MwoApplication);
};

#endif
