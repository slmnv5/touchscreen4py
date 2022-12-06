#ifndef MOUSEPORT_H
#define MOUSEPORT_H

//#include "pch.hpp"
#include "lib/touchutils.hpp"

class MousePort
{
private:
    int scrXmin, scrXmax, scrYmin, scrYmax, scrPmin, scrPmax;
    int xresolution, yresolution;
    float scaleX, scaleY, scaleP;
    int rawX, rawY, rawPressure, scaledX, scaledY;
    int fd;

public:
    MousePort()
    {
        if (getFrameBuffInfo(&xresolution, &yresolution, "/dev/fb1") < 0)
        {
            throw std::runtime_error("Cannot open frame buffer file");
        }
        std::string dev_id = find_touchscr_event();
        if ("" == dev_id)
        {
            throw std::runtime_error("Cannot find touch screen device");
        }
        std::string fname = "/dev/input/event" + dev_id;
        fd = getTouchInfo(&scrXmin, &scrXmax, &scrYmin, &scrYmax, &scrPmin, &scrPmax, fname);
        if (fd == -1)
        {
            throw std::runtime_error("Cannot open touch screen file");
        }
        std::thread(&MousePort::run, this).detach();
    }
    virtual ~MousePort()
    {
    }

    void run()
    {

        struct input_event ie;
        int touch_on = 0;
        while (read(fd, &ie, sizeof(struct input_event)) != -1)
        {
            if (ev.type == EV_KEY && ev.code == 330)
            {
                touch_on = ev.value;
            }
            else if (ie.type = EV_ABS && ev.code == 0 && ev.value > 0)
            {
                rawX = ev.value;
            })
            else if (ie.type = EV_ABS && ev.code == 1 && ev.value > 0)
            {
                rawY = ev.value;
            })
               else if (ie.type = EV_ABS && ev.code == 0 && ev.value > 0)
            {
                rawX = ev.value;
            })

            {
                continue;
            }
            LOG(LogLvl::DEBUG) << "type: " << ie.type << " code: " << ie.code << " value: " << ie.value;
        }
    }
};

#endif