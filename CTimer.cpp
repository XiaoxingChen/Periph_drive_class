/**
  ******************************************************************************
  * @file    CTimer.cpp
  * @author  Chenxx
  * @version V1.1
  * @date    2015-09-05
  * @brief   This file provides all the general perpose timer functions.
  ******************************************************************************/
#include "CTimer.h"
#include "CLed.h"
#include "stm32f10x.h"

/******************************************************************************
* @brief   This part for object declaration
******************************************************************************/
//CTimer MotTimer(TIM2, TIM2_Init);
//CTimer StepTimer(TIM3, TIM3_Init);
//CTimer EncoTimer(TIM4, TIM4_Init);

/******************************************************************************
* @brief   This part for CInte_Timer
******************************************************************************/
CInte_Timer::CInte_Timer(const CTimer& OTimer)
	:CInte_Base(OTimer.GetNVIC_IRQChannel_()),
	TIMx_(OTimer.GetTIMx_())
	{}
/******************************************************************************
* @brief   This part for CTimer
******************************************************************************/
CTimer::CTimer(TIM_TypeDef* TIMx,void(*InitFunc)(void))
	:CPeriph(InitFunc),TIMx_(TIMx)
{
	NVIC_IRQChannel_ = TIM2_IRQn+((uint32_t)TIMx_-TIM2_BASE)/0x400;
}

void CTimer::ClkConfig(void)
{
	uint32_t RCC_APB1Periph_TIMx;
	RCC_APB1Periph_TIMx = RCC_APB1Periph_TIM2<<((uint32_t)TIMx_-TIM2_BASE)/0x400;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIMx , ENABLE);
}

void CTimer::Config(uint16_t Period, uint16_t Prescaler)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	TIM_TimeBaseStructure.TIM_Period=Period-1;
	TIM_TimeBaseStructure.TIM_Prescaler= Prescaler-1;
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIMx_, &TIM_TimeBaseStructure);

	TIM_ClearFlag(TIMx_, TIM_FLAG_Update);
}

void CTimer::PinConfig(u8 TimChx, GPIOMode_TypeDef GPIO_Mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode; 
	if(TIMx_ == TIM2)
	{
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO , ENABLE);	
 	
	GPIO_InitStructure.GPIO_Pin = TimChx; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	else if(TIMx_ == TIM3 )
	{
		RCC_APB2PeriphClockCmd( 
			RCC_APB2Periph_GPIOA|
			RCC_APB2Periph_GPIOB|
			RCC_APB2Periph_AFIO , ENABLE);	
 	
		GPIO_InitStructure.GPIO_Pin = (TimChx&0x3)<<6;  
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = (TimChx&0xC)>>2; 
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	}
	else if(TIMx_ == TIM4)
	{
			RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO , ENABLE);	
 	
			GPIO_InitStructure.GPIO_Pin = TimChx<<6; 
			GPIO_Init(GPIOB, &GPIO_InitStructure);
	}
	else
		while(1);//error
}

void CTimer::PwmPinConfig(u8 TimChx)
{
	PinConfig(TimChx, GPIO_Mode_AF_PP);
}

void CTimer::PwmConfig(uint16_t Pulse, u8 TimChx)
{
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	TIM_OCInitStructure.TIM_Pulse = Pulse ;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	
	if(((u8)TimChx&(u8)TimCh1) == (u8)TimCh1)
		TIM_OC1Init(TIMx_, & TIM_OCInitStructure); 
	
	if(((u8)TimChx&(u8)TimCh2) == (u8)TimCh2)
		TIM_OC2Init(TIMx_, & TIM_OCInitStructure); 
	
	if(((u8)TimChx&(u8)TimCh3) == (u8)TimCh1)
		TIM_OC3Init(TIMx_, & TIM_OCInitStructure); 
	
	if(((u8)TimChx&(u8)TimCh4) == (u8)TimCh1)
		TIM_OC4Init(TIMx_, & TIM_OCInitStructure); 
}
void CTimer::PwmInit(uint16_t Pulse, u8 TimChx)
{
	PwmConfig(Pulse, TimChx);
	PwmPinConfig(TimChx);
}

void CTimer::IcPinConfig(u8 TimChx)
{
	PinConfig(TimChx, GPIO_Mode_IPU);
}
void CTimer::IcConfig(CTimer::TimChType TimChx, uint16_t ICFilter)
{
	//while the function lib does not us "|=" to judge channel
	//we cannot set more than 1 channel at one time
	//so the type of TimChx must be CTimer::TimChType
	TIM_ICInitTypeDef TIM_ICInitStructure;
	
	if((TimChx) ==TimCh1)
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_1; 
	else if(TimChx == TimCh2)
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_2; 
	else if(TimChx == TimCh3)
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_3; 
	else if(TimChx == TimCh4)
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_4; 
	else
		while(1); //input error            
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;   
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; 
	TIM_ICInitStructure.TIM_ICFilter = ICFilter;      
  TIM_ICInit(TIMx_, &TIM_ICInitStructure);
}

void CTimer::IcInit(u8 TimChx, uint16_t ICFilter)
{
	IcPinConfig(TimChx);
	if(TimChx&TimCh1)
		IcConfig(TimCh1, ICFilter);
	if(TimChx&TimCh2)
		IcConfig(TimCh2, ICFilter);
	if(TimChx&TimCh3)
		IcConfig(TimCh3, ICFilter);
	if(TimChx&TimCh4)
		IcConfig(TimCh4, ICFilter);
	
	TIM_SelectInputTrigger(TIMx_, TIM_TS_TI1FP1); // Select the IC1 as trigger source
	TIM_SelectSlaveMode(TIMx_, TIM_SlaveMode_Reset); // Select the slave mode, reset and trigged
	TIM_SelectMasterSlaveMode(TIMx_, TIM_MasterSlaveMode_Enable); 
}

