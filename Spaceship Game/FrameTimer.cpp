#include "FrameTimer.h"

void FrameTimer::init(int fps)
{
	QueryPerformanceFrequency(&timerFreq);
	QueryPerformanceCounter(&timeNow);
	QueryPerformanceCounter(&timePrevious);

	requestedFPS = fps;

	intervalsPerFrame = timerFreq.QuadPart / requestedFPS;
}

int FrameTimer::FramesToUpdate()
{
	int framesToUpdate = 0;
	QueryPerformanceCounter(&timeNow);

	intervalsSinceLastUpdate = timeNow.QuadPart - timePrevious.QuadPart;

	framesToUpdate = (int)(intervalsSinceLastUpdate / intervalsPerFrame);

	if (framesToUpdate != 0)
	{
		QueryPerformanceCounter(&timePrevious);
	}
	return framesToUpdate;
}
