/**
  ******************************************************************************
  * @file    CTimer.h
  * @author  Chenxx
  * @version V1.2
  * @date    2015-09-05
  * @brief   This file defines the CTimer class.
  ******************************************************************************/
#ifndef CTIMER_H
#define CTIMER_H
#include "stm32f10x.h"
#include "CPeriph.h"
#include "CLed.h"
//class CInte_Timer;
//class CDmaKit_Timer;
class CTimer;
/******************************************************************************
* @brief   This part for CInte_Timer
******************************************************************************/
class CInte_Timer:
	public CInte_Base
{
public:
	CInte_Timer()
	{}
	CInte_Timer(const CTimer* pTimer);
	virtual void EnableLine(uint32_t TIM_IT)
		{TIM_ITConfig((TIM_TypeDef*)Periphx_, TIM_IT, ENABLE);}
	virtual void DisableLine(uint32_t TIM_IT)
		{TIM_ITConfig((TIM_TypeDef*)Periphx_, TIM_IT, DISABLE);}
};
/******************************************************************************
* @brief   This part for CDma_Timer
******************************************************************************/
class CDma_Timer
	:public CDma_Base
{
public:
	/*default constructor*/
	CDma_Timer()
	{}
	/*mostly used constructor*/
	CDma_Timer(DMA_Channel_TypeDef* DMAy_Channelx)
		:CDma_Base(DMAy_Channelx)
	{}
	/*copy constructor*/
	CDma_Timer(const CDma_Timer& other)
		:CDma_Base(other.GetDMAy_Channelx_())
	{}
	virtual ~CDma_Timer()
	{
		if(INTE != NULL) delete INTE;
	}
	virtual void Init(uint32_t PeriphAddr,
		uint32_t MemAddr,
		uint32_t DMA_DIR,
		uint32_t BufferSize,
		uint32_t PDataSize,	
		uint32_t MDataSize,
		uint32_t DMA_Mode,
		CInte_Dma* OInte_Dma);
};
/******************************************************************************
* @brief   This part for CTimer
******************************************************************************/
class CTimer
	:public CPeriph
{
public:
	enum DmaChType
	{
		DmaChUp = TIM_DMA_Update,
		DmaChC1 = TIM_DMA_CC1,
		DmaChC2 = TIM_DMA_CC2,
		DmaChC3 = TIM_DMA_CC3,
		DmaChC4 = TIM_DMA_CC4,
		DmaChTri = TIM_DMA_Trigger
	};
	enum TimChType
	{
		TimChNone = 0,
		TimCh1 = 1,
		TimCh2 = 2,
		TimCh3 = 4,
		TimCh4 = 8,
		TimChAll = 0xF
	};
	CTimer(TIM_TypeDef* TIMx,void(*InitFunc)(void));
	virtual ~CTimer()
	{
		if(DMA != NULL) delete DMA;
		if(INTE != NULL) delete INTE;
	}
	void(* Init)();
	void Config(uint16_t Period, uint16_t Prescaler);
	void ClkConfig();
	void PinConfig(TimChType TimChx, GPIOMode_TypeDef GPIO_Mode);
	CDmaKit* DMA; //cover the DMA in CPeriph
	CInte_Timer* INTE;
	
	TIM_TypeDef* GetTIMx_(void) const
	{return TIMx_;}
	uint8_t GetNVIC_IRQChannel_(void) const
	{return NVIC_IRQChannel_;}
	DMA_Channel_TypeDef* DmaChCalc(DmaChType DmaCh) const;
	virtual CDma_Base* Creat_CDma(int DmaCh) const
	{
		/* Periph Config */
		TIM_DMACmd(TIMx_, DmaCh, ENABLE);
		return new CDma_Timer(DmaChCalc((DmaChType)DmaCh));
	}
	virtual void Destroy_CDma(int DmaCh) const
	{
		TIM_DMACmd(TIMx_, DmaCh, DISABLE);
		delete DMA->handle(DmaCh);
	}

	void IcInit(TimChType TimChx); // Input capture
	void IcConfig(TimChType TimChx);
	void IcPinConfig(TimChType TimChx);

	void Start(void)
	{TIM_Cmd(TIMx_, ENABLE);}
	void Stop(void)
	{TIM_Cmd(TIMx_, DISABLE);}
	
private:
	TIM_TypeDef* TIMx_;
	uint8_t NVIC_IRQChannel_;
};

/******************************************************************************
* @brief   This part for Init Function
******************************************************************************/
void TIM3_Init(void);
/******************************************************************************
* @brief   This part for extern object
******************************************************************************/
extern CTimer Timer3;
//extern CTimer Timer2;

#endif
/*End of File*/
