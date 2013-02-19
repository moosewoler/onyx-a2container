#include "onyx/sys/sys_status.h"
#include "mwo_application.h"        // 应用
#include "mwo_logger.h"             // 日志

int main(int argc, char * argv[])
{
    logger.log("ENTER main()");

    sys::SysStatus::instance().setSystemBusy(false);    // notify system is not busy via D-BUS. see onyx/sys/service.h
    
    MwoApplication myapp(argc, argv);
    myapp.start();

    logger.log("ENTER myapp.exec()");
    int ret = myapp.exec();                             // main event loop
    logger.log("LEAVE myapp.exec()");

    logger.log("LEAVE main()");
    return ret;
}
