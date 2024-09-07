#pragma once
#include <Windows.h> //Cannot use same include for all class
class FrameTimer
{
public:
	void init(int fps);
	int FramesToUpdate();

private:
	LARGE_INTEGER timerFreq;
	LARGE_INTEGER timeNow;
	LARGE_INTEGER timePrevious;
	int requestedFPS;
	float intervalsPerFrame;
	float intervalsSinceLastUpdate; //This is delta time
};

