#ifndef FRAMEBUFF_H
#define FRAMEBUFF_H

#include <stdlib.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "lib/log.hpp"

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
    int fdfb = -1; // file descriptor
    char *fbp = 0; // pointer to memory
    int resX, resY;
    uint screensize = 0; // for mem copy, in bytes
    uint linesize = 0;   // for finding pixels, in bytes

public:
    FrameBuff()
    {
        fdfb = open("/dev/fb1", O_RDWR);
        if (fdfb < 0)
        {
            throw std::runtime_error("Cannot open frame buffer file");
        }
        initBuff();
    }

    void put_pixel_16bpp(int x, int y, int r, int g, int b) const
    {
        int pix_offset = x * 2 + y * linesize;
        if (pix_offset < 0 || pix_offset > (int)(this->screensize - 2))
        {
            return;
        }
        unsigned short c = ((r / 8) << 11) + ((g / 4) << 5) + (b / 8);
        // write 'two bytes at once'
        *((unsigned short *)(fbp + pix_offset)) = c;
    }

    void drawSquare(int x, int y, int width, int height, int c) const
    {
        int h = 0;
        int w = 0;
        for (h = -height / 2; h < height / 2; h++)
            for (w = -width / 2; w < width / 2; w++)
                put_pixel_16bpp(h + x, w + y, def_r[c], def_g[c], def_b[c]);
    }

    const int &resx() const
    {
        return resX;
    }
    const int &resy() const
    {
        return resY;
    }

private:
    void initBuff()
    {

        struct fb_var_screeninfo var;
        if (ioctl(fdfb, FBIOGET_VSCREENINFO, &var) < 0)
        {
            close(fdfb);
            throw std::runtime_error("Cannot read buffer file var. info");
        }
        resX = var.xres;
        resY = var.yres;
        LOG(LogLvl::DEBUG) << "Screen resolution X, Y, BPP: " << resX << ", " << resY << ", " << var.bits_per_pixel;
        LOG(LogLvl::DEBUG) << "var.xoffset, var.left_margin: " << var.xoffset << ", " << var.left_margin;
        LOG(LogLvl::DEBUG) << "var.yoffset, var.upper_margin: " << var.yoffset << ", " << var.upper_margin;

        linesize = resX * (var.bits_per_pixel / 8);
        screensize = resY * linesize;
        LOG(LogLvl::DEBUG) << "Calculated memory and line sizes: " << screensize << ", " << linesize;

        fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fdfb, 0);
        int int_result = reinterpret_cast<std::intptr_t>(fbp);
        if (int_result == -1)
        {
            close(fdfb);
            throw std::runtime_error("Cannot map buffer memory");
        }
        LOG(LogLvl::DEBUG) << "Frame buffer memory mapped";
    }
};

#endif