
#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include "touch.h"
#include "touch.c"
#include "framebuffer.c"
#include <signal.h>

#define SAMPLE_AMOUNT 2

void INThandler(int sig)
{
	signal(sig, SIG_IGN);
	closeFramebuffer();
	exit(0);
}

int main()
{
	signal(SIGINT, INThandler);

	int xres, yres, x;

	int Xsamples[20];
	int Ysamples[20];

	int screenXmax, screenXmin;
	int screenYmax, screenYmin;

	float scaleXvalue, scaleYvalue;

	int rawX, rawY, rawPressure, scaledX, scaledY;

	int Xaverage = 0;
	int Yaverage = 0;

	if (openTouchScreen() == 1)
		perror("error opening touch screen");

	getTouchScreenDetails(&screenXmin, &screenXmax, &screenYmin, &screenYmax);

	framebufferInitialize(&xres, &yres);

	scaleXvalue = ((float)screenXmax - screenXmin) / xres;
	printf("X Scale Factor = %f\n", scaleXvalue);
	scaleYvalue = ((float)screenYmax - screenYmin) / yres;
	printf("Y Scale Factor = %f\n", scaleYvalue);

	int h;

	int sample;

	while (1)
	{
		for (sample = 0; sample < SAMPLE_AMOUNT; sample++)
		{
			getTouchSample(&rawX, &rawY, &rawPressure);
			Xsamples[sample] = rawX;
			Ysamples[sample] = rawY;
		}

		Xaverage = 0;
		Yaverage = 0;

		for (x = 0; x < SAMPLE_AMOUNT; x++)
		{
			Xaverage += Xsamples[x];
			Yaverage += Ysamples[x];
		}

		Xaverage = Xaverage / SAMPLE_AMOUNT;
		Yaverage = Yaverage / SAMPLE_AMOUNT;

		scaledX = Xaverage / scaleXvalue;
		scaledY = Yaverage / scaleYvalue;
	}
}
