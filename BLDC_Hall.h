/**
******************************************************************************
* @file    BLDC_Hall.h
* @author  Chenxx
* @version V1.0
* @date    2016-01-29
* @brief   This file achieve the hall for brushles DC motor class
******************************************************************************/
#ifndef __BLDC_HALL_H
#define __BLDC_HALL_H
#include "stm32f10x.h"
class CExti;
class BLDC_Hall
{
public:
	BLDC_Hall(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
	void Exti_Init();
	uint8_t Get_halldata() const;
	const CExti* Get_Exti_() const
	{return Exti_;}
private:
	CExti* Exti_;
	uint16_t PIN_POS;
};
//extern BLDC_Hall Hall_Port;
#endif
//end of file
