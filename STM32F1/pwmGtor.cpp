/**
  ******************************************************************************
  * @file    pwmGtor.cpp
  * @author  Chenxx
  * @version V1.0
  * @date    2016-01-30
  * @brief   This file provides the pwm generator functions.
  ******************************************************************************/
#include "pwmGtor.h"
#include "CTimer.h"

pwmGtor::pwmGtor(TIM_TypeDef* TIMx, int TimChx, uint32_t Freq)
	:TimCh_(TimChx)
{
	Timer_ = new CTimer(TIMx, Timer_->EmptyInit);
	CTimer_Init(TimChx, Freq);
}

void pwmGtor::CTimer_Init(int TimChx, uint32_t Freq)
{
	Timer_->ClkConfig();
	Timer_->Config(72000000ul/Freq ,1);
	Timer_->PinConfig((CTimer::TimChType)TimChx, GPIO_Mode_AF_PP);
	PwmConfig(0, (CTimer::TimChType)TimChx);
}

void pwmGtor::PwmConfig(uint16_t Pulse, int TimChx)
{
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	TIM_OCInitStructure.TIM_Pulse = Pulse ;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	
	if(((u8)TimChx&(u8)CTimer::TimCh1) == (u8)CTimer::TimCh1)
		TIM_OC1Init(Timer_->GetTIMx_(), & TIM_OCInitStructure); 
	
	if(((u8)TimChx&(u8)CTimer::TimCh2) == (u8)CTimer::TimCh2)
		TIM_OC2Init(Timer_->GetTIMx_(), & TIM_OCInitStructure); 
	
	if(((u8)TimChx&(u8)CTimer::TimCh3) == (u8)CTimer::TimCh3)
		TIM_OC3Init(Timer_->GetTIMx_(), & TIM_OCInitStructure); 
	
	if(((u8)TimChx&(u8)CTimer::TimCh4) == (u8)CTimer::TimCh4)
		TIM_OC4Init(Timer_->GetTIMx_(), & TIM_OCInitStructure); 
}
void pwmGtor::set_PwmDuty_phy(int TimChx, uint16_t duty_phy)
{
	static uint16_t AutoReloadValue = Timer_->GetTIMx_()->ARR;
	uint32_t duty_raw;
	duty_raw = (uint32_t)duty_phy*AutoReloadValue/10000;
	set_PwmDuty_raw(TimChx, duty_raw);
}

void pwmGtor::set_PwmDuty_raw(int TimChx, uint16_t duty_raw)
{
	static volatile uint16_t* BASE_ADDR = &(Timer_->GetTIMx_()->CCR1);
	TimChx &= TimCh_; //engage only operating the pwm channel
	
	for(uint8_t i = 0; i < 4; i++)
	{
		if((TimChx>>i)&1)
		{
			*(BASE_ADDR + 2*i) = duty_raw;
		}
		else
		{
			*(BASE_ADDR + 2*i) = 0;
		}
	}
}
void pwmGtor::start()
{
	Timer_->Start();
}
void pwmGtor::stop()
{
	Timer_->Stop();
}
const CTimer* pwmGtor::Get_Timer_() const
{
	return Timer_;
}
//pwmGtor pwmGtor1(TIM2, 1, 10000);
//end of file
