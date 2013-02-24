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
        enum OpReturn{
            OK    = 0,
            ERROR_FB_OPEN_ERROR,
            ERROR_FB_READ_ERROR,
            ERROR_UNKNOWN
        };
        enum ScreenState{
            READY = 0,
            BUSY
        };
    public:
        MwoScreen();
        ~MwoScreen();

    public:
        void TestDrawSpot(void);
        void TestDrawPicture(void);
        void TestAnimation(void);

        void TestDrawSpotDither(void);
    public:
        //__u32 UpdateToDisplay(int left, int top, int width, int height, int waveform, int wait_for_complete, uint flags);
        //void  DrawPicture(int left, int top, int width, int height, int bpp, unsigned char* ptr);

        void Update(void);
        void Update(int left, int top, int width, int height, int waveform, int wait_for_complete, uint flags);
        void Copy(int left, int top, int width, int height, int bpp, unsigned char* ptr);

        void DrawCircle(int x, int y, int r);
    public:
        int xres(void);
        int yres(void);
    private:
        inline void PutDot(int x, int y, int c); 
        //inline void PutDot(int x, int y); 
    private:
        int fb_;
        int ioctl_;
        int mem_size_;
        unsigned short* mem_;
        __u32 marker_val_;

        struct fb_var_screeninfo screen_info_;
        struct fb_var_screeninfo screen_info_saved_;

        enum OpReturn    opret_;
        enum ScreenState state_;
};

#endif
