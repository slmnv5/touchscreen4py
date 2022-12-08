#ifndef FRAMEBUFF_H
#define FRAMEBUFF_H

#include <stdlib.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include "lib/log.hpp"
#include "lib/utils.hpp"

// default framebuffer palette
typedef enum
{
    BLACK = 0,         /*   0,   0,   0 */
    BLUE = 1,          /*   0,   0, 172 */
    GREEN = 2,         /*   0, 172,   0 */
    CYAN = 3,          /*   0, 172, 172 */
    RED = 4,           /* 172,   0,   0 */
    PURPLE = 5,        /* 172,   0, 172 */
    ORANGE = 6,        /* 172,  84,   0 */
    LTGREY = 7,        /* 172, 172, 172 */
    GREY = 8,          /*  84,  84,  84 */
    LIGHT_BLUE = 9,    /*  84,  84, 255 */
    LIGHT_GREEN = 10,  /*  84, 255,  84 */
    LIGHT_CYAN = 11,   /*  84, 255, 255 */
    LIGHT_RED = 12,    /* 255,  84,  84 */
    LIGHT_PURPLE = 13, /* 255,  84, 255 */
    YELLOW = 14,       /* 255, 255,  84 */
    WHITE = 15         /* 255, 255, 255 */
} COLOR_INDEX_T;

static unsigned short def_r[] =
    {0, 0, 0, 0, 172, 172, 172, 168,
     84, 84, 84, 84, 255, 255, 255, 255};
static unsigned short def_g[] =
    {0, 0, 168, 168, 0, 0, 84, 168,
     84, 84, 255, 255, 84, 84, 255, 255};
static unsigned short def_b[] =
    {0, 172, 0, 168, 0, 172, 0, 168,
     84, 255, 84, 255, 84, 255, 84, 255};

class FrameBuff
{
private:
    int fdfb = -1;
    int resX, resY;
    int line_length = 0;

public:
    FrameBuff()
    {
        fdfb = open("/dev/fb1", O_RDWR);
        if (fdfb < 0)
        {
            throw std::runtime_error("Cannot open frame buffer file");
        }
        int bpp;
        getFrameBuffInfo(&resolutionX, &resolutionY, &bpp, fdfb);
        line_length = resX * bpp;
    }

    void put_pixel_16bpp(int x, int y, int r, int g, int b)
    {
        unsigned int pix_offset;
        unsigned short c;

        // calculate the pixel's byte offset inside the buffer
        pix_offset = x * 2 + y * line_length;

        // some magic to work out the color
        c = ((r / 8) << 11) + ((g / 4) << 5) + (b / 8);

        // write 'two bytes at once'
        *((unsigned short *)(fbp + pix_offset)) = c;
    }

    void drawSquare(int x, int y, int height, int width, int c)
    {
        int h = 0;
        int w = 0;
        for (h = 0; h < height; h++)
            for (w = 0; w < width; w++)
                put_pixel_16bpp(h + (x - 2), w + (y - 2), def_r[c], def_g[c], def_b[c]);
    }

private:
    void getFrameBuffInfo(int *xres, int *yres, int *bpp)
    {

        *xres = *yres = -1;
        struct fb_var_screeninfo var;

        if (ioctl(fdfb, FBIOGET_VSCREENINFO, &var) < 0)
        {
            close(fdfb);
            throw std::runtime_error("Cannot read buffer file");
        }

        LOG(LogLvl::INFO) << "Screen resolution X, Y: " << var.xres << ", " << var.yres << var.bits_per_pixel;
        *xres = var.xres;
        *yres = var.yres;
        *bpp = var.bits_per_pixel;
    }
};

#endif