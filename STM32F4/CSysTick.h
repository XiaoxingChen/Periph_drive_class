/**
******************************************************************************
* @file    CSysTick.h
* @author  Chenxx
* @version V1.0
* @date    2016-01-27
* @brief   This file defines the SysTick class.
* 					Note: this version is for F4xx.
******************************************************************************/
#ifndef __CSYSTICK_H
#define __CSYSTICK_H
#include "stm32f4xx.h"
#include "singleton.h"
#include "CUartConsole.h"
class CSysTick
{	
public:
	enum taskPeriod
	{
		TASK_100US = 0,
		TASK_200US,
		TASK_500US,
		TASK_1MS,
		TASK_2MS,
		TASK_5MS,
		TASK_10MS,
		TASK_20MS,
		TASK_500MS,
		TASK_1S,
		NUM_OF_TASK_PERIOD //keep this at end of enum
	};
	static const uint16_t taskPeriodOverflowNbr[NUM_OF_TASK_PERIOD];
	static uint16_t taskPeriodCounter[NUM_OF_TASK_PERIOD];
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
	void wait();
	void SetFreq(uint32_t freq);
	static uint32_t get_Freq();
	uint32_t getAbsoluteTime();
	
	
private:
	static uint16_t missUpCounter_; 
	static uint32_t absoluteTime_;
};

#define PERIOD_RUN(period, task) \
if(!CSysTick::taskPeriodCounter[period]) task;

typedef NormalSingleton<CSysTick> sysTick;

//CTickTimer
class CTickTimer
{
public:
	CTickTimer(uint32_t);
	bool isTimeUp();
	void restart();

private:
	uint32_t absoluteStartTime_;
	uint32_t timerPeriod_;
};
#endif
//end of file
