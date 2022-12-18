
#include "TouchScreenPy.hpp"

std::string TouchScreenPy::getClickEvent()
{
    auto pairColRow = this->getClickEventColRow();
    auto line = this->mTextLines.at(pairColRow.second);
    auto word = wordAtPosition(line, pairColRow.first, '[', ']');
    LOG(LogLvl::DEBUG) << line << ", word: " << word;
    return (word.length() > 0) ? word : "";
}

void TouchScreenPy::setText(const char *text, uint col, uint row, uint r, uint g, uint b)
{
    unsigned short color = ((r / 8) << 11) + ((g / 4) << 5) + (b / 8);
    this->mFrameBuffer.putString(col * mFrameBuffer.mFont.width, row * mFrameBuffer.mFont.height, text, color);
}

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
            x->mFrameBuffer.clearScreen(startY);
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
            x->setText(text, row, col, r, g, b);
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
