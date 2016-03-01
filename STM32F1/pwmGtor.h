/**
  ******************************************************************************
  * @file    pwmGtor.h
  * @author  Chenxx
  * @version V1.0
  * @date    2016-01-30
  * @brief   This file defines the pwm generator functions.
  ******************************************************************************/
#ifndef __PWMGTOR_H
#define __PWMGTOR_H
#include "stm32f10x.h"
class CTimer;
class pwmGtor
{
public:
	pwmGtor(TIM_TypeDef* TIMx = TIM2, 
		int TimChx = 1,
		uint32_t Freq = 10000);
	void CTimer_Init(int TimChx, uint32_t Freq);
	void set_PwmDuty_phy(int TimChx, uint16_t duty_phy);
	void set_PwmDuty_raw(int TimChx, uint16_t duty_raw);
	void start();
	void stop();
	const CTimer* Get_Timer_() const;
private:
	void PwmConfig(uint16_t Pulse, int TimChx);
	CTimer* Timer_;
	uint8_t TimCh_;
		
};
//extern pwmGtor pwmGtor1;
#endif
//end of file
