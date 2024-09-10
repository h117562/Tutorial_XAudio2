#include "FrameTimer.h"


FrameTimer::FrameTimer()
{
	m_frequency = 0;
	m_startTime = 0;
	m_frameTime = 0.0f;
	m_ticksPerMs = 0.0f;
}


FrameTimer::FrameTimer(const FrameTimer& other)
{
}


FrameTimer::~FrameTimer()
{
}

bool FrameTimer::Initialize()
{
	//성능 카운터 빈도(초당 개수)를 가져옴
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
	if (m_frequency == 0)
	{
		return false;
	}

	//ms로 단위 변환
	m_ticksPerMs = (float)(m_frequency / 1000);

	//현재 카운터 빈도를 저장
	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);

	return true;
}

void FrameTimer::Frame()
{
	INT64 currentTime;
	INT64 elapsedTicks;


	//현재 빈도 저장
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	//이전 빈도와 현재의 차이 계산
	elapsedTicks = currentTime - m_startTime;

	//프레임 시간 계산
	m_frameTime = (float)elapsedTicks / m_frequency;

	//현재 카운터 빈도 초기화
	m_startTime = currentTime;

	return;
}

float FrameTimer::GetTime()
{
	return m_frameTime;
}
