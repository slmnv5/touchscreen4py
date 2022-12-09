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

        int minV, maxV;
        getFromDevice(ABS_X, minV, maxV);
        minX = minV;
        scaleX = 1.0 / (maxV - minV) * resx;

        getFromDevice(ABS_Y, minV, maxV);
        minY = minV;
        scaleY = 1.0 / (maxV - minV) * resy;

        getFromDevice(ABS_PRESSURE, minV, maxV);
        minP = minV;
        scaleP = 1.0 / (maxV - minV);

        LOG(LogLvl::INFO) << "Opened touch screen device: " << name << ", scaleX: " << scaleX << ", scaleY: " << scaleY
                          << ", minX: " << minX << ", minY: " << minY;
    }
    ~TouchScr() {}

    void run(FrameBuff &fb)
    {
        int valx, valy, savex, savey;
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
                    savex = valx;
                    savey = valy;
                    moment = std::chrono::steady_clock::now();
                }
                else
                {
                    auto duration = moment - std::chrono::steady_clock::now();
                    if (duration.count() > 0.5 && abs(valx - savex) / scaleX < 0.1 && abs(valy - savey) / scaleY < 0.1)
                    {
                        LOG(LogLvl::DEBUG) << "Button click" << valx << valy;
                    }
                }
            }

            else if (ev.type == EV_ABS && ev.code == ABS_X && ev.value > minX)
            {
                valx = (ev.value - minX) * scaleX;
            }
            else if (ev.type == EV_ABS && ev.code == ABS_Y && ev.value > minY)
            {
                valy = (ev.value - minY) * scaleY;
            }
            else
            {
                continue;
            }
            assert(0 <= valx <= 480);
            assert(0 <= valy <= 320);
            LOG(LogLvl::DEBUG) << "valx, valy" << valx << ", " << valy;
            // fb.drawSquare(valx, valy, 11, 11, COLOR_INDEX_T::GREEN);
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
