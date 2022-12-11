#ifndef TOUCBUFF_H
#define TOUCBUFF_H

#include "pch.hpp"

#include "FrameBuffer.hpp"
#include "TouchScreen.hpp"
#include "lib/utils.hpp"

class TouchAndBuff
{
private:
    std::thread run_thread;

public:
    TouchScreen ts;
    std::vector<std::string> text_lines;

    TouchAndBuff() : ts(false, true)
    {
        run_thread = std::thread(&TouchAndBuff::run, this);
    }
    virtual ~TouchAndBuff()
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
        ts.run();
    }
};

extern "C"
{

    void *createTchScr()
    {
        return new (std::nothrow) TouchAndBuff;
    }

    void deleteTchScr(void *ptr)
    {
        TouchAndBuff *x = static_cast<TouchAndBuff *>(ptr);
        delete x;
    }

    int stop(void *ptr)
    {
        TouchAndBuff *x = static_cast<TouchAndBuff *>(ptr);
        x->stop();
        return 0;
    }

    int start(void *ptr)
    {
        TouchAndBuff *x = static_cast<TouchAndBuff *>(ptr);
        x->start();
        return 0;
    }

    const char *get_event(void *ptr)
    {
        try
        {
            TouchAndBuff *x = static_cast<TouchAndBuff *>(ptr);
            auto pair = x->ts.get_event();
            if (pair.first < x->text_lines.size())
            {
                auto line = x->text_lines.at(pair.first);
                auto word = word_at_position(line, pair.second, '[', ']');
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
            TouchAndBuff *x = static_cast<TouchAndBuff *>(ptr);
            x->text_lines = split_string(text, "\n");
            for (int i = 0; i < x->text_lines.size(); i++)
            {
                x->ts.fb.put_string(32 * (1 + i), 0, text, COLOR_INDEX_T::WHITE);
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