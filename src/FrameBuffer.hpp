#ifndef FRAMEBUFF_H
#define FRAMEBUFF_H

// #include <stdlib.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "pch.hpp"
#include "fbfonts.hpp"
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

class FrameBuffer
{
private:
    int mFdFb = -1;      // file descriptor
    char *mFbPtr = 0;    // pointer to frame buffer memory
    uint mScrSize = 0;   // screen memory size in bytes
    uint mPixelSize = 0; // pixel size in bytes

public:
    FbPixelFont &mFont = font_16x32; // font
    int mPixelsX, mPixelsY;          // screen resolution

    FrameBuffer(int fbidx = 1)
    {
        // fb1 connected to LCD screen, dont know how to change
        std::string fbname = "/dev/fb" + std::to_string(fbidx);
        mFdFb = open(fbname.c_str(), O_RDWR);
        if (mFdFb < 0)
        {
            throw std::runtime_error("Cannot open frame buffer file: " + fbname);
        }
        struct fb_var_screeninfo var;
        if (ioctl(mFdFb, FBIOGET_VSCREENINFO, &var) < 0)
        {
            close(mFdFb);
            throw std::runtime_error("Cannot read frame buffer file info: " + fbname);
        }
        mPixelsX = var.xres;
        mPixelsY = var.yres;
        LOG(LogLvl::DEBUG) << "Screen resolution X, Y, BPP: " << mPixelsX << ", " << mPixelsY << ", " << var.bits_per_pixel;
        LOG(LogLvl::DEBUG) << "var.xoffset, var.left_margin: " << var.xoffset << ", " << var.left_margin;
        LOG(LogLvl::DEBUG) << "var.yoffset, var.upper_margin: " << var.yoffset << ", " << var.upper_margin;

        mPixelSize = var.bits_per_pixel / 8;
        if (2 != mPixelSize)
        {
            close(mFdFb);
            throw std::runtime_error("Cannot use this device, color depth: " + std::to_string(var.bits_per_pixel));
        }
        mScrSize = mPixelsY * mPixelsX * mPixelSize;
        LOG(LogLvl::DEBUG) << "Memory and pixel sizes, bytes: " << mScrSize << ", " << mPixelSize;

        mFbPtr = (char *)mmap(0, mScrSize, PROT_READ | PROT_WRITE, MAP_SHARED, mFdFb, 0);
        int int_result = reinterpret_cast<std::intptr_t>(mFbPtr);
        if (int_result == -1)
        {
            close(mFdFb);
            throw std::runtime_error("Cannot map frame buffer memory");
        }
        LOG(LogLvl::DEBUG) << "Frame buffer memory mapped";
    }
    virtual ~FrameBuffer()
    {
        munmap(mFbPtr, mScrSize);
        close(mFdFb);
    }

    void draw_square(int x, int y, int width, int height, int colidx) const
    {
        int h = 0;
        int w = 0;
        unsigned short color = idx_to_color(colidx);
        for (h = -height / 2; h < height / 2; h++)
            for (w = -width / 2; w < width / 2; w++)
                put_pixel(h + x, w + y, color);
    }

    void clear() const
    {
        memset(mFbPtr, 0, mScrSize);
    }

    void putString(int x, int y, const char *s, uint colidx)
    {
        int i;
        for (i = 0; *s; i++, x += mFont.width, s++)
            putChar(x, y, *s, colidx);
    }

protected:
    void putChar(int x, int y, unsigned char chr, uint colidx)
    {
        uint font_offset = chr * mFont.height * mFont.width / 8;
        uint color = idx_to_color(colidx);

        for (int row = 0; row < mFont.height; row++)
        {
            uint pix_offset = ((y + row) * mPixelsX + x) * mPixelSize;
            for (int col = 0; col < mFont.width / 8; col++)
            {
                unsigned char bits = mFont.data[font_offset++];
                for (int j = 0; j < 8; j++, bits <<= 1)
                {
                    unsigned short scr_color = (bits & 0x80) ? color : 0;
                    *((unsigned short *)(mFbPtr + pix_offset)) = scr_color;
                    pix_offset += mPixelSize;
                }
            }
        }
    }

    void putPixel(int x, int y, int r, int g, int b) const
    {
        int pix_offset = x * mPixelSize + y * mPixelsX * mPixelSize;
        if (pix_offset < 0 || pix_offset > (int)(this->mScrSize - mPixelSize))
        {
            return;
        }
        unsigned short color = ((r / 8) << 11) + ((g / 4) << 5) + (b / 8);
        // write 'two bytes at once'
        *((unsigned short *)(mFbPtr + pix_offset)) = color;
    }

    void put_pixel(int x, int y, unsigned short color) const
    {
        int pix_offset = x * mPixelSize + y * mPixelsX * mPixelSize;
        if (pix_offset < 0 || pix_offset > (int)(this->mScrSize - mPixelSize))
        {
            return;
        }
        // write 'two bytes at once'
        *((unsigned short *)(mFbPtr + pix_offset)) = color;
    }
    unsigned short idx_to_color(int colidx) const
    {
        unsigned short r = def_r[colidx];
        unsigned short g = def_g[colidx];
        unsigned short b = def_b[colidx];
        return ((r / 8) << 11) + ((g / 4) << 5) + (b / 8);
    }
};

#endif