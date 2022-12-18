

#include "TouchScreen.hpp"
#include "lib/log.hpp"

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

TouchScreen::TouchScreen(uint fbId, bool invx, bool invy) : FrameBuffer(fbId), mInvertX(invx), mInvertY(invy)
{

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
    mScaleX = 1.0 / (mMaxX - mMinX) * mPixelsX;
    mScaleY = 1.0 / (mMaxY - mMinY) * mPixelsY;

    mUpdateThread = std::thread(&TouchScreen::updateScreen, this);
    LOG(LogLvl::INFO) << "Opened touch screen device: " << name
                      << ", X: " << mMinX << "--" << mMaxX << ", Y: " << mMinY << "--" << mMaxY;
}

void TouchScreen::updateScreen()
{
    LOG(LogLvl::INFO) << "Starting updateScreen()";
    uint pos = 0;
    while (true)
    {
        usleep(mLoopSeconds * 1000000 / 16);
        if (mIsStop)
        {
            continue;
        }
        mLoopPosition += 1.0 / 16;
        if (mLoopPosition > 1)
            mLoopPosition -= 1.0;

        pos = mLoopPosition * mPixelsX;
        putSquare(0, 0, pos, 2, YELLOW);
        putSquare(pos, 0, mPixelsX - pos, 2, BLACK);
        uint sz = mFont.height;
        putSquare(0, sz - 2, pos, 2, YELLOW);
        putSquare(pos, sz - 2, mPixelsX - pos, 2, BLACK);
    }
}

std::string TouchScreen::getClickEventWord()
{

    uint x, y, savex, savey;
    x = y = savex = savey = 0;
    auto started = myclock::now();
    bool touch_on = false;
    bool button_click = false;
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

        if (button_click)
        {
            button_click = false;
            x = mInvertX ? mMaxX - x : x;
            y = mInvertY ? mMaxY - y : y;
            x = (x - mMinX) * mScaleX;
            y = (y - mMinY) * mScaleY;
            uint col = x / mFont.width;
            uint row = y / mFont.height;

            auto line = mRows.at(row);
            auto word = wordAtPosition(line, x, '[', ']');
            if (word.length() == 0)
                continue;
            putSquareInv(0, row * mFont.height, mPixelsX, mFont.height);
            usleep(MIN_TOUCH_TIME * 10E6);
            putSquareInv(0, row * mFont.height, mPixelsX, mFont.height);
            LOG(LogLvl::DEBUG) << "Click event at col, row: " << col << ", " << row << ", word: " << word;
            return word;
        }
    }
    return "";
}

void TouchScreen::getInfoFromDevice(int propId, uint &minV, uint &maxV)
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
