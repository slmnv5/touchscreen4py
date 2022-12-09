#ifndef TOUCHSCR_H
#define TOUCHSCR_H

#include <linux/input.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>

#include "lib/log.hpp"
#include "FrameBuff.hpp"
#include "pch.hpp"

std::string find_touchscr_event()
{
    const char *cmd = "grep -E 'Handlers|EV=' /proc/bus/input/devices | "
                      "grep -B1 'EV=b' | grep -Eo 'event[0-9]+' | grep -Eo '[0-9]+' | tr -d '\n'";

    FILE *pipe = popen(cmd, "r");
    char buffer[128];
    std::string result = "";
    while (!feof(pipe))
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    pclose(pipe);
    return result;
}

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

class TouchScr
{
private:
    int fdscr;
    int minX, minY, minP;
    float scaleX, scaleY, scaleP;

public:
    bool stopped = false;

    TouchScr(int resx, int resy)
    {
        if (resx <= 0 || resy <= 0)
        {
            throw std::runtime_error("Screen resolution must be positive");
        }
        std::string dev_id = find_touchscr_event();
        if ("" == dev_id)
        {
            throw std::runtime_error("Cannot find touch screen device");
        }
        std::string fname = "/dev/input/event" + dev_id;
        fdscr = open(fname.c_str(), O_RDONLY);
        if (fdscr < 0)
        {
            throw std::runtime_error("Could not open touch screen device file: " + fname);
        }
        char name[256] = "Unknown";
        ioctl(fdscr, EVIOCGNAME(sizeof(name)), name);

        int maxX, maxY, maxP;
        getFromDevice(ABS_X, minX, maxX);
        getFromDevice(ABS_Y, minY, maxY);
        getFromDevice(ABS_PRESSURE, minP, maxP);

        scaleX = resx / (maxX - minX);
        scaleY = resy / (maxY - minY);
        scaleP = 1.0 / (maxP - minP);
        LOG(LogLvl::INFO) << "Opened touch screen device: " << name << ", scaleX: " << scaleX << ", scaleY: " << scaleY;
    }
    ~TouchScr() {}

    void run(FrameBuff &fb)
    {
        int scaledX, scaledY, savedX, savedY;
        auto moment = std::chrono::steady_clock::now();
        int touch_on = 0;

        struct input_event ev;

        while (read(fdscr, &ev, sizeof(struct input_event)) != -1)
        {
            if (stopped)
                break;

            if (ev.type == EV_KEY && ev.code == BTN_TOUCH)
            {
                touch_on = ev.value;
                if (touch_on)
                {
                    savedX = scaledX;
                    savedY = scaledY;
                    moment = std::chrono::steady_clock::now();
                }
                else
                {
                    auto duration = moment - std::chrono::steady_clock::now();
                    if (duration.count() > 0.5 && abs(scaledX - savedX) / scaleX < 0.1 && abs(scaledY - savedY) / scaleY < 0.1)
                    {
                        LOG(LogLvl::DEBUG) << "Button click" << scaledX << scaledY;
                    }
                }
                LOG(LogLvl::DEBUG) << "Touch: " << touch_on;
            }

            else if (ev.type == EV_ABS && ev.code == ABS_X && ev.value > 0)
            {
                scaledX = (ev.value - minX) * scaleX;
                LOG(LogLvl::DEBUG) << "X value: " << scaledX;
            }
            else if (ev.type == EV_ABS && ev.code == ABS_Y && ev.value > 0)
            {
                scaledY = (ev.value - minY) * scaleY;
                LOG(LogLvl::DEBUG) << "Y value: " << scaledY;
            }
            else
            {
                continue;
            }
            // fb.drawSquare(scaledX, scaledY, 25, 25, GREEN);
        }
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

        struct input_event ev;
        while (true)
        {
            const ssize_t ev_size = sizeof(struct input_event);

            read(fd, &ev, ev_size);
            LOG(LogLvl::DEBUG) << "type: " << events[ev.type] << " code: " << ev.code << " value: " << ev.value;
        }
    }

private:
    void getFromDevice(int propId, int &minV, int &maxV)
    {
        // const char *arrPropName[6] = {"Value", "Min", "Max", "Fuzz", "Flat", "Resolution"};
        int arrPropValue[6] = {};

        if (ioctl(fdscr, EVIOCGABS(propId), arrPropValue) < 0)
        {
            throw std::runtime_error("Cannot read touch screen device");
        }
        minV = arrPropValue[1];
        maxV = arrPropValue[2];
        LOG(LogLvl::DEBUG) << "ABS. property: " << propId << ", min: " << minV << ", max:" << maxV;
    }
};

#endif
