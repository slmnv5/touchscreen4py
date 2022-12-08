#ifndef TOUCHSCR_H
#define TOUCHSCR_H

#include "pch.hpp"
#include "lib/touchutils.hpp"
#include "FrameBuff.hpp"

class TouchScr
{
private:
    FrameBuff fb;
    int minX, minY, minP;
    float scaleX, scaleY, scaleP;
    int fdscr;
    std::thread run_thread;
    bool stopped = false;

public:
    TouchScr() : fb()
    {
        int maxX, maxY, maxP;

        std::string dev_id = find_touchscr_event();
        if ("" == dev_id)
        {
            throw std::runtime_error("Cannot find touch screen device");
        }
        std::string fname = "/dev/input/event" + dev_id;
        int fdscr = open(fname.c_str(), O_RDONLY);
        if (fdscr < 0)
        {
            throw std::runtime_error("Could not open touch screen device file: " + fname);
        }
        // getTouchInfo(&minX, &maxX, &minY, &maxY, &minP, &maxP, fdscr);
        scaleX = fb.resx() / (maxX - minX);
        scaleY = fb.resy() / (maxY - minY);
        scaleP = 1.0 / (maxP - minP);
        run_thread = std::thread(&TouchScr::run, this);
    }
    virtual ~TouchScr()
    {
        run_thread.join();
    }
    void stop()
    {
        stopped = true;
    }
    void start()
    {
        stopped = false;
    }
    int setText(const char *)
    {
        return 0;
    }

private:
    void run()
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
            }

            else if (ev.type == EV_ABS && ev.code == ABS_X && ev.value > 0)
            {
                scaledX = (ev.value - minX) * scaleX;
            }
            else if (ev.type == EV_ABS && ev.code == ABS_Y && ev.value > 0)
            {
                scaledY = (ev.value - minY) * scaleY;
            }
            else if (ev.type == EV_ABS && ev.code == ABS_PRESSURE && ev.value > 0)
            {
            }
            else
            {
                continue;
            }
            LOG(LogLvl::DEBUG) << "type: " << events[ev.type] << " code: " << ev.code << " value: " << ev.value;
        }
    }
};

extern "C"
{

    void *createTchScr()
    {
        return new (std::nothrow) TouchScr;
    }

    void deleteTchScr(void *ptr)
    {
        TouchScr *x = static_cast<TouchScr *>(ptr);
        delete x;
    }

    int stop(void *ptr)
    {
        TouchScr *x = static_cast<TouchScr *>(ptr);
        x->stop();
        return 0;
    }

    int start(void *ptr)
    {
        TouchScr *x = static_cast<TouchScr *>(ptr);
        x->start();
        return 0;
    }

    const char *getButtonName(void *ptr)
    {
        try
        {
            TouchScr *x = static_cast<TouchScr *>(ptr);
            std::string temp = "x->getID()";
            return temp.c_str();
        }
        catch (...)
        {
            return "-1";
        }
    }

    int setValues(void *ptr, const char *aa)
    {

        try
        {
            TouchScr *x = static_cast<TouchScr *>(ptr);
            return x->setText(aa);
        }
        catch (...)
        {
            return -1;
        }
    }
}

#endif