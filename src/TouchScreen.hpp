#ifndef TOUCHSCR_H
#define TOUCHSCR_H

#include <linux/input.h>

#include "pch.hpp"
#include "FrameBuffer.hpp"

static const float MIN_TOUCH_TIME = 0.15; // min time to hold button for click in seconds

std::string findTouchscrEvent();

std::string wordAtPosition(const std::string &s, size_t pos);

class TouchScreen : public FrameBuffer
{

public:
    bool mStopped = false;

protected:
    double mLoopSeconds = 1.0;  // loop length in seconds
    double mLoopPosition = 0.0; // loop postion 0 to 1
    bool mIsRec = false;        // is recording
    bool mIsStop = true;        // is stopped

private:
    int mFdScr;                // file descriptor of touch screen
    uint mMinX, mMinY, mMinP;  // min values for X, Y, P
    uint mMaxX, mMaxY, mMaxP;  // max values for X, Y, P
    const bool mInvertX;       // Invert touch screen X
    const bool mInvertY;       // Invert touch screen Y
    std::thread mUpdateThread; // Thread draw updates
    double mScaleX;            // scale for touch scren values
    double mScaleY;            // scale for touch scren values

public:
    TouchScreen(uint fbId, bool invx, bool invy);
    virtual ~TouchScreen()
    {
        mStopped = true;
        mUpdateThread.join();
        close(mFdScr);
    }

    void updateScreen();

    std::string getClickEventWord();

private:
    void getInfoFromDevice(int propId, uint &minV, uint &maxV);
};

#endif
