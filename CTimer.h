/**
  ******************************************************************************
  * @file    CTimer.h
  * @author  Chenxx
  * @version V1.1
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
	CInte_Timer(const CTimer& OTimer);
	inline void Init(uint8_t PrePriority, uint8_t SubPriority)
	{
		NvicConfig(PrePriority, SubPriority);
	}
	inline void Init(uint8_t PrePriority, uint8_t SubPriority, uint32_t TIM_IT)
	{
		NvicConfig(PrePriority, SubPriority);
		EnableLine(TIM_IT);
	}
	void EnableLine(uint16_t TIM_IT)
		{TIM_ITConfig(TIMx_, TIM_IT, ENABLE);}
	void DisableLine(uint16_t TIM_IT)
		{TIM_ITConfig(TIMx_, TIM_IT, DISABLE);}
private:
	TIM_TypeDef* TIMx_;
};
/******************************************************************************
* @brief   This part for CTimer
******************************************************************************/
class CDmaKit_Timer;
class CTimer
	:public CPeriph<CInte_Timer, CDma_Base, CTimer>
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
	
	CDmaKit_Timer* DMA; //cover the DMA in CPeriph
	
	void Config(uint16_t Period, uint16_t Prescaler);
	void ClkConfig();
	void PinConfig(u8 TimChx, GPIOMode_TypeDef GPIO_Mode);
	
	void PwmInit(uint16_t Pulse, u8 TimChx);
	void PwmConfig(uint16_t Pulse, u8 TimChx);
	void PwmPinConfig(u8 TimChx);

	void IcInit(u8 TimChx, uint16_t ICFilter); // Input capture
	void IcEncoderInit(uint16_t ICFilter); // Input capture
	void IcConfig(CTimer::TimChType TimChx, uint16_t ICFilter);
	void IcPinConfig(u8 TimChx);

	uint8_t GetNVIC_IRQChannel_(void) const
	{return NVIC_IRQChannel_;}
	TIM_TypeDef* GetTIMx_(void) const
	{return TIMx_;}
	
	void Start(void)
	{TIM_Cmd(TIMx_, ENABLE);}
	void Stop(void)
	{TIM_Cmd(TIMx_, DISABLE);}
	DMA_Channel_TypeDef* DmaChCalc(DmaChType DmaCh) const;
	~CTimer(void)
	{}
protected:
	TIM_TypeDef* TIMx_;
	uint8_t NVIC_IRQChannel_;
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
		:TIMx_(NULL)
	{}
	/*mostly used constructor*/
	CDma_Timer(const CTimer& OTimer, CTimer::DmaChType DmaCh);
	/*copy constructor*/
	CDma_Timer(const CDma_Timer& other)
		:CDma_Base(other.GetDMAy_Channelx_()),
		TIMx_(other.Get_TIMx_()),
		DmaCh_(other.Get_DmaCh_())
	{}
	~CDma_Timer()
	{}
	void PeriphConfig(uint16_t TIM_DMABase, uint16_t TIM_DMABurstLength);
	void Init(uint32_t PeriphAddr,
							uint32_t MemAddr,
							uint32_t DMA_DIR,
							uint32_t BufferSize,
							uint32_t PDataSize,	
							uint32_t MDataSize,
							uint32_t DMA_Mode,
							CInte_Dma* OInte_Dma);
	TIM_TypeDef* Get_TIMx_() const
		{return TIMx_;}
	CTimer::DmaChType Get_DmaCh_() const
		{return DmaCh_;}
private:
	TIM_TypeDef* TIMx_;
	CTimer::DmaChType DmaCh_;
};
/******************************************************************************
* @brief   This part for CDmaKit_Timer
******************************************************************************/
class CDmaKit_Timer
	:public CDmaKit<CDma_Timer, CTimer>
{
public:		
	CDmaKit_Timer(const CTimer& OTimer)
		:CDmaKit(OTimer)
	{}
	~CDmaKit_Timer()
	{}
	void add(CTimer::DmaChType DmaCh)
	{
		registry_.push_back(
			kit<CDma_Timer>::Association(DmaCh, CDma_Timer(OPeriph_, DmaCh)));
	}
private:
};
/******************************************************************************
* @brief   This part for Init Function
******************************************************************************/
void TIM3_Init(void);
void TIM4_Init(void);
//void TIM2_Init(void);
/******************************************************************************
* @brief   This part for extern object
******************************************************************************/
//extern CTimer StepTimer;
//extern CTimer EncoTimer;
//extern CTimer MotTimer;


#endif
/*End of File*/
