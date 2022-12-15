
#include "TouchScreenPy.hpp"
#include "lib/utils.hpp"

std::vector<std::string> splitString(std::string s, uint screenWidth, char chrDelim)
{
    std::vector<std::string> resultVector;
    std::vector<std::string> tokens1 = splitString(s.c_str(), chrDelim);
    for (auto element1 : tokens1)
    {
        std::vector<std::string> tokens2 = splitString(element1.c_str(), ' ');
        std::string tmp;
        for (std::string element2 : tokens2)
        {
            uint sz = element2.size();
            if (sz == 0)
                continue;
            if (sz > screenWidth)
                element2 = element2.substr(0, screenWidth);
            sz += tmp.size();
            if (sz == screenWidth)
            {
                resultVector.push_back(tmp + element2);
                tmp = "";
                continue;
            }
            if (sz > screenWidth)
            {
                resultVector.push_back(tmp);
                tmp = element2 + ' ';
                continue;
            }
            tmp += element2 + ' ';
        }
        resultVector.push_back(tmp);
    }
    return resultVector;
}

std::string TouchScreenPy::getClickEvent()
{
    auto pairColRow = this->getClickEventColRow();
    auto line = this->mTextLines.at(pairColRow.second);
    auto word = wordAtPosition(line, pairColRow.first, '[', ']');
    LOG(LogLvl::DEBUG) << line << ", word: " << word;
    return (word.length() > 0) ? word : "";
}

void TouchScreenPy::setText(const char *text)
{
    const char LINE_DELIMTER = 10;
    uint screenWidth = mFrameBuffer.mPixelsX / mFrameBuffer.mFont.width;
    mTextLines = splitString(text, screenWidth, LINE_DELIMTER);
    mFrameBuffer.clearScr();
    LOG(LogLvl::DEBUG) << text << ", lines: " << mTextLines.size();
    for (uint i = 0; i < mTextLines.size(); i++)
    {
        auto line = mTextLines.at(i);
        unsigned short color = COLOR_INDEX::LTGREY;
        if (i == 0)
        {
            color = COLOR_INDEX::WHITE;
        }
        else if (line.rfind("*", 0) == 0)
        {
            color = mIsRec ? COLOR_INDEX::RED : COLOR_INDEX::LTGREEN;
        }
        else if (line.rfind("~", 0) == 0)
        {
            color = COLOR_INDEX::YELLOW;
        }
        this->mFrameBuffer.putString(0, i++ * mFrameBuffer.mFont.height, line.c_str(), color);
    }
}

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

    int setLogLevel(int lvl)
    {
        try
        {
            LOG::ReportingLevel() = static_cast<LogLvl>(lvl);
            return 0;
        }
        catch (...)
        {
            return -1;
        }
    }
}
