
#include "TouchScreenPy.hpp"
#include "lib/log.hpp"

extern "C"
{

    void *createTouchScreen(int fbId)
    {
        try
        {
            return new TouchScreenPy(fbId);
        }
        catch (...)
        {
            return 0;
        }
    }

    void deleteTouchScreen(void *ptr)
    {
        TouchScreenPy *tsp = static_cast<TouchScreenPy *>(ptr);
        delete tsp;
    }

    void stop(void *ptr)
    {
        TouchScreenPy *tsp = static_cast<TouchScreenPy *>(ptr);
        tsp->mStopped = true;
    }

    void start(void *ptr)
    {
        TouchScreenPy *tsp = static_cast<TouchScreenPy *>(ptr);
        tsp->mStopped = false;
    }

    const char *getClickEventWord(void *ptr)
    {
        try
        {
            TouchScreenPy *tsp = static_cast<TouchScreenPy *>(ptr);
            return tsp->getClickEventWord().c_str();
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
            TouchScreenPy *tsp = static_cast<TouchScreenPy *>(ptr);
            tsp->setLoop(loopSeconds, loopPosition, isRec, isStop);
            return 0;
        }
        catch (...)
        {
            return -1;
        }
    }

    int clearScreen(void *ptr)
    {
        try
        {
            TouchScreenPy *tsp = static_cast<TouchScreenPy *>(ptr);
            tsp->clearScreen();
            return 0;
        }
        catch (...)
        {
            return -1;
        }
    }
    int setRowText(void *ptr, uint row, const char *text, uint r, uint g, uint b)
    {
        try
        {
            TouchScreenPy *tsp = static_cast<TouchScreenPy *>(ptr);
            tsp->setRowText(row, text, r, g, b);
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

    int getCols(void *ptr)
    {
        try
        {
            TouchScreenPy *tsp = static_cast<TouchScreenPy *>(ptr);
            return tsp->getCols();
        }
        catch (...)
        {
            return -1;
        }
    }

    int getRows(void *ptr)
    {
        try
        {
            TouchScreenPy *tsp = static_cast<TouchScreenPy *>(ptr);
            return tsp->getRows();
        }
        catch (...)
        {
            return -1;
        }
    }

    int putSquare(void *ptr, uint x, uint y, uint width, uint height, uint r, uint g, uint b) {
        try
        {
            TouchScreenPy *tsp = static_cast<TouchScreenPy *>(ptr);
            uint color =  ((r / 8) << 11) + ((g / 4) << 5) + (b / 8);
            tsp->putSquare(x, y, width, height, color);
            return 1;
        }
        catch (...)
        {
            return -1;
        }
    }

    int putSquareInv(void *ptr, uint x, uint y, uint width, uint height) {
        try
        {
            TouchScreenPy *tsp = static_cast<TouchScreenPy *>(ptr);            
            tsp->putSquareInv(x, y, width, height);
            return 1;
        }
        catch (...)
        {
            return -1;
        }
    }

}
