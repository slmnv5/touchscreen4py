
#include "TouchScreenPy.hpp"

extern "C"
{

    void *createTouchScreen()
    {
        return new (std::nothrow) TouchScreenPy();
    }

    void deleteTouchScreen(void *ptr)
    {
        TouchScreenPy *x = static_cast<TouchScreenPy *>(ptr);
        delete x;
    }

    void stop(void *ptr)
    {
        TouchScreenPy *x = static_cast<TouchScreenPy *>(ptr);
        x->mStopped = true;
    }

    void start(void *ptr)
    {
        TouchScreenPy *x = static_cast<TouchScreenPy *>(ptr);
        x->mStopped = false;
    }

    const char *getClickEvent(void *ptr)
    {
        try
        {
            TouchScreenPy *x = static_cast<TouchScreenPy *>(ptr);
            return x->getClickEvent().c_str();
        }
        catch (...)
        {
            return "";
        }
    }

    int setLoop(void *ptr, double loopSeconds, double loopPosition, bool isRec, bool isStop)
    {
        try
        {
            TouchScreenPy *x = static_cast<TouchScreenPy *>(ptr);
            x->setLoop(loopSeconds, loopPosition, isRec, isStop);
            return 0;
        }
        catch (...)
        {
            return -1;
        }
    }

    int clearScreen(void *ptr, int startY)
    {
        try
        {
            TouchScreenPy *x = static_cast<TouchScreenPy *>(ptr);
            x->clearScreen(startY);
            return 0;
        }
        catch (...)
        {
            return -1;
        }
    }
    int setText(void *ptr, const char *text, int row, int col, int r, int g, int b)
    {
        try
        {
            TouchScreenPy *x = static_cast<TouchScreenPy *>(ptr);
            x->setText(text, row, r, g, b);
            return 0;
        }
        catch (...)
        {
            return -1;
        }
    }

    int setLogLevel(int lvl)
    {
        try
        {
            LOG::ReportingLevel() = static_cast<LogLvl>(lvl);
            return 0;
        }
        catch (...)
        {
            return -1;
        }
    }
}
