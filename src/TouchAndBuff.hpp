#ifndef TOUCBUFF_H
#define TOUCBUFF_H

#include "pch.hpp"

#include "FrameBuff.hpp"
#include "TouchScr.hpp"

class TouchAndBuff1
{
private:
    FrameBuff fb;
    TouchScr ts;

    std::thread run_thread;

public:
    TouchAndBuff1() : fb(), ts(fb.resx(), fb.resy(), true)
    {
        run_thread = std::thread(&TouchAndBuff1::run, this);
    }
    virtual ~TouchAndBuff1()
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
        return new (std::nothrow) TouchAndBuff1;
    }

    void deleteTchScr(void *ptr)
    {
        TouchAndBuff1 *x = static_cast<TouchAndBuff1 *>(ptr);
        delete x;
    }

    int stop(void *ptr)
    {
        TouchAndBuff1 *x = static_cast<TouchAndBuff1 *>(ptr);
        x->stop();
        return 0;
    }

    int start(void *ptr)
    {
        TouchAndBuff1 *x = static_cast<TouchAndBuff1 *>(ptr);
        x->start();
        return 0;
    }

    const char *getButtonName(void *ptr)
    {
        try
        {
            TouchAndBuff1 *x = static_cast<TouchAndBuff1 *>(ptr);
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
            TouchAndBuff1 *x = static_cast<TouchAndBuff1 *>(ptr);
            return x->setText(aa);
        }
        catch (...)
        {
            return -1;
        }
    }
}

#endif