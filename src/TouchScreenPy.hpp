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
        auto pair = this->mQueue.pop();
        if (pair.second < this->mTextLines.size())
        {
            auto line = this->mTextLines.at(pair.first);
            LOG(LogLvl::INFO) << "Got clickEvent line: " << line;
            auto word = wordAtPosition(line, pair.second, '[', ']');
            return word;
        }
        return "beyond the text";
    }

    void setText(const char *text)
    {
        this->mTextLines = split_string(text, "\n");
        for (uint i = 0; i < this->mTextLines.size(); i++)
        {
            auto line = this->mTextLines.at(i);
            this->mFrameBuffer.putString(0, 32 * i, line.c_str(), COLOR_INDEX::WHITE);
        }
    }

    void setLoopSeconds(double loopSeconds, double position)
    {
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
            return x->getClickEvent().c_str();
        }
        catch (...)
        {
            return "";
        }
    }

    int setLoopSeconds(void *ptr, double loopSeconds, double position)
    {
        try
        {
            TouchScreenPy *x = static_cast<TouchScreenPy *>(ptr);
            x->setLoopSeconds(loopSeconds, position);
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