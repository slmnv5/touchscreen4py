#ifndef TOUCBUFF_H
#define TOUCBUFF_H

#include "pch.hpp"

#include "FrameBuffer.hpp"
#include "TouchScreen.hpp"
#include "lib/utils.hpp"

class TouchScreenPy : public TouchScreen
{
private:
    std::thread run_thread;

public:
    std::vector<std::string> text_lines;

    TouchScreenPy() : TouchScreen(false, true)
    {
        run_thread = std::thread(TouchScreen::run, this);
    }
    virtual ~TouchScreenPy()
    {
        run_thread.join();
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

    const char *get_event(void *ptr)
    {
        try
        {
            TouchScreenPy *x = static_cast<TouchScreenPy *>(ptr);
            auto pair = x->getClickPosition();
            if (pair.first < x->text_lines.size())
            {
                auto line = x->text_lines.at(pair.first);
                auto word = wordAtPosition(line, pair.second, '[', ']');
                if (word.length() > 0)
                    return word.c_str();
            }
            return "";
        }
        catch (...)
        {
            return "-1";
        }
    }

    int setText(void *ptr, const char *text)
    {
        try
        {
            TouchScreenPy *x = static_cast<TouchScreenPy *>(ptr);
            x->text_lines = split_string(text, "\n");
            for (int i = 0; i < x->text_lines.size(); i++)
            {
                x->mFrameBuffer.putString(32 * (1 + i), 0, text, COLOR_INDEX_T::WHITE);
            }
            return 0;
        }
        catch (...)
        {
            return -1;
        }
    }
}

#endif