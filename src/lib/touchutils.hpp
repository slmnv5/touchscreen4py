#ifndef TOUCHUTILS_H
#define TOUCHUTILS_H

#include <linux/input.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>

#include "tmlib/touch.h"

#define KWHT "\x1B[37m"
#define KYEL "\x1B[33m"

int framebufferInitialize(int *xres, int *yres)
{
    *xres = *yres = -1;
    struct fb_var_screeninfo var;
    int fb = 0;
    fb = open("/dev/fb1", O_RDONLY | O_NONBLOCK);
    if (fb == -1)
    {
        return -1;
    }

    if (ioctl(fb, FBIOGET_VSCREENINFO, &var) < 0)
    {
        close(fb);
        return -1;
    }

    *xres = var.xres;
    *yres = var.yres;
    close(fb);
    return 0;
}

void getTouchSample(int fd, int *rawX, int *rawY, int *rawPressure)
{
    int i;
    /* how many bytes were read */
    size_t rb;
    /* the events (up to 64 at once) */
    struct input_event ev[64];

    rb = read(fd, ev, sizeof(struct input_event) * 64);
    for (i = 0; i < (rb / sizeof(struct input_event)); i++)
    {
        if (ev[i].type == EV_SYN)
            printf("Event type is %s%s%s = Start of New Event\n", KYEL, events[ev[i].type], KWHT);

        else if (ev[i].type == EV_KEY && ev[i].code == 330 && ev[i].value == 1)
            printf("Event type is %s%s%s & Event code is %sTOUCH(330)%s & Event value is %s1%s = Touch Starting\n", KYEL, events[ev[i].type], KWHT, KYEL, KWHT, KYEL, KWHT);

        else if (ev[i].type == EV_KEY && ev[i].code == 330 && ev[i].value == 0)
            printf("Event type is %s%s%s & Event code is %sTOUCH(330)%s & Event value is %s0%s = Touch Finished\n", KYEL, events[ev[i].type], KWHT, KYEL, KWHT, KYEL, KWHT);

        else if (ev[i].type == EV_ABS && ev[i].code == 0 && ev[i].value > 0)
        {
            printf("Event type is %s%s%s & Event code is %sX(0)%s & Event value is %s%d%s\n", KYEL, events[ev[i].type], KWHT, KYEL, KWHT, KYEL, ev[i].value, KWHT);
            *rawX = ev[i].value;
        }
        else if (ev[i].type == EV_ABS && ev[i].code == 1 && ev[i].value > 0)
        {
            printf("Event type is %s%s%s & Event code is %sY(1)%s & Event value is %s%d%s\n", KYEL, events[ev[i].type], KWHT, KYEL, KWHT, KYEL, ev[i].value, KWHT);
            *rawY = ev[i].value;
        }
        else if (ev[i].type == EV_ABS && ev[i].code == 24 && ev[i].value > 0)
        {
            printf("Event type is %s%s%s & Event code is %sPressure(24)%s & Event value is %s%d%s\n", KYEL, events[ev[i].type], KWHT, KYEL, KWHT, KYEL, ev[i].value, KWHT);
            *rawPressure = ev[i].value;
        }
    }
}

// return screen file descr. and details
int getTouchScreenDetails2(int *screenXmin, int *screenXmax, int *screenYmin, int *screenYmax)

{

    int fd;
    if ((fd = open("/dev/input/event0", O_RDONLY | O_NONBLOCK)) < 0)
    {
        cout << "Could not open device file. Try running as root." << endl;
        return -1;
    }

    char *absval[6] = {"Value", "Min", "Max", "Fuzz", "Flat", "Resolution"};
    int absX[6] = {};
    int absY[6] = {};

    ioctl(fd, EVIOCGABS(ABS_MT_POSITION_X), absX);
    ioctl(fd, EVIOCGABS(ABS_MT_POSITION_Y), absY);

    cout << "ABS_MT_POSITION_X Properties" << endl;
    for (int x = 0; x < 6; x++)
    {
        if ((x < 3) || absX[x])
        {
            cout << absval[x] << ": " << absX[x] << endl;
        }
    }

    cout << "ABS_MT_POSITION_Y Properties" << endl;
    for (int y = 0; y < 6; y++)
    {
        if ((y < 3) || absX[y])
        {
            cout << absval[y] << ": " << absY[y] << endl;
        }
    }

    *screenXmin = absX[1];
    *screenXmax = absX[2];
    *screenYmin = absY[1];
    *screenYmax = absY[2];

    // auto SCALE_X = (1920.0f / absX[2]);
    // auto SCALE_Y = (1080.0f / absY[2]);

    return fd;
}

void getTouchSample2(int fd, int *rawX, int *rawY, int *rawPressure)
{

    struct input_event ev;
    char name[256] = "Unknown";

    ioctl(fd, EVIOCGNAME(sizeof(name)), name);
    printf("device file = %s\n", EVENT_DEVICE);
    printf("device name = %s\n", name);

    while (true)
    {
        const size_t ev_size = sizeof(struct input_event);
        ssize_t size;

        size = read(fd, &ev, ev_size);
        if (size < ev_size)
        {
            fprintf(stderr, "Error size when reading\n");
            return 1;
        }

        if (ev.type == EVENT_TYPE && (ev.code == EVENT_CODE_X || ev.code == EVENT_CODE_Y))
        {
            printf("%s = %d\n", ev.code == EVENT_CODE_X ? "X" : "Y",
                   ev.value);
        }
    }
}
#endif
