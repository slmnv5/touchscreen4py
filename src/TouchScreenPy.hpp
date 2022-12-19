#ifndef TOUCBUFF_H
#define TOUCBUFF_H

#include "pch.hpp"
#include "TouchScreen.hpp"

std::vector<std::string> splitString(std::string s, uint screenWidth, char chrDelim);

class TouchScreenPy : public TouchScreen
{

public:
    TouchScreenPy(int fbId = 1) : TouchScreen(fbId, false, true)
    {
    }
    virtual ~TouchScreenPy()
    {
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

    void *createTouchScreen();

    void deleteTouchScreen(void *ptr);

    void stop(void *ptr);

    void start(void *ptr);

    const char *getClickEventWord(void *ptr);

    int setLoop(void *ptr, double loopSeconds, double loopPosition, bool isRec, bool isStop);

    int clearScreen(void *ptr, int startY);

    int setRowText(void *ptr, int row, const char *text, int r, int g, int b);

    int setLogLevel(int lvl);
}

#endif