/**
******************************************************************************
* @file    CExti.h
* @author  Chenxx
* @version V1.0
* @date    2016-01-26
* @brief   This file defines the Exit class.
******************************************************************************/
#ifndef __CEXTI_H
#define __CEXTI_H
#include "stm32f10x.h"
#include "CPeriph.h"
	
class CExti;
/******************************************************************************
* @brief   This part for CInte_Exti
******************************************************************************/
class CInte_Exti
	:public CInte_Base
{
public:
	CInte_Exti()
	{}
	CInte_Exti(const CExti* pExti);
	virtual void EnableLine(uint32_t EXTI_IT)
	{
		EXTI->IMR |= EXTI_IT;
	}
	virtual void DisableLine(uint32_t EXTI_IT)
	{
		EXTI->IMR &= ~EXTI_IT;
	}
private:
//	GPIO_TypeDef* GPIOx_;
};

/******************************************************************************
* @brief   This part for CExti
******************************************************************************/
class CExti
	:public CPeriph
{
public:
	friend void Hall_Exti_Init(void);
	enum DmaChType
	{
		NoDmaCh = 0
	};
	CExti(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, void(*InitFunc)(void));
	virtual ~CExti()
	{
		if(INTE != NULL) delete INTE;
	}
	void(* Init)();
	void Config(EXTITrigger_TypeDef TrigType);
	void ClkConfig();
	void PinConfig();
	CInte_Exti* INTE;
	
	GPIO_TypeDef* Get_GPIOx_(void) const
		{return GPIOx_;}
	uint8_t Get_NVIC_IRQChannel_(void) const
		{return NVIC_IRQChannel_;}
	uint16_t Get_GPIO_Pin_(void) const
		{return GPIO_Pin_;}
	uint32_t Get_ExtiLine(void) const
		{return (uint32_t)GPIO_Pin_;}
		
	virtual CDma_Base* Creat_CDma(int DmaCh) const
	{return 0;}
	virtual void Destroy_CDma(int DmaCh) const
	{}
	
private:
	uint16_t GPIO_Pin_;
	GPIO_TypeDef* GPIOx_;
	uint8_t NVIC_IRQChannel_;
};

//extern CExti Hall_Port;
	
#endif
//end of file
