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

const char *events[EV_MAX + 1] = {
    events[EV_SYN] = "Sync",
    events[EV_KEY] = "Key",
    events[EV_REL] = "Relative",
    events[EV_ABS] = "Absolute",
    events[EV_MSC] = "Misc",
    events[EV_LED] = "LED",
    events[EV_SND] = "Sound",
    events[EV_REP] = "Repeat",
    events[EV_FF] = "ForceFeedback",
    events[EV_PWR] = "Power",
    events[EV_FF_STATUS] = "ForceFeedbackStatus"};

void fillArrayFromDevice(int fdscr, int propId, int *minV, int *maxV)
{
    const char *arrPropName[6] = {"Value", "Min", "Max", "Fuzz", "Flat", "Resolution"};
    int arrPropValue[6] = {0, 0, 0, 0, 0, 0};

    if (ioctl(fdscr, EVIOCGABS(propId), arrPropValue) < 0)
    {
        close(fdscr);
        throw std::runtime_error("Cannot read property of touch device: " + std::string(events[propId]));
    }
    LOG(LogLvl::INFO) << "Properties " << events[propId];
    for (int x = 0; x < 6; x++)
    {
        if ((x < 3) || arrPropValue[x])
        {
            LOG(LogLvl::INFO) << arrPropName[x] << ": " << arrPropValue[x];
        }
    }
    *minV = arrPropValue[1];
    *maxV = arrPropValue[2];
}

// return screen file descr. and details
void getTouchInfo(int *scrXmin, int *scrXmax,
                  int *scrYmin, int *scrYmax,
                  int *scrPmin, int *scrPmax,
                  int fdscr)

{

    fillArrayFromDevice(fdscr, ABS_X, scrXmin, scrXmax);
    fillArrayFromDevice(fdscr, ABS_Y, scrYmin, scrYmax);
    fillArrayFromDevice(fdscr, ABS_PRESSURE, scrPmin, scrPmax);
};

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

    struct input_event ev;
    while (true)
    {
        const ssize_t ev_size = sizeof(struct input_event);

        read(fd, &ev, ev_size);
        LOG(LogLvl::DEBUG) << "type: " << events[ev.type] << " code: " << ev.code << " value: " << ev.value;
    }
}

#endif
