/**
  ******************************************************************************
  * @file    CPeriph.h
  * @author  Chenxx
  * @version V1.1
  * @date    2015-09-05
  * @brief   This file defines the class CInte_Base, CDma_Base, CInte_Dma, CPeriph.
  ******************************************************************************/
#ifndef __CPERIPH_HPP
#define __CPERIPH_HPP
#include "stm32f10x.h"
#include <string.h>
#include "kit.h"

class CDma_Base;
/******************************************************************************
* @brief   This part for CInte_Base
******************************************************************************/
class CInte_Base
{
public:
	CInte_Base()
		:NVIC_IRQChannel_(0)
	{}
	CInte_Base(uint8_t NVIC_IRQChannel)
		:NVIC_IRQChannel_(NVIC_IRQChannel)
	{}
	uint8_t GetIRQChannel(void)	
		{return NVIC_IRQChannel_;}
	~CInte_Base(void)
	{}
protected:
	void NvicConfig(uint8_t PrePriority,uint8_t SubPriority);
	uint8_t NVIC_IRQChannel_;	
};

/******************************************************************************
* @brief   This part for CInte_Dma
******************************************************************************/
class CInte_Dma 
	:public CInte_Base
{
public:
	CInte_Dma(const CDma_Base& ODma);
	inline void Init(uint8_t PrePriority, uint8_t SubPriority)
	{
		NvicConfig(PrePriority, SubPriority);
	}
	inline void Init(uint8_t PrePriority, uint8_t SubPriority, uint32_t DMA_IT)
	{
		NvicConfig(PrePriority, SubPriority);
		EnableLine(DMA_IT);
	}
	void EnableLine(uint32_t DMA_IT)
		{DMA_ITConfig(DMAy_Channelx_ ,DMA_IT,ENABLE);}	
	void DisableLine(uint32_t DMA_IT)
		{DMA_ITConfig(DMAy_Channelx_ ,DMA_IT,DISABLE);}
		
	DMA_Channel_TypeDef* GetDMAy_Channelx_(void)
		{return DMAy_Channelx_;}
		
private:
	DMA_Channel_TypeDef* DMAy_Channelx_;
};

/******************************************************************************
* @brief   This part for CDma_Base
******************************************************************************/
class CDma_Base
{
public:
	
	CDma_Base():INTE(NULL), DMAy_Channelx_(0), NVIC_IRQChannel_(0) 
	{
		memset(&DMA_InitStructure_, 0, sizeof(DMA_InitStructure_));
	}
	CDma_Base(DMA_Channel_TypeDef* DMAy_Channelx);
	void ClkConfig(void);
	void Config(uint32_t PeriphAddr,
							uint32_t MemAddr,
							uint32_t DMA_DIR,
							uint32_t BufferSize,
							uint32_t PDataSize,	
							uint32_t MDataSize,
							uint32_t DMA_Mode);
	inline void Enable(void)
		{DMA_Cmd(DMAy_Channelx_, ENABLE);}
	inline void Disable(void)
		{DMA_Cmd(DMAy_Channelx_, DISABLE);}
	inline void Reboot(uint32_t ReloadMemAddr, u16 ReloadBufferSize)
	{
		Disable();
		
		if(ReloadBufferSize != 0)
		{
			DMAy_Channelx_->CNDTR = ReloadBufferSize;
			DMA_InitStructure_.DMA_BufferSize = ReloadBufferSize;
		}
		else	//ReloadBufferSize == 0, load the size that used last time
			DMAy_Channelx_->CNDTR = DMA_InitStructure_.DMA_BufferSize;
		
		if(ReloadMemAddr != NULL)
		{
			DMAy_Channelx_->CMAR = ReloadMemAddr;
			DMA_InitStructure_.DMA_MemoryBaseAddr = ReloadMemAddr;
		}
		else
			DMAy_Channelx_->CMAR = DMA_InitStructure_.DMA_MemoryBaseAddr;
		
		Enable();
	}
	CInte_Dma* INTE;
	
	DMA_Channel_TypeDef* GetDMAy_Channelx_(void) const
		{return DMAy_Channelx_;}
	uint8_t GetNVIC_IRQChannel_(void) const
		{return NVIC_IRQChannel_;}
	DMA_InitTypeDef& GetDMA_InitStructure_(void)
	{
		if (DMA_InitStructure_.DMA_MemoryBaseAddr != 0)
			return DMA_InitStructure_;
		else while(1); //#error! Value was not set
	}
	~CDma_Base(void)
		{/*delete INTE;*/}
protected:
	DMA_Channel_TypeDef* DMAy_Channelx_;
	DMA_InitTypeDef DMA_InitStructure_;
	uint8_t NVIC_IRQChannel_;
};
/******************************************************************************
* @brief   This part for CDmaKit
******************************************************************************/
template <typename TDma_Periph, typename TPeriph>
class CDmaKit
	:public kit<TDma_Periph>
{
public:		
	CDmaKit(const TPeriph& OPeriph)
		:OPeriph_(OPeriph)
	{}
	~CDmaKit()
	{}
	void add(typename TPeriph::DmaChType DmaCh)
	{
		kit<TDma_Periph>::registry_.push_back(
			typename kit<TDma_Periph>::Association(DmaCh, TDma_Periph(OPeriph_.DmaChCalc(DmaCh))));
	}		
protected:
	const TPeriph& OPeriph_;
};
/******************************************************************************
* @brief   This part for CPeriph
******************************************************************************/
template <typename TInte, typename TDma_Periph, typename TPeriph>
//template <typename TInte, typename T2>
class CPeriph 
{
public:
	CPeriph(void (*InitFunc)(void))
		:Init(InitFunc),INTE(NULL),DMA(NULL)
	{
		//InitFunc is a pointer, but was passed to base class
	}
	void (*Init)(void);
	TInte* INTE;
	CDmaKit<TDma_Periph, TPeriph>* DMA;
	~CPeriph(void)
	{
		delete INTE;
		delete DMA;
	}
protected:

};

#endif 
