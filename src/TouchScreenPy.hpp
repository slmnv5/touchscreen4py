#ifndef TOUCBUFF_H
#define TOUCBUFF_H

#include "pch.hpp"
#include "TouchScreen.hpp"

std::vector<std::string> splitString(std::string s, uint screenWidth, char chrDelim);

class TouchScreenPy : public TouchScreen
{

public:
    TouchScreenPy(int fbId) : TouchScreen(fbId, false, true)
    {
    }
    virtual ~TouchScreenPy()
    {
    }
    std::string clickWord = "";
    void setLoop(double loopSeconds, double loopPosition, bool isRec, bool isStop)
    {
        this->mLoopSeconds = loopSeconds;
        this->mLoopPosition = loopPosition;
        this->mIsRec = isRec;
        this->mStopped = isStop;
    }
};

extern "C"
{

    void *createTouchScreen(int fbId);

    void deleteTouchScreen(void *ptr);

    void stop(void *ptr);

    void start(void *ptr);

    const char *getClickEventWord(void *ptr);

    const char *testStr(void *ptr);

    int setLoop(void *ptr, double loopSeconds, double loopPosition, bool isRec, bool isStop);

    int clearScreen(void *ptr);

    int setRowText(void *ptr, uint row, const char *text, uint r, uint g, uint b);

    int setLogLevel(int lvl);

    int getCols(void *ptr);

    int getRows(void *ptr);

    int getRows(void *ptr);
    
    int putSquare(void *ptr, uint x, uint y, uint width, uint height, uint r, uint g, uint b);

    int putSquareInv(void *ptr, uint x, uint y, uint width, uint height);
}

#endif
