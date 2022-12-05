#ifndef TOUCHUTILS_H
#define TOUCHUTILS_H

#include <linux/input.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>

#define KWHT "\x1B[37m"
#define KYEL "\x1B[33m"

#define EVENT_DEVICE "/dev/input/event0"
#define EVENT_TYPE EV_ABS
#define EVENT_CODE_X ABS_X
#define EVENT_CODE_Y ABS_Y

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

// return screen file descr. and details
int getTouchScreenDetails2(int *screenXmin, int *screenXmax, int *screenYmin, int *screenYmax)

{

    int fd;
    if ((fd = open("/dev/input/event0", O_RDONLY | O_NONBLOCK)) < 0)
    {
        std::cout << "Could not open device file. Try running as root." << std::endl;
        return -1;
    }

    const char *absval[6] = {"Value", "Min", "Max", "Fuzz", "Flat", "Resolution"};
    int absX[6] = {};
    int absY[6] = {};

    ioctl(fd, EVIOCGABS(ABS_MT_POSITION_X), absX);
    ioctl(fd, EVIOCGABS(ABS_MT_POSITION_Y), absY);

    cout << "ABS_MT_POSITION_X Properties" << std::endl;
    for (int x = 0; x < 6; x++)
    {
        if ((x < 3) || absX[x])
        {
            cout << absval[x] << ": " << absX[x] << std::endl;
        }
    }

    cout << "ABS_MT_POSITION_Y Properties" << std::endl;
    for (int y = 0; y < 6; y++)
    {
        if ((y < 3) || absX[y])
        {
            cout << absval[y] << ": " << absY[y] << std::endl;
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
        const ssize_t ev_size = sizeof(struct input_event);

        auto size = read(fd, &ev, ev_size);
        if (size < ev_size)
        {
            fprintf(stderr, "Error size when reading\n");
            return;
        }

        if (ev.type == EVENT_TYPE && (ev.code == EVENT_CODE_X || ev.code == EVENT_CODE_Y))
        {
            printf("%s = %d\n", ev.code == EVENT_CODE_X ? "X" : "Y",
                   ev.value);
        }
    }
}
#endif