void CTimer::IcEncoderInit(uint16_t ICFilter)
{
	IcPinConfig(TimCh1|TimCh2);

		IcConfig(TimCh1, ICFilter);
		IcConfig(TimCh2, ICFilter);
	TIM_EncoderInterfaceConfig(TIMx_, 
		TIM_EncoderMode_TI12, 
		TIM_ICPolarity_Rising, 
		TIM_ICPolarity_Rising);
}

DMA_Channel_TypeDef* CTimer::DmaChCalc(DmaChType DmaCh) const
{
	if(((TIMx_ == TIM2)&&(DmaCh == DmaChC3))
		||((TIMx_ == TIM4)&&(DmaCh == DmaChC1)))
	{
		return DMA1_Channel1;
	}
	else if(((TIMx_ == TIM2)&&(DmaCh == DmaChUp))
				||((TIMx_ == TIM3)&&(DmaCh == DmaChC3)))
	{
		return DMA1_Channel2;
	}
	else if(((TIMx_ == TIM3)&&(DmaCh == DmaChC4))
				||((TIMx_ == TIM3)&&(DmaCh == DmaChUp)))
	{
		return DMA1_Channel3;
	}
	else if(((TIMx_ == TIM4)&&(DmaCh == DmaChC2)))
	{
		return DMA1_Channel4;
	}
	else if(((TIMx_ == TIM2)&&(DmaCh == DmaChC1))
				||((TIMx_ == TIM4)&&(DmaCh == DmaChC3)))
	{
		return DMA1_Channel5;
	}
	else if(((TIMx_ == TIM3)&&(DmaCh == DmaChC1))
				||((TIMx_ == TIM3)&&(DmaCh == DmaChTri)))
	{
		return DMA1_Channel6;
	}
	else if(((TIMx_ == TIM2)&&(DmaCh == DmaChC2))
				||((TIMx_ == TIM2)&&(DmaCh == DmaChC4))
				||((TIMx_ == TIM4)&&(DmaCh == DmaChUp)))
	{
		return DMA1_Channel7;
	}
	else
		while(1);
}
/******************************************************************************
* @brief   This part for CDma_Timer
******************************************************************************/
CDma_Timer::CDma_Timer(const CTimer& OTimer, CTimer::DmaChType DmaCh)
	:CDma_Base(OTimer.DmaChCalc(DmaCh)),
	TIMx_(OTimer.GetTIMx_()),
	DmaCh_(DmaCh)
{}
void CDma_Timer::Init(uint32_t PeriphAddr,
							uint32_t MemAddr,
							uint32_t DMA_DIR,
							uint32_t BufferSize,
							uint32_t PDataSize,	
							uint32_t MDataSize,
							uint32_t DMA_Mode,
							CInte_Dma* OInte_Dma)
{
	INTE = OInte_Dma;
	ClkConfig();
	Config(PeriphAddr,
				MemAddr,
				DMA_DIR,
				BufferSize,
				PDataSize,	
				MDataSize,
				DMA_Mode);
	PeriphConfig(PeriphAddr-TIMx_->CR1, (BufferSize-1)<<8);
	Enable();
}
void CDma_Timer::PeriphConfig(uint16_t TIM_DMABase, uint16_t TIM_DMABurstLength)
{
	TIM_DMAConfig(TIMx_, TIM_DMABase, TIM_DMABurstLength);
	TIM_DMACmd(TIMx_, DmaCh_, ENABLE);
}

/******************************************************************************
* @brief   This part for TIM3 InitFunction
 ******************************************************************************/
void TIM3_Init(void)
{
//	CTimer& Timerx = StepTimer;
//	
//	Timerx.ClkConfig();
//	Timerx.Config(3000,24);
//	
//	Timerx.INTE = new CInte_Timer(Timerx);
//	Timerx.INTE->Init(1,1,TIM_IT_Update);
}
/******************************************************************************
* @brief   This part for TIM4 InitFunction
 ******************************************************************************/
//void TIM4_Init(void)
//{
//	CTimer& Timerx = EncoTimer;
//	const u16 ENCODER_PPR = 512;
//	const u8 OV_CIRCLE = 30;
//	
//	Timerx.ClkConfig();
//	Timerx.Config(ENCODER_PPR*4*OV_CIRCLE, 1);
//	
//	Timerx.IcEncoderInit(0x06);
//	
//	Timerx.INTE = new CInte_Timer(Timerx);
//	Timerx.INTE->Init(1,1,TIM_IT_Update);
//	Timerx.INTE->EnableLine(TIM_IT_Update);
//}
/******************************************************************************
* @brief   This part for TIM2 InitFunction
 ******************************************************************************/
//void TIM2_Init(void)
//{
//	CTimer& Timerx = MotTimer;
//	
//	Timerx.ClkConfig();
//	Timerx.Config(800,9);
//	
//	Timerx.PwmInit(400, CTimer::TimCh1);
////Timerx.PwmInit(400, CTimer::TimCh1|CTimer::TimCh2);
//}

/*End of File*/
