#ifndef TOUCHSCR_H
#define TOUCHSCR_H

#include <linux/input.h>

#include "FrameBuff.hpp"
#include "pch.hpp"

#define MAX_QUEUE_SZ 20

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

using myclock = std::chrono::system_clock;
using sec = std::chrono::duration<double>;

class TouchScr
{
private:
    int fdscr;
    int minX, minY, minP;
    int maxX, maxY, maxP;
    std::queue<std::pair<float, float>> que;
    const FrameBuff fb;
    const bool invX;
    const bool invY;

public:
    bool stopped = false;

    TouchScr(bool invx, bool invy) : invX(invx), invY(invy)
    {

        if (fb.res_x() <= 0 || fb.res_y() <= 0)
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
        getFromDevice(ABS_X, minX, maxX);
        getFromDevice(ABS_Y, minY, maxY);
        getFromDevice(ABS_PRESSURE, minP, maxP);
        LOG(LogLvl::INFO) << "Opened touch screen device: " << name
                          << ", X: " << minX << "--" << maxX << ", Y: " << minY << "--" << maxY;
    }
    ~TouchScr() {}

    void run()
    {
        int x, y, savex, savey;
        x = y = savex = savey = 0;
        auto started = myclock::now();
        int touch_on = 0;
        bool button_click = false;
        float scaleX = 1.0 / (maxX - minX) * fb.res_x();
        float scaleY = 1.0 / (maxY - minY) * fb.res_y();
        LOG(LogLvl::DEBUG) << "Touch screen: "
                           << "scaleX: " << scaleX << ", scaleY: " << scaleY;

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
                    savex = x;
                    savey = y;
                    started = myclock::now();
                }
                else
                {
                    sec duration = myclock::now() - started;
                    if (duration.count() > 0.5 && abs(x - savex) / (maxX - minX) < 0.1 &&
                        abs(y - savey) / (maxY - minY) < 0.1)
                    {
                        button_click = true;
                    }
                }
            }
            else if (ev.type == EV_ABS && ev.code == ABS_X)
            {
                x = ev.value;
            }
            else if (ev.type == EV_ABS && ev.code == ABS_Y)
            {
                y = ev.value;
            }
            else
            {
                continue;
            }
            if (button_click and que.size() < MAX_QUEUE_SZ)
            {
                button_click = false;
                x = invX ? maxX - x : x;
                y = invY ? maxY - y : y;
                x = (x - minX) * scaleX;
                y = (y - minY) * scaleY;
                LOG(LogLvl::DEBUG) << "Button click at X, Y: " << x << " " << y;
                fb.draw_square(x, y, 15, 15, COLOR_INDEX_T::WHITE);
                que.push(std::pair<float, float>(x, y));
            }
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
