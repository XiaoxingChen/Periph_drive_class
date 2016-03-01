#include "SysTick.h"
using namespace std;
CSysTick tickTimer;
DWORD WINAPI CSysTick::run(LPVOID lpParameter)
{
	CSysTick* sysTick = (CSysTick*)lpParameter;
	LARGE_INTEGER litmp;
	LONGLONG QPart1, QPart2;

	while (true)
	{
		QueryPerformanceCounter(&litmp);
		QPart1 = litmp.QuadPart;// get initial value
		QPart2 = QPart1 + (long)(sysTick->Get_PERFORMENCE_FREQ_()* (sysTick->step_time_) + 0.5); //target value
		do
		{
			QueryPerformanceCounter(&litmp);
			QPart1 = litmp.QuadPart;// current value
			Sleep(0);
		} while (QPart1 < QPart2);
		sysTick->Set_TimerFlag_();
	}

	return 0;
}
/*End of File*/
