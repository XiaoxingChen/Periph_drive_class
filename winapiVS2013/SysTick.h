#ifndef SYSTICK_H
#define SYSTICK_H
#include <windows.h>
#include <iostream>
class CSysTick
{
	
public:
	/*friend DWORD WINAPI Fun1Proc(LPVOID lpParameter);*/
	CSysTick()
		:TimerFlag_(0), step_time_(0)
	{
		LARGE_INTEGER litmp;
		QueryPerformanceFrequency(&litmp);
		PERFORMENCE_FREQ_ = (double)litmp.QuadPart;
	}

	void Init(double step_time)
	{
		step_time_ = step_time;
		HANDLE hThread_1 = CreateThread(NULL, 0, run, this, 0, NULL);
		CloseHandle(hThread_1);
	} 
	static DWORD WINAPI run(LPVOID lpParameter);
	double Get_PERFORMENCE_FREQ_()
	{
		return PERFORMENCE_FREQ_;
	}
	void Wait_Step()
	{
		while (TimerFlag_ != 1) Sleep(0);
		Reset_TimerFlag_();
	}
	bool Get_TimerFlag_()
	{
		return TimerFlag_;
	}

private:

	void Set_TimerFlag_()
	{
		TimerFlag_ = 1;
	}
	void Reset_TimerFlag_()
	{
		TimerFlag_ = 0;
	}

	double PERFORMENCE_FREQ_;
	bool TimerFlag_;
	double step_time_;
};

extern CSysTick tickTimer;
#endif
/*End of File*/
