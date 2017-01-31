#pragma once

namespace global
{
	class timer
	{
	public:
		static void initializeTimer(float t);
		static void updateTimer(float t);
		static float mDt;
		static float mFPS;
	private:
		timer();
		~timer();	
		static float mPreviousTime;
		
	};
}
