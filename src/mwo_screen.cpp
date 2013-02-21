#include "mwo_screen.h"
#include "mwo_logger.h"


#define TFAIL -1
#define TPASS 0

#define EPDC_STR_ID		"mxc_epdc_fb"

#define WAVEFORM_MODE_INIT	0x0	/* Screen goes to white (clears) */
#define WAVEFORM_MODE_DU	0x1	/* Grey->white/grey->black */
#define WAVEFORM_MODE_GC16	0x2	/* High fidelity (flashing) */
#define WAVEFORM_MODE_GC4	0x3	/* Lower fidelity */
#define WAVEFORM_MODE_A2	0x4	/* Fast black/white animation */



MwoScreen::MwoScreen(void)
{
    logger.log("ENTER MwoScreen:MwoScreen().");

    int retval=TPASS;
	char fb_filename[] = "/dev/fb0";

    // 打开设备文件
    fb_ = open(fb_filename, O_RDWR, 0);
    if (fb_ < 0) 
    {
        printf("Unable to open %s\n", fb_filename);
        //return 1;
    }

    {
        // 判断该framebuffer是否为EPDC 
        struct  fb_fix_screeninfo screen_info_fix;
        retval = ioctl(fb_, FBIOGET_FSCREENINFO, &screen_info_fix);
        if (retval < 0)
        {
            printf("Unable to read fixed screeninfo for %s\n", fb_filename);
            //return 2;
        }
        else
        {
            if (!strcmp(EPDC_STR_ID, screen_info_fix.id)) 
            {
                printf("Opened EPDC fb device %s\n", fb_filename);
            }
        }
    }
    
    {
        // 如果有内核测试驱动，用之
        ioctl_ = open("/dev/epdc_test", O_RDWR, 0);
        if (ioctl_ >= 0)
        {
            printf("\n****Using EPDC kernel module test driver!****\n\n");
        }
        else
        {
            ioctl_ = fb_;
        }
    }

    {
        // 获取并修改设备信息
        retval = ioctl(fb_, FBIOGET_VSCREENINFO, &screen_info_);
        if (retval < 0)
        {
            //return 2;
        }
        printf("Set the background to 16-bpp\n");
        screen_info_.bits_per_pixel = 16;
        screen_info_.xres_virtual = 768;
        screen_info_.yres_virtual =1024;
        screen_info_.grayscale = 0;
        screen_info_.yoffset = 0;
        screen_info_.rotate = FB_ROTATE_CCW;      // set rotation to 180
        screen_info_.activate = FB_ACTIVATE_FORCE;
        retval = ioctl(fb_, FBIOPUT_VSCREENINFO, &screen_info_);
        if (retval < 0)
        {
            //return 2;
        }

    }

    {
        // 将fb映射到地址空间上
        mem_size_ = screen_info_.xres_virtual * screen_info_.yres_virtual * screen_info_.bits_per_pixel / 8;

        printf("screen_info.xres_virtual = %d\nscreen_info.yres_virtual = %d\nscreen_info.bits_per_pixel = %d\n",
                screen_info_.xres_virtual, screen_info_.yres_virtual, screen_info_.bits_per_pixel);

        printf("Mem-Mapping FB\n");

        mem_ = (unsigned short *)mmap(0, mem_size_ ,PROT_READ | PROT_WRITE, MAP_SHARED, fb_, 0);
        if ((int)mem_ <= 0)
        {
            printf("\nError: failed to map framebuffer device 0 to memory.\n");
            //return 2;
        }
    }

    {
        // 设置自动更新的模式
        int auto_update_mode;
        printf("Set to region update mode\n");
        auto_update_mode = AUTO_UPDATE_MODE_REGION_MODE;
        retval = ioctl(ioctl_, MXCFB_SET_AUTO_UPDATE_MODE, &auto_update_mode);
        if (retval < 0)
        {
            printf("\nError: failed to set update mode.\n");
            //return 3;
        }
    }

    {
        // 设置波形模式
        struct  mxcfb_waveform_modes wv_modes;
        printf("Set waveform modes\n");
        wv_modes.mode_init = WAVEFORM_MODE_INIT;
        wv_modes.mode_du = WAVEFORM_MODE_DU;
        wv_modes.mode_gc4 = WAVEFORM_MODE_GC4;
        wv_modes.mode_gc8 = WAVEFORM_MODE_GC16;
        wv_modes.mode_gc16 = WAVEFORM_MODE_GC16;
        wv_modes.mode_gc32 = WAVEFORM_MODE_GC16;
        retval = ioctl(ioctl_, MXCFB_SET_WAVEFORM_MODES, &wv_modes);
        if (retval < 0)
        {
            printf("\nError: failed to set waveform mode.\n");
            //return 3;
        }
    }

    {
        // 设置更新策略
        printf("Set update scheme - %d\n", scheme_);
        retval = ioctl(ioctl_, MXCFB_SET_UPDATE_SCHEME, &scheme_);
        if (retval < 0)
        {
            printf("\nError: failed to set update scheme.\n");
            //return 3;
        }
    }

    {
        // 设置休眠延迟
        printf("Set pwrdown_delay - %d\n", pwrdown_delay_);
        retval = ioctl(ioctl_, MXCFB_SET_PWRDOWN_DELAY, &pwrdown_delay_);
        if (retval < 0)
        {
            printf("\nError: failed to set power down delay.\n");
            //return 3;
        }
    }


    logger.log("LEAVE MwoScreen:MwoScreen().");
}

