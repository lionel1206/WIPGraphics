#include "timer.h"

namespace global
{
	float timer::mDt;
	float timer::mPreviousTime;
	float timer::mFPS;

	timer::timer()
	{
	}

	timer::~timer()
	{
	}

	void timer::initializeTimer(float t)
	{
		mDt = 0.0;
		mPreviousTime = t;
		mFPS = 0.0;
	}

	void timer::updateTimer(float t)
	{
		float currentTIme = t;
		mDt = currentTIme - mPreviousTime;
		mFPS = 1.f / mDt;
		mPreviousTime = currentTIme;
	}
}

