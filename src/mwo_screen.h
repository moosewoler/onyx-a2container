#ifndef _MWO_SCREEN_H_
#define _MWO_SCREEN_H_

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/mxcfb.h>

class MwoScreen
{
public:
    MwoScreen();
    ~MwoScreen();

public:
    void TestDrawSpot(void);
    void TestDrawPicture(void);
    void TestAnimation(void);

private:
    int             fb_;
    int             ioctl_;
    int             mem_size_;
    unsigned short* mem_;
    __u32           scheme_;
    __u32           pwrdown_delay_;
    __u32           marker_val_;

    struct fb_var_screeninfo screen_info_;
    struct fb_var_screeninfo screen_info_saved_;
};

#endif
