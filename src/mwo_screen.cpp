#include <directfb.h>
#include "mwo_screen.h"
#include "mwo_logger.h"

#include "a2test.c"

#define EPDC_STR_ID		"mxc_epdc_fb"

#define WAVEFORM_MODE_INIT	0x0	/* Screen goes to white (clears) */
#define WAVEFORM_MODE_DU	0x1	/* Grey->white/grey->black */
#define WAVEFORM_MODE_GC16	0x2	/* High fidelity (flashing) */
#define WAVEFORM_MODE_GC4	0x3	/* Lower fidelity */
#define WAVEFORM_MODE_A2	0x4	/* Fast black/white animation */


MwoScreen::MwoScreen(void)
{
    logger.log("ENTER MwoScreen:MwoScreen().");

    int retval;
	char fb_filename[] = "/dev/fb0";

    opret_ = MwoScreen::OK;

    // 打开设备文件
    fb_ = open(fb_filename, O_RDWR, 0);
    if (fb_ < 0) 
    {
        logger.log("ERROR Unable to open" + QString(fb_filename));
        opret_ = MwoScreen::ERROR_FB_OPEN_ERROR;
    }

    {
        // 判断该framebuffer是否为EPDC 
        struct  fb_fix_screeninfo screen_info_fix;
        retval = ioctl(fb_, FBIOGET_FSCREENINFO, &screen_info_fix);
        if (retval < 0)
        {
            logger.log("ERROR Unable to read fixed screeninfo for" + QString(fb_filename));
            opret_ = MwoScreen::ERROR_FB_READ_ERROR;
        }
        else
        {
            if (!strcmp(EPDC_STR_ID, screen_info_fix.id)) 
            {
                logger.log("ERROR Opened EPDC fb device" + QString(fb_filename));
            }
        }
    }
    
    {
        // 如果有内核测试驱动，用之
        ioctl_ = open("/dev/epdc_test", O_RDWR, 0);
        if (ioctl_ >= 0)
        {
            logger.log("\n****Using EPDC kernel module test driver!****\n\n");
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
            opret_ = MwoScreen::ERROR_FB_READ_ERROR;
        }
        //printf("Set the background to 16-bpp\n");
        //screen_info_.bits_per_pixel = 16;
        //screen_info_.xres_virtual = 768;
        //screen_info_.yres_virtual =1024;
        //screen_info_.grayscale = 0;
        //screen_info_.yoffset = 0;
        //screen_info_.rotate = FB_ROTATE_CCW;      // set rotation to 180
        //screen_info_.activate = FB_ACTIVATE_FORCE;
        //retval = ioctl(fb_, FBIOPUT_VSCREENINFO, &screen_info_);
        //if (retval < 0)
        //{
        //    //return 2;
        //}

    }

    {
        // 将fb映射到地址空间上
        mem_size_ = screen_info_.xres_virtual * screen_info_.yres_virtual * screen_info_.bits_per_pixel / 8;
        //logger.log("INFO "+
        //        "screen_info.xres_virtual = " + QString::number(screen_info_.xres_virtual) + "\n" +
        //           "screen_info.yres_virtual = " + QString::number(screen_info_.yres_virtual)+ "\n" +
        //           "screen_info.bits_per_pixel = "+QString::number(screen_info_.bits_per_pixel));
        logger.log("INFO Mem-Mapping FB");

        mem_ = (unsigned short *)mmap(0, mem_size_ ,PROT_READ | PROT_WRITE, MAP_SHARED, fb_, 0);
        if ((int)mem_ <= 0)
        {
            logger.log("ERROR failed to map framebuffer device 0 to memory.");
            opret_ = MwoScreen::ERROR_FB_READ_ERROR;
        }
    }

    {
        //// 设置自动更新的模式
        //int auto_update_mode;
        //printf("Set to region update mode\n");
        //auto_update_mode = AUTO_UPDATE_MODE_REGION_MODE;
        //retval = ioctl(ioctl_, MXCFB_SET_AUTO_UPDATE_MODE, &auto_update_mode);
        //if (retval < 0)
        //{
        //    printf("\nError: failed to set update mode.\n");
        //    //return 3;
        //}
    }

    {
        // 设置波形模式
        //struct  mxcfb_waveform_modes wv_modes;
        //printf("Set waveform modes\n");
        //wv_modes.mode_init = WAVEFORM_MODE_INIT;
        //wv_modes.mode_du = WAVEFORM_MODE_DU;
        //wv_modes.mode_gc4 = WAVEFORM_MODE_GC4;
        //wv_modes.mode_gc8 = WAVEFORM_MODE_GC16;
        //wv_modes.mode_gc16 = WAVEFORM_MODE_GC16;
        //wv_modes.mode_gc32 = WAVEFORM_MODE_GC16;
        //retval = ioctl(ioctl_, MXCFB_SET_WAVEFORM_MODES, &wv_modes);
        //if (retval < 0)
        //{
        //    printf("\nError: failed to set waveform mode.\n");
        //    //return 3;
        //}
    }

    {
        // 设置更新策略
        //printf("Set update scheme - %d\n", scheme_);
        //retval = ioctl(ioctl_, MXCFB_SET_UPDATE_SCHEME, &scheme_);
        //if (retval < 0)
        //{
        //    printf("\nError: failed to set update scheme.\n");
        //    //return 3;
        //}
    }

    {
        // 设置休眠延迟
        //printf("Set pwrdown_delay - %d\n", pwrdown_delay_);
        //retval = ioctl(ioctl_, MXCFB_SET_PWRDOWN_DELAY, &pwrdown_delay_);
        //if (retval < 0)
        //{
        //    printf("\nError: failed to set power down delay.\n");
        //    //return 3;
        //}
    }

    // 设置自身的属性
    state_ = MwoScreen::READY;
    opret_ = MwoScreen::OK;

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
    //screen_info_.rotate = FB_ROTATE_CCW;
    //screen_info_.bits_per_pixel = 16;
    //screen_info_.grayscale = 0;
    //int retval = ioctl(ioctl_, FBIOPUT_VSCREENINFO, &screen_info_);
    //if (retval < 0)
    //{
    //    printf("Back to normal failed\n");
    //}

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
    if (state_ == MwoScreen::READY)
    {

        logger.log("ENTER MwoScreen:TestDrawSpot().");
        int retval;
        int wave_mode = WAVEFORM_MODE_A2;
        int x,y;

        state_ = MwoScreen::BUSY;

        logger.log("INFO Blank whole screen");
        memset(mem_, 0xFF, mem_size_);
        Update(0, 0, screen_info_.xres, screen_info_.yres, WAVEFORM_MODE_AUTO, TRUE, 0);

        logger.log("INFO test draw spot");

        /* MwoScreen定义的坐标系如下所示：
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
                int tx=-x+screen_info_.xres;
                int ty=-y+screen_info_.yres;
                int index;
                index = ty*screen_info_.xres_virtual + tx;
                mem_[index] = 0x00;   // 直接操作fb的话，操作对象是单个像素。如果转化为byte流指针的话，需要使用bpp信息
            }
            // FIXME: 变换不适用于边界值
            //UpdateToDisplay(0, y, screen_info_.xres, 1, wave_mode, TRUE, EPDC_FLAG_FORCE_MONOCHROME);
            //UpdateToDisplay(0, 0, screen_info_.xres, screen_info_.yres, wave_mode, TRUE, EPDC_FLAG_FORCE_MONOCHROME);
            Update();
        }
        state_ = MwoScreen::READY;
        logger.log("LEAVE MwoScreen:TestDrawSpot().");
    }
}

/*******************************************************************************
 *
 ******************************************************************************/
static IDirectFB *dfb = NULL;
static IDirectFBSurface *primary = NULL;
static int screen_width  = 0;
static int screen_height = 0;

#define DFBCHECK(x...)                                          \
{                                                               \
    DFBResult err = x;                                          \
                                                                \
    if (err != DFB_OK)                                          \
    {                                                           \
        fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ );  \
        DirectFBErrorFatal( #x, err );                          \
    }                                                           \
}
void MwoScreen::TestDirectFB(void)
{
    if (state_ == MwoScreen::READY)
    {

        logger.log("ENTER MwoScreen:TestDrawSpot().");
        int retval;
        int wave_mode = WAVEFORM_MODE_A2;
        int x,y;

        state_ = MwoScreen::BUSY;

        DFBSurfaceDescription dsc;

        DFBCHECK (DirectFBInit (NULL, NULL));
        DFBCHECK( DirectFBSetOption( "no-vt", NULL ) );                // 由于onyx没有tty0，所以姑且禁用vt
        DFBCHECK( DirectFBSetOption( "disable-module", "tslib" ) );    // 禁用tslib
        DFBCHECK (DirectFBCreate (&dfb));
        DFBCHECK (dfb->SetCooperativeLevel (dfb, DFSCL_FULLSCREEN));

        dsc.flags = DSDESC_CAPS;
        dsc.caps  = (DFBSurfaceCapabilities)(DSCAPS_PRIMARY | DSCAPS_FLIPPING);

        DFBCHECK (dfb->CreateSurface( dfb, &dsc, &primary ));
        DFBCHECK (primary->GetSize (primary, &screen_width, &screen_height));
        DFBCHECK (primary->FillRectangle (primary, 0, 0, screen_width, screen_height));
        DFBCHECK (primary->SetColor (primary, 0x80, 0x80, 0xff, 0xff));

        for (int i=0;i<10;i++)
        {
            DFBCHECK (primary->DrawLine (primary, 0, i*10+1, screen_width - 1, i*10+1));
        }
        DFBCHECK (primary->Flip (primary, NULL, (DFBSurfaceFlipFlags)(0)));
        Update();

        primary->Release( primary );
        dfb->Release( dfb );
        state_ = MwoScreen::READY;
        logger.log("LEAVE MwoScreen:TestDrawSpot().");
    }
}

void MwoScreen::TestDrawPicture(void)
{
	int wave_mode = WAVEFORM_MODE_A2;

    if ( state_ == MwoScreen::READY)
    {
        state_ = MwoScreen::BUSY;
        logger.log("INFO  Blank whole screen");
        memset(mem_, 0xFF, mem_size_);
        Update(0, 0, screen_info_.xres, screen_info_.yres, WAVEFORM_MODE_AUTO, TRUE, 0);

        logger.log("INFO  test draw pic");
        {
            int x=100;
            int y=200;
            Copy(x,y, gimp_image_001.width, gimp_image_001.height, 2, (unsigned char*)(gimp_image_001.pixel_data));
            //UpdateToDisplay(0, 0, screen_info_.xres, screen_info_.yres, wave_mode, TRUE, EPDC_FLAG_FORCE_MONOCHROME);
            Update();
        }
        state_ = MwoScreen::READY;
    }

}
void MwoScreen::TestAnimation(void)
{
    int wave_mode = WAVEFORM_MODE_A2;

    if ( state_ == MwoScreen::READY)
    {
        state_ = MwoScreen::BUSY;
        memset(mem_, 0xFF, mem_size_);
        Update(0, 0, screen_info_.xres, screen_info_.yres, WAVEFORM_MODE_AUTO, TRUE, 0);

        int i;
        int x=100;
        int y=200;
        int frame=0;
        static const TImage* images[] = {
            &gimp_image_001,&gimp_image_002,&gimp_image_003,&gimp_image_004,
            &gimp_image_005,&gimp_image_006,&gimp_image_007,&gimp_image_008,
            &gimp_image_009,&gimp_image_010,&gimp_image_011,&gimp_image_012,
            &gimp_image_013,&gimp_image_014,&gimp_image_015,&gimp_image_016
        };

        for (i=0;i<5;i++)
        {
            for (frame=0;frame<16;frame++)
            {
                Copy(x,y, images[frame]->width, images[frame]->height, 2, (unsigned char*)(images[frame]->pixel_data));
                Copy(x+300,y, images[frame]->width, images[frame]->height, 2, (unsigned char*)(images[frame]->pixel_data));
                Copy(x,y+300, images[frame]->width, images[frame]->height, 2, (unsigned char*)(images[frame]->pixel_data));
                Copy(x+300,y+300, images[frame]->width, images[frame]->height, 2, (unsigned char*)(images[frame]->pixel_data));
                //UpdateToDisplay(x, y, images[frame]->width, images[frame]->height, wave_mode, TRUE, EPDC_FLAG_FORCE_MONOCHROME);
                //UpdateToDisplay(0, 0, screen_info_.xres, screen_info_.yres, wave_mode, TRUE, EPDC_FLAG_FORCE_MONOCHROME);
                Update();
            }
        }
        state_ = MwoScreen::READY;
    }
}

void MwoScreen::TestDrawSpotDither(void)
{
    logger.log("ENTER MwoScreen:TestDrawSpotDither().");

    if (state_ == MwoScreen::READY)
    {
        int retval;
        int wave_mode = WAVEFORM_MODE_A2;
        int x,y;

        state_ = MwoScreen::BUSY;

        memset(mem_, 0xFF, mem_size_);
        Update(0, 0, screen_info_.xres, screen_info_.yres, WAVEFORM_MODE_AUTO, TRUE, 0);

        for (int r=10;r<200;r++)
        {
            DrawCircle(xres()/2,yres()/2,r); 
            Update();
        }

        state_ = MwoScreen::READY;
    }
    logger.log("LEAVE MwoScreen:TestDrawSpot().");
}

void MwoScreen::Copy(int left, int top, int width, int height, int bpp, unsigned char* ptr)
{
	unsigned char* fbp = (unsigned char*)mem_;
    int i,j,k;
    int index;

    // TODO: add rotation support
    // TODO: use memcpy to optimize(maybe) the routine
    left= -(left+width)+screen_info_.xres;
    top = -(top+height)+screen_info_.yres;

    ptr = ptr + height * width * bpp;

	for (i = top; i < top+height; i++)
    {
		for (j = left; j < left+width;j++)
        {
            index= i*screen_info_.xres_virtual+j;
            for (k=0;k<bpp;k++)
            {
                fbp[index*bpp+k] = *ptr;
                ptr--;
            }
        }
    }
}

// Midpoint Circle Algorithm
// see: http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
void MwoScreen::DrawCircle(int cx,int cy,int r) 
{
    int e=-r;
    int x=r;
    int y=0;
    static int c=64;

    while (x>y) 
    {
        PutDot(cx+y,cy-x,c); 
        PutDot(cx+x,cy-y,c);
        PutDot(cx+x,cy+y,c); 
        PutDot(cx+y,cy+x,c);
        PutDot(cx-y,cy+x,c); 
        PutDot(cx-x,cy+y,c);
        PutDot(cx-x,cy-y,c); 
        PutDot(cx-y,cy-x,c);
        e+=y; 
        y++; 
        e+=y;
        if (e>0) 
        { 
            e-=x; 
            x-=1; 
            e-=x; 
        }
    }

    c--;
    if (c<0)
    {
        c = 64;
    }
}

//========================================
// putdot_dither - draw pixel using ordered dither
// x,y: screen coordinates, c: color(0-64).
// (This works on all eink kindle models.)
//----------------------------------------
void inline MwoScreen::PutDot(int x,int y,int c) 
{
    static int dither_map_64[64] = { 
        1, 33,9, 41,3, 35,11,43,
        49,17,57,25,51,19,59,27,
        13,45,5, 37,15,47,7, 39,
        61,29,53,21,63,31,55,23,
        4, 36,12,44,2, 34,10,42,
        52,20,60,28,50,18,58,26,
        16,48,8, 40,14,46,6, 38,
        64,32,56,24,62,30,54,22 }; // 64 threshold dither table, see http://en.wikipedia.org/wiki/Ordered_dithering

//    mem_[pb*x/8+fs*y]=((128&(c-dither_map_64[(7&x)+8*(7&y)]))/128*(blk&(240*(1&~x)|
//                       15*(1&x)|fb0[pb*x/8+fs*y])))|
//                       ((128&(dither_map_64[(7&x)+8*(7&y)]-c))/128*wht|
//                       (blk&((240*(1&x)|15*(1&~x))&fb0[pb*x/8+fs*y]))); // geekmaster formula 42

    if (c > dither_map_64[x&7, y&7])
    {
        register int tx=-x+screen_info_.xres;
        register int ty=-y+screen_info_.yres;
        register int index;
        index = ty*screen_info_.xres_virtual + tx;
        mem_[index] = 0x0;
    }
    else
    {
        register int tx=-x+screen_info_.xres;
        register int ty=-y+screen_info_.yres;
        register int index;
        index = ty*screen_info_.xres_virtual + tx;
        mem_[index] = 0xFFFF;
    }
}


//__u32 MwoScreen::UpdateToDisplay(int left, int top, int width, int height, int waveform, int wait_for_complete, uint flags)
//{
//    // 见2013-02-22笔记1号
//	struct mxcfb_update_data upd_data;
//    __u32 upd_marker_data;
//	int retval;
//	int wait = wait_for_complete | flags;
//	int max_retry = 10;
//
//    // FIXME: 变换不适用于边界值
//    //int tleft= -(left+width)+screen_info_.xres;
//    //int ttop = -(top+height-1)+screen_info_.yres;
//    int tleft= left;
//    int ttop = top;
//
//	upd_data.update_mode = UPDATE_MODE_PARTIAL;
//	upd_data.waveform_mode = waveform;
//	upd_data.update_region.left = tleft;
//	upd_data.update_region.width = width;
//	upd_data.update_region.top = ttop;
//	upd_data.update_region.height = height;
//	upd_data.temp = TEMP_USE_AMBIENT;
//	upd_data.flags = flags;
//
//	if (wait)
//		/* Get unique marker value */
//		upd_data.update_marker = marker_val_++;
//	else
//		upd_data.update_marker = 0;
//
//	retval = ioctl(ioctl_, MXCFB_SEND_UPDATE, &upd_data);
//	while (retval < 0) 
//    {
//		/* We have limited memory available for updates, so wait and
//		 * then try again after some updates have completed */
//		sleep(1);
//		retval = ioctl(ioctl_, MXCFB_SEND_UPDATE, &upd_data);
//		if (--max_retry <= 0) 
//        {
//			printf("Max retries exceeded\n");
//			wait = 0;
//			flags = 0;
//			break;
//		}
//	}
//
//	if (wait) 
//    {
//		upd_marker_data = upd_data.update_marker;
//
//		/* Wait for update to complete */
//		retval = ioctl(ioctl_, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &upd_marker_data);
//		if (retval < 0) 
//        {
//			printf("Wait for update complete failed.  Error = 0x%x", retval);
//			flags = 0;
//		}
//	}
//
//    return upd_data.waveform_mode;
//}

//void MwoScreen::DrawPicture(int left, int top, int width, int height, int bpp, unsigned char* ptr)
//{
//    logger.log("ENTER MwoScreen:DrawPicture().");
//
//	unsigned char* fbp = (unsigned char*)mem_;
//    int i,j,k;
//    int index;
//
//    logger.log("INFO  left=" + QString::number(left) + " right=" + QString::number(top) + " width="+ QString::number(width) + " height=" + QString::number(height));
//
//    // TODO: add rotation support
//    // TODO: use memcpy to optimize(maybe) the routine
//    left= -(left+width)+screen_info_.xres;
//    top = -(top+height)+screen_info_.yres;
//
//    logger.log("INFO  rotate left=" + QString::number(left) + " right=" + QString::number(top) + " width="+ QString::number(width) + " height=" + QString::number(height));
//
//    ptr = ptr + height * width * bpp;
//
//	for (i = top; i < top+height; i++)
//    {
//		for (j = left; j < left+width;j++)
//        {
//            index= i*screen_info_.xres_virtual+j;
//            for (k=0;k<bpp;k++)
//            {
//                fbp[index*bpp+k] = *ptr;
//                ptr--;
//            }
//        }
//    }
//    logger.log("LEAVE MwoScreen:DrawPicture().");
//}


void MwoScreen::Update(void)
{
    Update(0,0,screen_info_.xres, screen_info_.yres, WAVEFORM_MODE_A2, TRUE, EPDC_FLAG_FORCE_MONOCHROME);
}

void MwoScreen::Update(int left, int top, int width, int height, int waveform, int wait_for_complete, uint flags)
{
    // 见2013-02-22笔记1号
	struct mxcfb_update_data upd_data;
    __u32   upd_marker_data;
	int     retval;
	int     wait = wait_for_complete | flags;
	int     max_retry = 10;

    // FIXME: 计算出合适的边界值
    //int tleft= -(left+width)+screen_info_.xres;
    //int ttop = -(top+height-1)+screen_info_.yres;
    int tleft= left;
    int ttop = top;

	upd_data.update_mode = UPDATE_MODE_PARTIAL;
	upd_data.waveform_mode = waveform;
	upd_data.update_region.left = tleft;
	upd_data.update_region.width = width;
	upd_data.update_region.top = ttop;
	upd_data.update_region.height = height;
	upd_data.temp = TEMP_USE_AMBIENT;
	upd_data.flags = flags;

	if (wait)
    {
		/* Get unique marker value */
		upd_data.update_marker = marker_val_++;
    }
	else
    {
		upd_data.update_marker = 0;
    }

	retval = ioctl(ioctl_, MXCFB_SEND_UPDATE, &upd_data);
	while (retval < 0) 
    {
		/* We have limited memory available for updates, so wait and
		 * then try again after some updates have completed */
		sleep(1);
		retval = ioctl(ioctl_, MXCFB_SEND_UPDATE, &upd_data);
		if (--max_retry <= 0) 
        {
			logger.log("ERROR Max retries exceeded");
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
			logger.log("ERROR Wait for update complete failed.  Error = " + QString::number(retval));
			flags = 0;
		}
	}
}


int MwoScreen::xres(void)
{
    return screen_info_.xres;
}
int MwoScreen::yres(void)
{
    return screen_info_.yres;
}
