#pragma once

namespace global
{
	class timer
	{
	public:
		static void initializeTimer(double t);
		static void updateTimer(double t);
		static double mDt;
		static double mFPS;
	private:
		timer();
		~timer();	
		static double mPreviousTime;
		
	};
}
