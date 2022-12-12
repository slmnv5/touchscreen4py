#ifndef TOUCHSCR_H
#define TOUCHSCR_H

#include <linux/input.h>

#include "pch.hpp"
#include "FrameBuffer.hpp"
#include "lib/SafeQueue.hpp"

#define MAX_QUEUE_SZ 20    // used to stop putting events in touch screen queue
#define MIN_TOUCH_TIME 0.2 // min time to hold button for click in seconds

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

//============================================================

class TouchScreen
{

public:
    FrameBuffer mFrameBuffer;
    bool mStopped = false;

protected:
    std::vector<std::string> mTextLines;         // text on top of screen
    SafeQueue<std::pair<double, double>> mQueue; // queue for click events <col, row>
    double mLoopSeconds = 0;                     // loop length in seconds

private:
    int mFdScr;                // file descriptor of touch screen
    uint mMinX, mMinY, mMinP;  // min values for X, Y, P
    uint mMaxX, mMaxY, mMaxP;  // max values for X, Y, P
    const bool mInvertX;       // Invert touch screen X
    const bool mInvertY;       // Invert touch screen Y
    std::thread mReadThread;   // Thread read toch events
    std::thread mUpdateThread; // Thread draw updates

public:
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
        mReadThread = std::thread(&TouchScreen::readScreen, this);
        mUpdateThread = std::thread(&TouchScreen::updateScreen, this);
    }
    virtual ~TouchScreen()
    {
        mStopped = true;
        mReadThread.join();
        mUpdateThread.join();
        close(mFdScr);
    }

    void updateScreen()
    {
        LOG(LogLvl::INFO) << "Starting updateScreen()";
        double pos = 0.0;
        unsigned short color = idxToColor(COLOR_INDEX::YELLOW);
        while (!mStopped)
        {
            usleep(1000000);
            pos += 1 / 16.0;
            pos = pos - floor(pos);
            uint startX = pos * mFrameBuffer.mPixelsX;
            mFrameBuffer.putSquareColor(startX, 0, 16, 32, color);
        }
    }

    void readScreen()
    {
        LOG(LogLvl::INFO) << "Starting readScreen()";
        uint x, y, savex, savey;
        x = y = savex = savey = 0;
        auto started = myclock::now();
        bool touch_on = false;
        bool button_click = false;
        double scaleX = 1.0 / (mMaxX - mMinX) * mFrameBuffer.mPixelsX;
        double scaleY = 1.0 / (mMaxY - mMinY) * mFrameBuffer.mPixelsY;

        struct input_event ev;

        while (read(mFdScr, &ev, sizeof(struct input_event)) != -1)
        {
            if (mStopped)
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
                    bool sameX = abs(x - savex + 0.0) / (mMaxX - mMinX) < 0.1;
                    bool sameY = abs(y - savey + 0.0) / (mMaxY - mMinY) < 0.1;
                    seconds duration = myclock::now() - started;
                    if (duration.count() > MIN_TOUCH_TIME and sameX and sameY)
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

                uint col = x / mFrameBuffer.mFont.width;
                uint row = y / mFrameBuffer.mFont.height;
                LOG(LogLvl::DEBUG) << "Click event at col, row: " << col << ", " << row;
                mQueue.push(std::pair<uint, uint>(col, row));
            }
        }
    }

private:
    void getInfoFromDevice(int propId, uint &minV, uint &maxV)
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
