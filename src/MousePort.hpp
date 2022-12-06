#ifndef MOUSEPORT_H
#define MOUSEPORT_H

//#include "pch.hpp"
#include "lib/touchutils.hpp"

class MousePort
{
private:
    int screenXmin, screenXmax, screenYmin, screenYmax;
    int xresolution, yresolution;
    float scaleXvalue, scaleYvalue;
    int rawX, rawY, rawPressure, scaledX, scaledY;
    int fd;

public:
    MousePort()
    {
        std::string dev_id = find_touchscr_event();
        if ("" == dev_id)
        {
            throw std::runtime_error("Cannot find touch screen device");
        }
        std::string fname = "/dev/input/event" + dev_id;
        if (framebuff_info(&xresolution, &yresolution) < 0)
        {
            throw std::runtime_error("Cannot open frame buffer file");
        }

        fd = touchscr_info(&screenXmin, &screenXmax, &screenYmin, &screenYmax, fname);
        if (fd == -1)
        {
            throw std::runtime_error("Cannot open touch screen file");
        }
        std::thread(&MousePort::run, this).detach();
    }
    virtual ~MousePort()
    {
    }

private:
    void run()
    {

        struct input_event ie;

        while (read(fd, &ie, sizeof(struct input_event)) != -1)
        {

            if (ie.type != EV_REL && ie.type != EV_ABS && ie.type != EV_KEY)
            {
                continue;
            }
            printf("-------type %d\tcode %d\tvalue %d\n", ie.type, ie.code, ie.value);
            printf("\n");
        }
    }
};

#endif