MwoScreen::~MwoScreen(void)
{
    logger.log("ENTER MwoScreen:~MwoScreen().");

    if ((int)mem_ > 0)
    {
        munmap(mem_, mem_size_);
    }

    // 恢复设置
    screen_info_.rotate = FB_ROTATE_CCW;
    screen_info_.bits_per_pixel = 16;
    screen_info_.grayscale = 0;

    int retval = ioctl(ioctl_, FBIOPUT_VSCREENINFO, &screen_info_);
    if (retval < 0)
    {
        printf("Back to normal failed\n");
    }

    if (fb_>0)
    {
        close(fb_);
    }

    if (fb_ != ioctl_)
    {
        close(ioctl_);
    }

    logger.log("LEAVE MwoScreen:~MwoScreen().");
}

void MwoScreen::TestDrawSpot(void)
{
    logger.log("ENTER MwoScreen:TestDrawSpot().");

	int retval = TPASS;
	int wave_mode = WAVEFORM_MODE_A2;
    int x,y;

    logger.log("INFO Blank whole screen");
    memset(mem_, 0xFF, mem_size_);
    UpdateToDisplay(0, 0, screen_info_.xres, screen_info_.yres, WAVEFORM_MODE_AUTO, TRUE, 0);
    
	logger.log("INFO test draw spot");
  
    /* 旋转为FB_ROTATE_CW 
     *      ->x
     *   +--------+
     *   |        |
     * | |        |
     * v |        |
     * y |        |
     *   |        |
     *   +--------+
     */
    for (y=0;y<screen_info_.yres;y+=20)
    {
        for (x=0;x<screen_info_.xres;x+=2)
        {
            int index;
            index = y*screen_info_.xres_virtual + x;
            mem_[index] = 0x00;   // 直接操作fb的话，操作对象是单个像素。如果转化为byte流指针的话，需要使用bpp信息
        }
        UpdateToDisplay(0, y, screen_info_.xres, 1, wave_mode, TRUE, EPDC_FLAG_FORCE_MONOCHROME);
    }

    logger.log("LEAVE MwoScreen:TestDrawSpot().");
}

void MwoScreen::TestDrawPicture(void)
{
}
void MwoScreen::TestAnimation(void)
{
}

__u32 MwoScreen::UpdateToDisplay(int left, int top, int width, int height, int waveform, int wait_for_complete, uint flags)
{
	struct mxcfb_update_data upd_data;
    __u32 upd_marker_data;
	int retval;
	int wait = wait_for_complete | flags;
	int max_retry = 10;

	upd_data.update_mode = UPDATE_MODE_PARTIAL;
	upd_data.waveform_mode = waveform;
	upd_data.update_region.left = left;
	upd_data.update_region.width = width;
	upd_data.update_region.top = top;
	upd_data.update_region.height = height;
	upd_data.temp = TEMP_USE_AMBIENT;
	upd_data.flags = flags;

	if (wait)
		/* Get unique marker value */
		upd_data.update_marker = marker_val_++;
	else
		upd_data.update_marker = 0;

	retval = ioctl(ioctl_, MXCFB_SEND_UPDATE, &upd_data);
	while (retval < 0) 
    {
		/* We have limited memory available for updates, so wait and
		 * then try again after some updates have completed */
		sleep(1);
		retval = ioctl(ioctl_, MXCFB_SEND_UPDATE, &upd_data);
		if (--max_retry <= 0) 
        {
			printf("Max retries exceeded\n");
			wait = 0;
			flags = 0;
			break;
		}
	}

	if (wait) 
    {
		upd_marker_data = upd_data.update_marker;

		/* Wait for update to complete */
		retval = ioctl(ioctl_, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &upd_marker_data);
		if (retval < 0) 
        {
			printf("Wait for update complete failed.  Error = 0x%x", retval);
			flags = 0;
		}
	}

    return upd_data.waveform_mode;
}
