/**
  ******************************************************************************
  * @file    CPeriph.h
  * @author  Chenxx
  * @version V1.2
  * @date    2015-09-05
  * @brief   This file defines the class CInte_Base, CDma_Base, CInte_Dma.
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
	{}
	uint8_t GetIRQChannel(void)	
	{
		return NVIC_IRQChannel_;
	}
	void Init(uint8_t PrePriority, uint8_t SubPriority);
	void Init(uint8_t PrePriority, uint8_t SubPriority, uint32_t Periph_IT);
	virtual void EnableLine(uint32_t Periph_IT) = 0;
	virtual void DisableLine(uint32_t Periph_IT) = 0;
		
	virtual ~CInte_Base(void)
	{}
protected:
	void NvicConfig(uint8_t PrePriority,uint8_t SubPriority);
	uint8_t NVIC_IRQChannel_;	
	void* Periphx_;
};

/******************************************************************************
* @brief   This part for CInte_Dma
******************************************************************************/
class CInte_Dma 
	:public CInte_Base
{
public:
	CInte_Dma(const CDma_Base* pDma);
	virtual void EnableLine(uint32_t DMA_IT)
		{DMA_ITConfig((DMA_Channel_TypeDef*)Periphx_ ,DMA_IT,ENABLE);}	
	virtual void DisableLine(uint32_t DMA_IT)
		{DMA_ITConfig((DMA_Channel_TypeDef*)Periphx_ ,DMA_IT,DISABLE);}
		
//	DMA_Channel_TypeDef* GetDMAy_Channelx_(void)
//		{return (DMA_Channel_TypeDef*)Periphx_;}
};

/******************************************************************************
* @brief   This part for CDma_Base
******************************************************************************/
class CDma_Base
{
public:
	CDma_Base(DMA_Channel_TypeDef* DMAy_Channelx = 0);
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
	void Reboot(uint32_t ReloadMemAddr, u16 ReloadBufferSize);
	
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
	
	virtual void PeriphConfig() = 0;
	virtual void Init(uint32_t PeriphAddr,
		uint32_t MemAddr,
		uint32_t DMA_DIR,
		uint32_t BufferSize,
		uint32_t PDataSize,	
		uint32_t MDataSize,
		uint32_t DMA_Mode,
		CInte_Dma* pInte_Dma) = 0;

	virtual ~CDma_Base(void)
	{
		delete INTE;
	}
protected:
	DMA_Channel_TypeDef* DMAy_Channelx_;
	DMA_InitTypeDef DMA_InitStructure_;
	uint8_t NVIC_IRQChannel_;
};
/******************************************************************************
* @brief   This part for CPeriph
******************************************************************************/
class CPeriph
{
public:
	CPeriph()
	{}
	virtual CDma_Base* Creat_CDma(int DmaCh) const = 0;
	static void EmptyInit();
	virtual ~CPeriph()
	{}
	
private:
};
/******************************************************************************
* @brief   This part for CDmaKit
******************************************************************************/
class CDmaKit:
	public kit<CDma_Base*>
{
public:
	CDmaKit(const CPeriph* pPeriph):
		pPeriph_(pPeriph)
	{}
	~CDmaKit()
	{}
		
	void add(int DmaCh)
	{
		kit::add(DmaCh, pPeriph_->Creat_CDma(DmaCh));
	}
	void destroy(int DmaCh)
	{
		delete handle(DmaCh);
		while(1);// TO DO. The 'destroy' in "kit.h" was left unfinished!
	}
private:
	const CPeriph* pPeriph_;
};
#endif 
