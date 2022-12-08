#ifndef MOUSEPORT_H
#define MOUSEPORT_H

// #include "pch.hpp"
#include "lib/touchutils.hpp"

class MousePort
{
private:
    int minX, minY, minP;
    float scaleX, scaleY, scaleP;
    int fdscr, fdfb;
    std::thread run_thread;

public:
    MousePort()
    {
        int maxX, maxY, maxP;
        int resolutionX, resolutionY;
        fdfb = open("/dev/fb1", O_RDWR);
        if (fdfb < 0)
        {
            throw std::runtime_error("Cannot open frame buffer file");
        }

        getFrameBuffInfo(&xresolution, &yresolution, fdfb);
        std::string dev_id = find_touchscr_event();
        if ("" == dev_id)
        {
            throw std::runtime_error("Cannot find touch screen device");
        }
        std::string fname = "/dev/input/event" + dev_id;
        int fdscr = open(fname.c_str(), O_RDONLY);
        if (fdscr < 0)
        {
              throw std::runtime_error("Could not open touch screen device file: " << fname;
        }
        getTouchInfo(&minX, &maxX, &minY, &maxY, &minP, &maxP, fdscr);
        scaleX = resolutionX / (maxX - minX);
        scaleY = resolutionY / (maxY - minY);
        scaleP = 1.0 / (maxP - minP);
        run_thread = std::thread(&MousePort::run, this);
    }
    virtual ~MousePort()
    {
        stopped = true;
        run_thread.join();
    }

    void run()
    {
        int scaledX, scaledY;
        auto moment = std::chrono::steady_clock();
        int touch_on = 0;

        struct input_event ie;

        while (read(fd, &ie, sizeof(struct input_event)) != -1)
        {
              if (stoppeed)
                  break;

              if (ev.type == EV_KEY && ev.code == BTN_TOUCH)
              {
                  touch_on = ev.value;
                  if (touch_on)
                  {
                      savedX = scaledX;
                      savedY = scaledY;
                      moment = std::chrono::steady_clock();
                  }
                  else
                  {
                      auto duration<double> = moment - std::chrono::steady_clock();
                      if (duration > 0.5 && abs(scaledX - savedX) / scaleX < 0.1 && abs(scaledY - savedY) / scaleY < 0.1)
                      {
                          LOG(LogLvl::DEBUG) << "Button click" << scaledX << scaledY;
                      }
                  }
              }
        }
        else if (ie.type = EV_ABS && ev.code == ABS_X && ev.value > 0)
        {
              scaledX = (ev.value - minX) * scaleX;
        }
        else if (ie.type = EV_ABS && ev.code == ABS_Y && ev.value > 0)
        {
              scaledY = (ev.value - minY) * scaleY;
        }
        else if (ie.type = EV_ABS && ev.code == ABS_PRESSURE && ev.value > 0)
        {
              scaledP = (ev.value - minP) * scaleP;
        }
        else
        {
              continue;
        }
        LOG(LogLvl::DEBUG) << "type: " << ie.type << " code: " << ie.code << " value: " << ie.value;
    }
}
}
;

extern "C"
{

    void *createClass(void)
    {
        return new (std::nothrow) MousePort;
    }

    void deleteClass(void *ptr)
    {
        MousePort *x = static_cast<MousePort *>(ptr);
        delete x;
    }

    int stop(void *ptr)
    {
        MousePort *x = static_cast<MousePort *>(ptr);
        x->stop();
    }

    int start(void *ptr)
    {
        MousePort *x = static_cast<MousePort *>(ptr);
        x->start();
    }

    const char *getButtonName(void *ptr)
    {

        try
        {
              MousePort *x = static_cast<MousePort *>(ptr);
              std::string temp = x->getID();
              return temp.c_str();
        }
        catch (...)
        {
              return "-1";
        }
    }

    int setValues(void *ptr)
    {

        try
        {
              MousePort *x = static_cast<MousePort *>(ptr);
              return x->getValue();
        }
        catch (...)
        {
              return -1;
        }
    }
}

#endif