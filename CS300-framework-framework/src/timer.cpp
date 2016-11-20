#include "timer.h"

namespace global
{
	double timer::mDt;
	double timer::mPreviousTime;
	double timer::mFPS;

	timer::timer()
	{
	}

	timer::~timer()
	{
	}

	void timer::initializeTimer(double t)
	{
		mDt = 0.0;
		mPreviousTime = t;
		mFPS = 0.0;
	}

	void timer::updateTimer(double t)
	{
		double currentTIme = t;
		mDt = currentTIme - mPreviousTime;
		mFPS = 1.0 / mDt;
		mPreviousTime = currentTIme;
	}
}

