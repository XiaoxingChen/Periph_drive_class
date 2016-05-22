/**
******************************************************************************
* @file    CSysTick.cpp
* @author  Chenxx
* @version V1.0
* @date    2016-1-27
* @brief   This file provides all the SysTick functions.
* 					Note: this version is for F4xx.
******************************************************************************/
#include "CSysTick.h"
#include "stddef.h"

const uint32_t CLK_FREQ = 21000000;

//
//constructor
//
CSysTick::CSysTick()
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); //21MHz. Different from F1.
	stop();
	reset();
	SetFreq(2);	//default frequence 1kHz
}

//
//void CSysTick::SetFreq(uint32_t freq)
//
void CSysTick::SetFreq(uint32_t freq)
{
	if( CLK_FREQ/freq > 0xFFFFFFul)
	{
		while(1);//error
	}	
	else
	{
		SysTick->LOAD = CLK_FREQ/freq;
	}
}

//
//uint32_t CSysTick::get_Freq()
//
uint32_t CSysTick::get_Freq()
{
	uint32_t freq = CLK_FREQ / SysTick->LOAD;
	return freq;
}
//end of file
