/**
******************************************************************************
* @file    CSysTick.h
* @author  Chenxx
* @version V1.0
* @date    2016-01-27
* @brief   This file defines the SysTick class.
******************************************************************************/
#ifndef __CSYSTICK_H
#define __CSYSTICK_H
#include "stm32f10x.h"
#include "singleton.h"
class CSysTick
{	
public:
	CSysTick();
	inline void reset()
	{
		SysTick->VAL = 0;
	}
	inline void start()
	{
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	}
	inline void stop()
	{
		SysTick->CTRL &= ~(uint16_t)SysTick_CTRL_ENABLE_Msk;
	}
	inline void wait()
	{
		while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
	}
	void SetFreq(uint32_t freq);
	static uint32_t get_Freq();
private:
	
};
typedef NormalSingleton<CSysTick> sysTick;
#endif
//end of file
