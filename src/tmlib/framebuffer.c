
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/mman.h>

static struct fb_var_screeninfo var;
char *fbp = 0;
int fb = 0;
long int screensize = 0;

int framebufferInitialize(int *xres, int *yres)
{
	char *fbdevice = "/dev/fb1";

	fb = open(fbdevice, O_RDWR);
	if (fb == -1)
	{
		perror("open fbdevice");
		return -1;
	}

		if (ioctl(fb, FBIOGET_VSCREENINFO, &var) < 0)
	{
		perror("ioctl FBIOGET_VSCREENINFO");
		close(fb);
		return -1;
	}

	printf("Original %dx%d, %dbpp\n", var.xres, var.yres,
		   var.bits_per_pixel);

	printf("%dx%d, %d bpp\n\n\n", var.xres, var.yres, var.bits_per_pixel);

	*xres = var.xres;
	*yres = var.yres;
	close(fb);
}
