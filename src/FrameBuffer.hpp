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

// default framebuffer palette
typedef enum
{
    BLACK = 0,     /*   0,   0,   0 */
    BLUE = 1,      /*   0,   0, 172 */
    GREEN = 2,     /*   0, 172,   0 */
    CYAN = 3,      /*   0, 172, 172 */
    RED = 4,       /* 172,   0,   0 */
    PURPLE = 5,    /* 172,   0, 172 */
    ORANGE = 6,    /* 172,  84,   0 */
    LTGREY = 7,    /* 172, 172, 172 */
    GREY = 8,      /*  84,  84,  84 */
    LTBLUE = 9,    /*  84,  84, 255 */
    LTGREEN = 10,  /*  84, 255,  84 */
    LTCYAN = 11,   /*  84, 255, 255 */
    LTRED = 12,    /* 255,  84,  84 */
    LTPURPLE = 13, /* 255,  84, 255 */
    YELLOW = 14,   /* 255, 255,  84 */
    WHITE = 15     /* 255, 255, 255 */
} COLOR_IND;

static unsigned short def_r[] =
    {0, 0, 0, 0, 172, 172, 172, 168,
     84, 84, 84, 84, 255, 255, 255, 255};
static unsigned short def_g[] =
    {0, 0, 168, 168, 0, 0, 84, 168,
     84, 84, 255, 255, 84, 84, 255, 255};
static unsigned short def_b[] =
    {0, 172, 0, 168, 0, 172, 0, 168,
     84, 255, 84, 255, 84, 255, 84, 255};

inline unsigned short idxToColor(uint colorIdx)
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
protected:
    std::vector<std::string> mRowText; // text on screen
    FbPixelFont &mFont = font_16x32;   // font
    uint mPixelsX, mPixelsY;           // screen resolution

public:
    FrameBuffer(int fbidx = 1);

    virtual ~FrameBuffer()
    {
        munmap(mFbPtr, mScrSize);
        close(mFdFb);
    }

    void setRowText(uint row, const char *s, uint r, uint g, uint b);
    void clearScreen() const
    {
        memset(mFbPtr, 0, mScrSize);
    }

    void putSquare(uint x, uint y, uint width, uint height, uint color) const
    {
        for (uint h = 0; h < height; h++)
            for (uint w = 0; w < width; w++)
                putPixel(x + w, y + h, color);
    }

    void putSquareInv(uint x, uint y, uint width, uint height) const
    {
        for (uint h = 0; h < height; h++)
            for (uint w = 0; w < width; w++)
                putPixelInv(x + w, y + h);
    }
    uint getCols() const
    {
        return mPixelsX / mFont.width;
    }

    uint getRows() const
    {
        return mPixelsY / mFont.height;
    }

protected:
    void putChar(uint x, uint y, unsigned char chr, unsigned short color) const;
    void putPixel(uint x, uint y, unsigned short color) const;
    void putPixelInv(uint x, uint y) const;
};

#endif