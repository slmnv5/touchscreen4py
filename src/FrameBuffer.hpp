#ifndef FRAMEBUFF_H
#define FRAMEBUFF_H

#include <fcntl.h>     // For O_RDWR
#include <unistd.h>    // For open(), creat()
#include <cstring>     // memset
#include <linux/fb.h>  // FBIOGET_VSCREENINFO
#include <sys/mman.h>  // PROT_READ
#include <sys/ioctl.h> // ioctl
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
} COLOR_INDEX;

static unsigned short def_r[] =
    {0, 0, 0, 0, 172, 172, 172, 168,
     84, 84, 84, 84, 255, 255, 255, 255};
static unsigned short def_g[] =
    {0, 0, 168, 168, 0, 0, 84, 168,
     84, 84, 255, 255, 84, 84, 255, 255};
static unsigned short def_b[] =
    {0, 172, 0, 168, 0, 172, 0, 168,
     84, 255, 84, 255, 84, 255, 84, 255};

unsigned short idxToColor(uint colorIdx)
{
    unsigned short r = def_r[colorIdx];
    unsigned short g = def_g[colorIdx];
    unsigned short b = def_b[colorIdx];
    return ((r / 8) << 11) + ((g / 4) << 5) + (b / 8);
}

class FrameBuffer
{
private:
    int mFdFb = -1;      // file descriptor
    char *mFbPtr = 0;    // pointer to frame buffer memory
    uint mScrSize = 0;   // screen memory size in bytes
    uint mColorSize = 0; // screen color size in bytes

public:
    FbPixelFont &mFont = font_12x27; // font
    uint mPixelsX, mPixelsY;         // screen resolution

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

        mColorSize = var.bits_per_pixel / 8;
        if (2 != mColorSize)
        {
            close(mFdFb);
            throw std::runtime_error("Cannot use this device, color depth: " + std::to_string(var.bits_per_pixel));
        }
        mScrSize = mPixelsY * mPixelsX * mColorSize;
        LOG(LogLvl::DEBUG) << "Memory and color sizes, bytes: " << mScrSize << ", " << mColorSize;

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

    void putSquare(uint x, uint y, uint width, uint height, uint colorIdx) const
    {
        unsigned short color = idxToColor(colorIdx);
        for (uint h = 0; h < height; h++)
            for (uint w = 0; w < width; w++)
                putPixel(h + x, w + y, color);
    }

    void clear() const
    {
        memset(mFbPtr, 0, mScrSize);
    }

    void putSquareInv(uint x, uint y, uint width, uint height) const
    {
        for (uint h = 0; h < height; h++)
            for (uint w = 0; w < width; w++)
                putPixelInv(h + x, w + y);
    }

    void putString(uint x, uint y, const char *s, uint colorIdx)
    {
        unsigned short color = idxToColor(colorIdx);
        for (uint i = 0; *s; i++, x += mFont.width, s++)
            putChar(x, y, *s, color);
    }

    void putStringNice(uint x, uint y, const char *s)
    {
        unsigned short colorNormal = idxToColor(COLOR_INDEX::WHITE);
        unsigned short colorActive = idxToColor(COLOR_INDEX::YELLOW);

        auto color = colorNormal;
        for (; *s; x += mFont.width, s++)
        {
            if (color == colorNormal and *s == '[')
            {
                color = colorActive;
            }
            else if (color != colorNormal and *s == ']')
            {
                color = colorNormal;
            }
            putChar(x, y, *s, color);
        }
    }

protected:
    void
    putChar(uint x, uint y, unsigned char chr, unsigned short color)
    {
        uint mFontStep = std::ceil(mFont.width / 8.0);
        uint fontOffset = chr * mFontStep * mFont.height;
        for (uint row = 0; row < mFont.height; row++)
        {
            uint pixOffset = ((y + row) * mPixelsX + x) * mColorSize;
            for (uint i = 0; i < mFontStep; i++)
            {
                char bits = mFont.data[fontOffset++];
                for (uint j = 0; j < 8; j++, bits <<= 1)
                {
                    unsigned short scr_color = (bits & 0x80) ? color : 0;
                    *((unsigned short *)(mFbPtr + pixOffset)) = scr_color;
                    pixOffset += mColorSize;
                }
            }
        }
    }

    void putPixel(uint x, uint y, uint r, uint g, uint b) const
    {
        uint pix_offset = x * mColorSize + y * mPixelsX * mColorSize;
        if (pix_offset < 0 || pix_offset > this->mScrSize - mColorSize)
        {
            return;
        }
        unsigned short color = ((r / 8) << 11) + ((g / 4) << 5) + (b / 8);
        // write 'two bytes at once'
        *((unsigned short *)(mFbPtr + pix_offset)) = color;
    }

    void putPixel(uint x, uint y, unsigned short color) const
    {
        uint pix_offset = x * mColorSize + y * mPixelsX * mColorSize;
        if (pix_offset < 0 || pix_offset > this->mScrSize - mColorSize)
        {
            return;
        }
        // write 'two bytes at once'
        *((unsigned short *)(mFbPtr + pix_offset)) = color;
    }

    void putPixelInv(uint x, uint y) const
    {
        uint pix_offset = x * mColorSize + y * mPixelsX * mColorSize;
        if (pix_offset < 0 || pix_offset > this->mScrSize - mColorSize)
        {
            return;
        }
        unsigned short color = *((unsigned short *)(mFbPtr + pix_offset));
        *((unsigned short *)(mFbPtr + pix_offset)) = ~color;
    }
};

#endif