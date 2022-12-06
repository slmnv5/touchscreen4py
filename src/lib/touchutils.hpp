#ifndef TOUCHUTILS_H
#define TOUCHUTILS_H

#include <linux/input.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include "lib/log.hpp"

#define KWHT "\x1B[37m"
#define KYEL "\x1B[33m"

#define EVENT_TYPE EV_ABS
#define EVENT_CODE_X ABS_X
#define EVENT_CODE_Y ABS_Y

int framebuff_info(int *xres, int *yres)
{
    *xres = *yres = -1;
    struct fb_var_screeninfo var;
    int fb = 0;
    fb = open("/dev/fb1", O_RDONLY | O_NONBLOCK);
    if (fb == -1)
    {
        return -1;
    }

    if (ioctl(fb, FBIOGET_VSCREENINFO, &var) < 0)
    {
        close(fb);
        return -1;
    }

    LOG(LogLvl::INFO) << "Screen resolution X, Y: " << var.xres << ", " << var.yres;
    *xres = var.xres;
    *yres = var.yres;
    close(fb);
    return 0;
}

// return screen file descr. and details
int touchscr_info(int *scrXmin, int *scrXmax,
                  int *scrYmin, int *scrYmax, std::string fname)

{

    int fd;
    if ((fd = open(fname.c_str(), O_RDONLY)) < 0)
    {
        LOG(LogLvl::ERROR) << "Could not open device file: " << fname;
        return -1;
    }

    const char *absval[6] = {"Value", "Min", "Max", "Fuzz", "Flat", "Resolution"};
    int absX[6] = {};
    int absY[6] = {};

    if (ioctl(fd, EVIOCGABS(ABS_MT_POSITION_X), absX) < 0)
    {
        LOG(LogLvl::ERROR) << "Could not open ABS_MT_POSITION_X";
        return -1;
    }
    if (ioctl(fd, EVIOCGABS(ABS_MT_POSITION_Y), absY) < 0)
    {
        LOG(LogLvl::ERROR) << "Could not open ABS_MT_POSITION_Y";
        return -1;
    }

    LOG(LogLvl::INFO) << "ABS_MT_POSITION_X Properties";
    for (int x = 0; x < 6; x++)
    {
        if ((x < 3) || absX[x])
        {
            LOG(LogLvl::INFO) << absval[x] << ": " << absX[x];
        }
    }

    LOG(LogLvl::INFO) << "ABS_MT_POSITION_Y Properties";
    for (int y = 0; y < 6; y++)
    {
        if ((y < 3) || absX[y])
        {
            LOG(LogLvl::INFO) << absval[y] << ": " << absY[y];
        }
    }

    *scrXmin = absX[1];
    *scrXmax = absX[2];
    *scrYmin = absY[1];
    *scrYmax = absY[2];

    // auto SCALE_X = (1920.0f / absX[2]);
    // auto SCALE_Y = (1080.0f / absY[2]);

    return fd;
}

void get2(int fd)
{

    struct input_event ev;

    while (true)
    {
        const ssize_t ev_size = sizeof(struct input_event);

        auto size = read(fd, &ev, ev_size);
        if (size < ev_size)
        {
            throw std::runtime_error("Error in size when reading touch screen");
        }

        if (ev.type == EVENT_TYPE && (ev.code == EVENT_CODE_X || ev.code == EVENT_CODE_Y))
        {
            LOG(LogLvl::INFO) << (ev.code == EVENT_CODE_X ? "X" : "Y") << ev.value;
        }
    }
}

void get1()
{
    std::string dev_id = find_touchscr_event();
    if ("" == dev_id)
    {
        throw std::runtime_error("Cannot find touch screen device");
    }
    std::string fname = "/dev/input/event" + dev_id;
    int fd = open(fname.c_str(), O_RDONLY);
    if (fd < 0)
    {
        throw std::runtime_error("Cannot open touch screen device");
    }
    get2(fd);
}

#endif
