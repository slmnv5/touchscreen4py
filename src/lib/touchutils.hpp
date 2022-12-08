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
#include "lib/utils.hpp"

#define KWHT "\x1B[37m"
#define KYEL "\x1B[33m"

int fillArrayFromDevice(int fd, int propId, const char *propName, int *minV, int *maxV)
{
    const char *arrPropName[6] = {"Value", "Min", "Max", "Fuzz", "Flat", "Resolution"};
    int arrPropValue[6] = {0, 0, 0, 0, 0, 0};

    if (ioctl(fd, EVIOCGABS(propId), arrPropValue) < 0)
    {
        LOG(LogLvl::ERROR) << "Could not fill property: " << propName;
        return -1;
    }
    LOG(LogLvl::INFO) << "Properties " << propName;
    for (int x = 0; x < 6; x++)
    {
        if ((x < 3) || arrPropValue[x])
        {
            LOG(LogLvl::INFO) << arrPropName[x] << ": " << arrPropValue[x];
        }
    }
    *minV = arrPropValue[1];
    *maxV = arrPropValue[2];

    return 0;
}

void getFrameBuffInfo(int *xres, int *yres, int fdfb)
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
    return 0;
}

// return screen file descr. and details
void getTouchInfo(int *scrXmin, int *scrXmax,
                  int *scrYmin, int *scrYmax,
                  int *scrPmin, int *scrPmax,
                  int fdscr)

{

    int r1 = fillArrayFromDevice(fdscr, ABS_X, "ABS_X", scrXmin, scrXmax);
    int r2 = fillArrayFromDevice(fdscr, ABS_Y, "ABS_Y", scrYmin, scrYmax);
    int r3 = fillArrayFromDevice(fdscr, ABS_PRESSURE, "ABS_PRESSURE", scrPmin, scrPmax);

    if (r1 < 0 || r2 < 0 || r3 < 0)
    {
        close(fdscr);
        throw std::runtime_error("Cannot read touch screen file");
    }
};

// auto SCALE_X = (1920.0f / absX[2]);
// auto SCALE_Y = (1080.0f / absY[2]);
}

void run_test()
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

    const char *events[EV_MAX + 1] = {};
    events[EV_SYN] = "Sync";
    events[EV_KEY] = "Key";
    events[EV_REL] = "Relative";
    events[EV_ABS] = "Absolute";
    events[EV_MSC] = "Misc";
    events[EV_LED] = "LED";
    events[EV_SND] = "Sound";
    events[EV_REP] = "Repeat";
    events[EV_FF] = "ForceFeedback";
    events[EV_PWR] = "Power";
    events[EV_FF_STATUS] = "ForceFeedbackStatus";

    int rawX;
    int rawY;
    int rawPressure;

    struct input_event ev;
    while (true)
    {
        const ssize_t ev_size = sizeof(struct input_event);

        read(fd, &ev, ev_size);

        if (ev.type == EV_SYN)
            printf("Event type is %s%s%s = Start of New Event\n", KYEL, events[ev.type], KWHT);

        else if (ev.type == EV_KEY && ev.code == 330 && ev.value == 1)
            printf("Event type is %s%s%s & Event code is %sTOUCH(330)%s & Event value is %s1%s = Touch Starting\n", KYEL, events[ev.type], KWHT, KYEL, KWHT, KYEL, KWHT);

        else if (ev.type == EV_KEY && ev.code == 330 && ev.value == 0)
            printf("Event type is %s%s%s & Event code is %sTOUCH(330)%s & Event value is %s0%s = Touch Finished\n", KYEL, events[ev.type], KWHT, KYEL, KWHT, KYEL, KWHT);

        else if (ev.type == EV_ABS)
        {
            if (ev.code == 0 && ev.value > 0)
            {
                rawX = ev.value;
            }
            else if (ev.code == 1 && ev.value > 0)
            {
                rawY = ev.value;
            }
            else if (ev.code == 24 && ev.value > 0)
            {
                rawPressure = ev.value;
            }
        }
    }
}

#endif
