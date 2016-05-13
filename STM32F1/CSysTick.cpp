/**
******************************************************************************
* @file    CSysTick.cpp
* @author  Chenxx
* @version V1.0
* @date    2016-1-27
* @brief   This file provides all the SysTick functions.
******************************************************************************/
#include "CSysTick.h"
#include "stddef.h"

//
//constructor
//
CSysTick::CSysTick()
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); //9MHz
	stop();
	reset();
	SetFreq(10);	//default frequence 1kHz
}

//
//void CSysTick::SetFreq(uint32_t freq)
//
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

//
//uint32_t CSysTick::get_Freq()
//
uint32_t CSysTick::get_Freq()
{
	uint32_t freq = 9000000/SysTick->LOAD;
	return freq;
}
//end of file
