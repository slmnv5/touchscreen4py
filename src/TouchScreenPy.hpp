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

    const char *getClickEvent()
    {
        auto pair = this->mQueue.pop();
        if (pair.first < this->mTextLines.size())
        {
            auto line = this->mTextLines.at(pair.first);
            auto word = wordAtPosition(line, pair.second, '[', ']');
            if (word.length() > 0)
                return word.c_str();
        }
        return "";
    }

    void setText(const char *text)
    {
        this->mTextLines = split_string(text, "\n");
        for (uint i = 0; i < this->mTextLines.size(); i++)
        {
            this->mFrameBuffer.putString(32 * (1 + i), 0, text, COLOR_INDEX_T::WHITE);
        }
    }

    void setHeader(const char *header, double loopSeconds)
    {
        this->mHeaderLine = header;
        this->mLoopSeconds = loopSeconds;
    }
};

extern "C"
{

    void *createTouchScreen()
    {
        return new (std::nothrow) TouchScreenPy;
    }

    void deleteTchScr(void *ptr)
    {
        TouchScreenPy *x = static_cast<TouchScreenPy *>(ptr);
        delete x;
    }

    int stop(void *ptr)
    {
        TouchScreenPy *x = static_cast<TouchScreenPy *>(ptr);
        x->mStopped = true;
        return 0;
    }

    int start(void *ptr)
    {
        TouchScreenPy *x = static_cast<TouchScreenPy *>(ptr);
        x->mStopped = false;
        return 0;
    }

    const char *getClickEvent(void *ptr)
    {
        try
        {
            TouchScreenPy *x = static_cast<TouchScreenPy *>(ptr);
            auto pair = x->mQueue.pop();
            return x->getClickEvent();
        }
        catch (...)
        {
            return "-1";
        }
    }

    int setHeader(void *ptr, const char *header, float loopSeconds)
    {
        try
        {
            TouchScreenPy *x = static_cast<TouchScreenPy *>(ptr);
            x->setHeader(header, loopSeconds);
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