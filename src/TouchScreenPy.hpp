#ifndef TOUCBUFF_H
#define TOUCBUFF_H

#include "pch.hpp"

#include "FrameBuffer.hpp"
#include "TouchScreen.hpp"
#include "lib/utils.hpp"

class TouchScreenPy : public TouchScreen
{

public:
    TouchScreenPy() : TouchScreen(false, true)
    {
    }
    virtual ~TouchScreenPy()
    {
    }

    std::string getClickEvent()
    {
        while (this->mTextLines.size())
        {
            auto pairColRow = this->mQueue.pop();
            if (pairColRow.second >= this->mTextLines.size())
                continue;

            auto line = this->mTextLines.at(pairColRow.second);
            auto word = wordAtPosition(line, pairColRow.first, '[', ']');
            if (word.length() > 0)
                return word;
        }
        return "";
    }

    void setText(const char *text)
    {
        this->mTextLines = split_string(text, "\n");
        for (uint i = 0; i < this->mTextLines.size(); i++)
        {
            auto line = this->mTextLines.at(i);
            this->mFrameBuffer.putStringNice(0, 32 * i, line.c_str());
        }
    }

    void setContent(const char *content)
    {
        uint row_offset = mTextLines.size();
        this->mContentLines = split_string(content, "\n");
        for (uint i = 0; i < this->mContentLines.size(); i++)
        {
            auto line = this->mTextLines.at(i);
            unsigned short color = WHITE;
            if (line.rfind("*", 0) == 0)
            {
                color = mIsRec ? RED : GREEN;
            }
            else if (line.rfind("~", 0) == 0)
            {
                color = YELLOW;
            }
            this->mFrameBuffer.putString(0, row_offset + 32 * i, line.c_str(), color);
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

    void clear(void *ptr)
    {
        TouchScreenPy *x = static_cast<TouchScreenPy *>(ptr);
        x->mFrameBuffer.clear();
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
}

#endif