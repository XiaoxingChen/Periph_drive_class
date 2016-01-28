/**
******************************************************************************
* @file    CSysTick.cpp
* @author  Chenxx
* @version V1.0
* @date    2016-1-27
* @brief   This file provides all the SysTick functions.
******************************************************************************/
#include "CSysTick.h"
CSysTick::CSysTick(uint32_t freq)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); //9MHz
	stop();
	reset();
	SetFreq(freq);
}
void CSysTick::SetFreq(uint32_t freq)
{
	if( 9000000/freq > 0xFFFFFFul)
	{
		while(1);//error
	}	
	else
	{
		SysTick->LOAD = 9000000/freq;
	}
}
CSysTick StepTimer(1000);
//end of file
