#ifndef TOUCBUFF_H
#define TOUCBUFF_H

#include "pch.hpp"

#include "FrameBuff.hpp"
#include "TouchScr.hpp"

class TouchAndBuff
{
private:
    FrameBuff fb;
    TouchScr ts;

    std::thread run_thread;

public:
    TouchAndBuff() : fb(), ts(fb.resx(), fb.resy())
    {
        run_thread = std::thread(&TouchAndBuff::run, this);
    }
    virtual ~TouchAndBuff()
    {
        run_thread.join();
    }
    void stop()
    {
        ts.stopped = true;
    }
    void start()
    {
        ts.stopped = false;
    }
    int setText(const char *)
    {
        return 0;
    }

    void run()
    {
        ts.run(fb);
    }
};

extern "C"
{

    void *createTchScr()
    {
        return new (std::nothrow) TouchAndBuff;
    }

    void deleteTchScr(void *ptr)
    {
        TouchAndBuff *x = static_cast<TouchAndBuff *>(ptr);
        delete x;
    }

    int stop(void *ptr)
    {
        TouchAndBuff *x = static_cast<TouchAndBuff *>(ptr);
        x->stop();
        return 0;
    }

    int start(void *ptr)
    {
        TouchAndBuff *x = static_cast<TouchAndBuff *>(ptr);
        x->start();
        return 0;
    }

    const char *getButtonName(void *ptr)
    {
        try
        {
            TouchAndBuff *x = static_cast<TouchAndBuff *>(ptr);
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
            TouchAndBuff *x = static_cast<TouchAndBuff *>(ptr);
            return x->setText(aa);
        }
        catch (...)
        {
            return -1;
        }
    }
}

#endif