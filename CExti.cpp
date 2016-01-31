/**
******************************************************************************
* @file    CExti.cpp
* @author  Chenxx
* @version V1.0
* @date    2016-1-26
* @brief   This file provides all the Exti functions.
******************************************************************************/
#include "stm32f10x.h"
#include "CExti.h"

/******************************************************************************
* @brief   This part for CInte_Exti
******************************************************************************/
CInte_Exti::CInte_Exti(const CExti* pExti)
{
	NVIC_IRQChannel_ = pExti->Get_NVIC_IRQChannel_();
	Periphx_ = (void*)pExti->Get_GPIOx_();
}
	
/******************************************************************************
* @brief   This part for CExti
******************************************************************************/
CExti::CExti(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, void(*InitFunc)(void)):
	Init(InitFunc), 
	INTE(NULL),
	GPIO_Pin_(GPIO_Pin), 
	GPIOx_(GPIOx)
{
	u8 flag = 0;
	for(int i = 0; i < 16; i++)
	{
		if(GPIO_Pin_ & (1<<i))
		{
			if(i < 5)
			{
				NVIC_IRQChannel_ = EXTI0_IRQn+i;
				flag |= 1;
			}
			else if(i < 10)
			{
				NVIC_IRQChannel_ = EXTI9_5_IRQn;
				flag |= 2;
			}
			else
			{
				NVIC_IRQChannel_ = EXTI15_10_IRQn;
				flag |= 4;
			}
		}
	}
	/* check if pins belong to 1, 2, 3, 4, 5-9, 10-15 */
	if((flag != 1) && (flag != 2) && (flag != 4))
	{
		while(1);//pin error! 
	}
}
void CExti::ClkConfig()
{
	uint32_t RCC_APB2Periph_GPIOx;
	RCC_APB2Periph_GPIOx = RCC_APB2Periph_GPIOA<<(((uint32_t)GPIOx_-(uint32_t)GPIOA)/0x400);
	// num / 0x400 == num >> 10 ?
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOx|RCC_APB2Periph_AFIO, ENABLE); 
}
void CExti::PinConfig()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;
	GPIO_Init(GPIOx_, &GPIO_InitStructure); 
	
	uint8_t GPIO_PortSource;
	uint8_t GPIO_PinSource;
	GPIO_PortSource = (uint32_t)GPIOx_>>11;
	for(int i = 0; i < 16; i++)
	{
		if(GPIO_Pin_ & (1<<i))
		{
			GPIO_PinSource = i;
			GPIO_EXTILineConfig(GPIO_PortSource, GPIO_PinSource);
		}
	}
}
void CExti::Config(EXTITrigger_TypeDef TrigType)
{
	EXTI_InitTypeDef EXTI_InitStructure; 
	
	EXTI_InitStructure.EXTI_Line = Get_ExtiLine(); 
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
	EXTI_InitStructure.EXTI_Trigger = TrigType; 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE; 
	EXTI_Init(&EXTI_InitStructure); 
}
/******************************************************************************
* @brief   This part for Init Function
******************************************************************************/
//void Hall_Exti_Init()
//{
//	CExti& CExtix = Hall_Port;
//	CExtix.ClkConfig();
//	CExtix.PinConfig();
//	CExtix.Config(EXTI_Trigger_Rising_Falling);
//	
//	CExtix.INTE = new CInte_Exti(&CExtix);
//	CExtix.INTE->Init(1,1,CExtix.Get_ExtiLine());
//}
//CExti Hall_Port(GPIOC, GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12, Hall_Exti_Init);
//end of file
