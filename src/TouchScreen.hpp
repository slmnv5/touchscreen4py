#ifndef TOUCHSCR_H
#define TOUCHSCR_H

#include <linux/input.h>

#include "FrameBuffer.hpp"
#include "pch.hpp"

#define MAX_QUEUE_SZ 20
#define MKS_QUEUE_SLEEP 100000

using myclock = std::chrono::steady_clock;
using seconds = std::chrono::duration<double>;

std::string findTouchscrEvent()
{
    const char *cmd = "grep -E 'Handlers|EV=' /proc/bus/input/devices | "
                      "grep -B1 'EV=b' | grep -Eo 'event[0-9]+' | grep -Eo '[0-9]+' | tr -d '\n'";

    FILE *pipe = popen(cmd, "r");
    char buffer[128];
    std::string result = "";
    while (!feof(pipe))
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    pclose(pipe);
    return result;
}

std::string wordAtPosition(const std::string &s, unsigned int pos, char leftSpace, char rightSpace)
{
    unsigned int maxPos = s.length();

    unsigned int start, stop;
    start = stop = pos;
    while (start < maxPos and s.at(start) != leftSpace)
        start--;
    while (stop < maxPos and s.at(stop) != rightSpace)
        stop++;
    if (stop >= maxPos or start >= maxPos)
        return "";

    auto word = s.substr(start, stop - start + 1);
    return word;
}
class TouchScreen
{
private:
    int mFdScr;
    int mMinX, mMinY, mMinP;
    int mMaxX, mMaxY, mMaxP;
    std::queue<std::pair<float, float>> mQueue;
    const bool mInvertX;
    const bool mInvertY;

public:
    bool stopped = false;
    FrameBuffer mFrameBuffer;

    TouchScreen(bool invx, bool invy) : mFrameBuffer(), mInvertX(invx), mInvertY(invy)
    {

        if (mFrameBuffer.mPixelsX <= 0 || mFrameBuffer.mPixelsY <= 0)
        {
            throw std::runtime_error("Screen resolution must be positive");
        }
        std::string dev_id = findTouchscrEvent();
        if ("" == dev_id)
        {
            throw std::runtime_error("Cannot find touch screen device");
        }
        std::string fname = "/dev/input/event" + dev_id;
        mFdScr = open(fname.c_str(), O_RDONLY);
        if (mFdScr < 0)
        {
            throw std::runtime_error("Could not open touch screen device file: " + fname);
        }
        char name[256] = "Unknown";
        ioctl(mFdScr, EVIOCGNAME(sizeof(name)), name);
        getInfoFromDevice(ABS_X, mMinX, mMaxX);
        getInfoFromDevice(ABS_Y, mMinY, mMaxY);
        getInfoFromDevice(ABS_PRESSURE, mMinP, mMaxP);
        LOG(LogLvl::INFO) << "Opened touch screen device: " << name
                          << ", X: " << mMinX << "--" << mMaxX << ", Y: " << mMinY << "--" << mMaxY;
    }
    virtual ~TouchScreen()
    {
        stopped = true;
        close(mFdScr);
    }

    void run()
    {
        LOG(LogLvl::INFO) << "========= Starting run =========";
        int x, y, savex, savey;
        x = y = savex = savey = 0;
        auto started = myclock::now();
        int touch_on = 0;
        bool button_click = false;
        float scaleX = 1.0 / (mMaxX - mMinX) * mFrameBuffer.mPixelsX;
        float scaleY = 1.0 / (mMaxY - mMinY) * mFrameBuffer.mPixelsY;

        struct input_event ev;

        while (read(mFdScr, &ev, sizeof(struct input_event)) != -1)
        {
            if (stopped)
                break;

            if (ev.type == EV_KEY && ev.code == BTN_TOUCH)
            {
                touch_on = ev.value;
                if (touch_on)
                {
                    savex = x;
                    savey = y;
                    started = myclock::now();
                }
                else
                {
                    seconds duration = myclock::now() - started;
                    if (duration.count() > 0.5 && abs(x - savex) / (mMaxX - mMinX) < 0.1 &&
                        abs(y - savey) / (mMaxY - mMinY) < 0.1)
                    {
                        button_click = true;
                    }
                }
            }
            else if (ev.type == EV_ABS && ev.code == ABS_X)
            {
                x = ev.value;
            }
            else if (ev.type == EV_ABS && ev.code == ABS_Y)
            {
                y = ev.value;
            }
            else
            {
                continue;
            }
            if (button_click and mQueue.size() < MAX_QUEUE_SZ)
            {
                button_click = false;
                x = mInvertX ? mMaxX - x : x;
                y = mInvertY ? mMaxY - y : y;
                x = (x - mMinX) * scaleX;
                y = (y - mMinY) * scaleY;

                mFrameBuffer.draw_square(x, y, 15, 15, COLOR_INDEX_T::WHITE);
                int col = x / mFrameBuffer.mFont.width;
                int row = y / mFrameBuffer.mFont.height;
                LOG(LogLvl::DEBUG) << "Click event at col, row: " << col << ", " << row;
                mQueue.push(std::pair<int, int>(col, row));
            }
        }
    }

    const std::pair<int, int> &getClickPosition()
    {
        if (mQueue.empty())
        {
            usleep(MKS_QUEUE_SLEEP);
        }
        std::pair<int, int> pos = mQueue.front();
        mQueue.pop();
        return pos;
    }

private:
    void getInfoFromDevice(int propId, int &minV, int &maxV)
    {
        // const char *arrPropName[6] = {"Value", "Min", "Max", "Fuzz", "Flat", "Resolution"};
        int arrPropValue[6] = {};
        if (ioctl(mFdScr, EVIOCGABS(propId), arrPropValue) < 0)
        {
            throw std::runtime_error("Cannot read touch screen device");
        }
        minV = arrPropValue[1];
        maxV = arrPropValue[2];
        LOG(LogLvl::DEBUG) << "ABS. property: " << propId << ", min: " << minV << ", max:" << maxV;
    }
};

#endif
