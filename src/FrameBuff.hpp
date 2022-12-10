#ifndef FRAMEBUFF_H
#define FRAMEBUFF_H

// #include <stdlib.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "pch.hpp"
#include "fbfonts.hpp"

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
    int fdfb = -1;       // file descriptor
    char *fbp = 0;       // pointer to frame buffer memory
    int resX, resY;      // screen resolution
    uint screensize = 0; // screen memory size in bytes
    uint linesize = 0;   // screen line size in bytes
    fb_pixel_font *font = &font_8x8;

public:
    FrameBuff(int fbidx = 1)
    {
        // fb1 connected to LCD screen, dont know how to change
        std::string fbname = "/dev/fb" + std::to_string(fbidx);
        fdfb = open(fbname.c_str(), O_RDWR);
        if (fdfb < 0)
        {
            throw std::runtime_error("Cannot open frame buffer file: " + fbname);
        }
        struct fb_var_screeninfo var;
        if (ioctl(fdfb, FBIOGET_VSCREENINFO, &var) < 0)
        {
            close(fdfb);
            throw std::runtime_error("Cannot read frame buffer file info: " + fbname);
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
            throw std::runtime_error("Cannot map frame buffer memory");
        }
        LOG(LogLvl::DEBUG) << "Frame buffer memory mapped";
    }
    ~FrameBuff()
    {
        munmap(fbp, screensize);
        close(fdfb);
    }

    void draw_square(int x, int y, int width, int height, int colidx) const
    {
        int h = 0;
        int w = 0;
        for (h = -height / 2; h < height / 2; h++)
            for (w = -width / 2; w < width / 2; w++)
                put_pixel_16bpp(h + x, w + y, def_r[colidx], def_g[colidx], def_b[colidx]);
    }

    void clear() const
    {
        memset(fbp, 0, screensize);
    }

    const int &res_x() const
    {
        return resX;
    }
    const int &res_y() const
    {
        return resY;
    }

    void put_string(int x, int y, char *s, uint colidx)
    {
        int i;
        for (i = 0; *s; i++, x += font->width, s++)
            put_char(x, y, *s, colidx);
    }

    void set_font(fb_pixel_font &f)
    {
        font = &f;
    }

private:
    void put_pixel_16bpp(int x, int y, int r, int g, int b) const
    {
        int pix_offset = x * 2 + y * linesize;
        if (pix_offset < 0 || pix_offset > (int)(this->screensize - 2))
        {
            return;
        }
        unsigned short colidx = ((r / 8) << 11) + ((g / 4) << 5) + (b / 8);
        // write 'two bytes at once'
        *((unsigned short *)(fbp + pix_offset)) = colidx;
    }

    void put_char(int x, int y, int font_chr, uint colidx)
    {
        uint char_w = font->width / 8;
        uint char_sz = font->height * char_w;
        uint start = font_chr * char_sz;
        int i, j, bits;
        for (i = 0; i < char_sz; i++)
        {
            uint col = i % char_w;
            uint row = i / char_w;
            bits = fbp[start + i];
            for (j = 0; j < 8; j++, bits <<= 1)
                if (bits & 0x80)
                {
                    put_pixel_16bpp(x + col, y + row, def_r[colidx], def_g[colidx], def_b[colidx]);
                }
                else
                {
                    put_pixel_16bpp(x + col, y + row, 0, 0, 0);
                }
        }
    }
};

#endif