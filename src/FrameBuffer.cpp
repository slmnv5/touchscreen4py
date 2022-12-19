#include "FrameBuffer.hpp"
#include "fbfonts.hpp"
#include "lib/log.hpp"

FrameBuffer::FrameBuffer(int fbidx)
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
        throw std::runtime_error("Device color depth not supported: " + std::to_string(var.bits_per_pixel));
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
    for (uint i = 0; i < mPixelsY / mFont.height; i++)
    {
        mRowText.push_back("");
    }

    if (ioctl(mFdFb, FBIOBLANK, VESA_POWERDOWN) < 0)
    {
        LOG(LogLvl::ERROR) << "Cannot disable cursor";
    }
    LOG(LogLvl::DEBUG) << "Frame buffer memory mapped. Number of screen rows: "
                       << getRows() << " columns: " << getCols();
}

void FrameBuffer::setRowText(uint row, const char *s, uint r, uint g, uint b)
{
    if (row >= mRowText.size())
        return;
    unsigned short color = ((r / 8) << 11) + ((g / 4) << 5) + (b / 8);

    mRowText.at(row) = std::string(s).substr(0, getCols());
    for (uint i = 0; *s and i < getCols(); i++, s++)
        putChar(i * mFont.width, row * mFont.height, *s, color);
}

void FrameBuffer::putChar(uint x, uint y, unsigned char chr, unsigned short color) const
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

void FrameBuffer::putPixel(uint x, uint y, uint r, uint g, uint b) const
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

void FrameBuffer::putPixel(uint x, uint y, unsigned short color) const
{
    uint pix_offset = x * mColorSize + y * mPixelsX * mColorSize;
    if (pix_offset < 0 || pix_offset > this->mScrSize - mColorSize)
    {
        return;
    }
    // write 'two bytes at once'
    *((unsigned short *)(mFbPtr + pix_offset)) = color;
}

void FrameBuffer::putPixelInv(uint x, uint y) const
{
    uint pix_offset = x * mColorSize + y * mPixelsX * mColorSize;
    if (pix_offset < 0 || pix_offset > this->mScrSize - mColorSize)
    {
        return;
    }
    unsigned short color = *((unsigned short *)(mFbPtr + pix_offset));
    *((unsigned short *)(mFbPtr + pix_offset)) = ~color;
}