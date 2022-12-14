#ifndef TOUCBUFF_H
#define TOUCBUFF_H

#include "pch.hpp"

#include "FrameBuffer.hpp"
#include "TouchScreen.hpp"
#include "lib/utils.hpp"
#include "lib/log.hpp"

static const char LINE_DELIMTER = 10;

class TouchScreenPy : public TouchScreen
{

public:
    TouchScreenPy(int fbId = 1) : TouchScreen(fbId, false, true)
    {
    }
    virtual ~TouchScreenPy()
    {
    }

    void setLogLevel(uint lvl) const
    {
        LOG::ReportingLevel() = static_cast<LogLvl>(lvl);
    }

    std::string getClickEvent()
    {
        auto pairColRow = this->getClickEventColRow();
        auto line = this->mTextLines.at(pairColRow.second);
        auto word = wordAtPosition(line, pairColRow.first, '[', ']');
        LOG(LogLvl::DEBUG) << line << ", word: " << word;
        return (word.length() > 0) ? word : "";
    }

    void setText(const char *text)
    {
        mTextLines = splitString(text, LINE_DELIMTER);
        mFrameBuffer.clearScr();
        LOG(LogLvl::DEBUG) << text << ", lines: " << mTextLines.size();
        for (uint i = 0; i < mTextLines.size(); i++)
        {
            auto line = mTextLines.at(i);
            unsigned short color = COLOR_INDEX::WHITE;
            if (line.rfind("*", 0) == 0)
            {
                color = mIsRec ? COLOR_INDEX::RED : COLOR_INDEX::GREEN;
            }
            else if (line.rfind("~", 0) == 0)
            {
                color = COLOR_INDEX::YELLOW;
            }
            this->mFrameBuffer.putString(0, i * mFrameBuffer.mFont.height, line.c_str(), color);
        }
    }

    void setLoop(double loopSeconds, double loopPosition, bool isRec, bool isStop)
    {
        this->mLoopSeconds = loopSeconds;
        this->mLoopPosition = loopPosition;
        this->mIsRec = isRec;
        this->mIsStop = isStop;
    }
};

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
    int setText(void *ptr, const char *text)
    {
        try
        {
            TouchScreenPy *x = static_cast<TouchScreenPy *>(ptr);
            x->setText(text);
            return 0;
        }
        catch (...)
        {
            return -1;
        }
    }

    int setLogLevel(void *ptr, int lvl)
    {
        try
        {
            TouchScreenPy *x = static_cast<TouchScreenPy *>(ptr);
            x->setLogLevel(lvl);
            return 0;
        }
        catch (...)
        {
            return -1;
        }
    }
}

#endif