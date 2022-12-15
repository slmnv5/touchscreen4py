#ifndef TOUCBUFF_H
#define TOUCBUFF_H

#include "pch.hpp"

#include "FrameBuffer.hpp"
#include "TouchScreen.hpp"
#include "lib/utils.hpp"
#include "lib/log.hpp"

std::vector<std::string> splitString(std::string s, uint screenWidth, char chrDelim)
{
    std::vector<std::string> resultVector;
    std::vector<std::string> tokens1 = splitString(s.c_str(), chrDelim);
    for (auto element1 : tokens1)
    {
        std::string oneLine;
        std::vector<std::string> tokens2 = splitString(element1.c_str(), ' ');
        for (std::string element2 : tokens2)
        {
            if (oneLine.size() + element2.size() + 1 > screenWidth)
            {
                oneLine += chrDelim;
                break;
            }
            oneLine += ' ' + element2;
        }
        resultVector.push_back(oneLine);
    }
    return resultVector;
}

class TouchScreenPy : public TouchScreen
{

public:
    TouchScreenPy(int fbId = 1) : TouchScreen(fbId, false, true)
    {
        LOG::ReportingLevel() = LogLvl::ERROR;
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
        const char LINE_DELIMTER = 10;
        uint screenWidth = mFrameBuffer.mPixelsX / mFrameBuffer.mFont.width;
        mTextLines = splitString(text, screenWidth, LINE_DELIMTER);
        mFrameBuffer.clearScr();
        LOG(LogLvl::DEBUG) << text << ", lines: " << mTextLines.size();
        uint i = 0;
        for (auto line : mTextLines)
        {
            unsigned short color = COLOR_INDEX::WHITE;
            if (line.rfind("*", 0) == 0)
            {
                color = mIsRec ? COLOR_INDEX::RED : COLOR_INDEX::GREEN;
            }
            else if (line.rfind("~", 0) == 0)
            {
                color = COLOR_INDEX::YELLOW;
            }
            this->mFrameBuffer.putString(0, i++ * mFrameBuffer.mFont.height, line.c_str(), color);
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