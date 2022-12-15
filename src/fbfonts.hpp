#ifndef FBFONTS_H
#define FBFONTS_H

#include "pch.hpp"

struct FbPixelFont
{
    const char *name;
    uint width, height;
    unsigned char *data;
};
extern struct FbPixelFont font_16x32;

#endif