#ifndef _FRAMETIMER_H_
#define _FRAMETIMER_H_

#include <windows.h>

class FrameTimer
{
public:
	FrameTimer();
	FrameTimer(const FrameTimer&);
	~FrameTimer();

	bool Initialize();
	void Frame();

	float GetTime();

private:
	INT64 m_frequency;
	INT64 m_startTime;
	float m_frameTime;
	float m_ticksPerMs;
};


#endif