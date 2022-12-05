// #include "pch.hpp"
#include <iostream>
#include <fcntl.h>
#include <linux/fb.h>

int framebufferInitialize(int *xres, int *yres)
{
	struct fb_var_screeninfo var;
	char *fbp = 0;
	int fb = 0;
	long int screensize = 0;

	fb = open("/dev/fb1", O_RDONLY);
	if (fb == -1)
	{
		throw new std::runtime_error("Fail to open fbdevice");
	}

	if (ioctl(fb, FBIOGET_VSCREENINFO, &var) < 0)
	{
		close(fb);
		throw new std::runtime_error("Fail ioctl FBIOGET_VSCREENINFO");
	}

	*xres = var.xres;
	*yres = var.yres;
	close(fb);
}
