/**
  ******************************************************************************
  * @file    CPeriph.h
  * @author  Chenxx
  * @version V1.2
  * @date    2015-09-05
  * @brief   This file defines the class CInte_Base, CDma_Base, CInte_Dma.
  ******************************************************************************/
/*******************************************************************************
* release information
* 2016/01/02 chenxx add void CDma_Base::Reboot() const;
*******************************************************************************/	
#ifndef __CPERIPH_H
#define __CPERIPH_H
#include "stm32f4xx.h"
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
#if 1
/******************************************************************************
* @brief   This part for CInte_Dma
******************************************************************************/
class CInte_Dma 
	:public CInte_Base
{
public:
	CInte_Dma(const CDma_Base* pDma);
	virtual void EnableLine(uint32_t DMA_IT)
		{DMA_ITConfig((DMA_Stream_TypeDef*)Periphx_ ,DMA_IT,ENABLE);}	
	virtual void DisableLine(uint32_t DMA_IT)
		{DMA_ITConfig((DMA_Stream_TypeDef*)Periphx_ ,DMA_IT,DISABLE);}
		
//	DMAy_Streamx* GetDMAy_Streamx_(void)
//		{return (DMAy_Streamx*)Periphx_;}
};
#else
	typedef void CInte_Dma;
#endif


/******************************************************************************
* @brief   This part for CDma_Base
******************************************************************************/
class CDma_Base
{
public:
	CDma_Base(DMA_Stream_TypeDef* DMAy_Streamx = 0);
	void ClkConfig(void);
	void Config(uint32_t PeriphAddr,
							uint32_t MemAddr,
							uint32_t DMA_DIR,
							uint32_t BufferSize,
							uint32_t PDataSize,	
							uint32_t MDataSize,
							uint32_t DMA_Mode);
	
	inline void Enable(void) const
		{DMA_Cmd(DMAy_Streamx_, ENABLE);}
	inline void Disable(void) const
		{DMA_Cmd(DMAy_Streamx_, DISABLE);}
	void Reboot(uint32_t ReloadMemAddr, u16 ReloadBufferSize);
	void Reboot() const;
	void Write_DMA_Channel(uint32_t);

	CInte_Dma* INTE;
	
	DMA_Stream_TypeDef* GetDMAy_Streamx_(void) const
		{return DMAy_Streamx_;}
	uint8_t GetNVIC_IRQChannel_(void) const
		{return NVIC_IRQChannel_;}
	DMA_InitTypeDef& GetDMA_InitStructure_(void)
	{
		if (DMA_InitStructure_.DMA_Memory0BaseAddr != 0)
			return DMA_InitStructure_;
		else while(1); //#error! Value was not set
	}
	virtual void Init(uint32_t PeriphAddr,
		uint32_t MemAddr,
		uint32_t DMA_DIR,
		uint32_t BufferSize,
		uint32_t PDataSize,	
		uint32_t MDataSize,
		uint32_t DMA_Mode,
		CInte_Dma* pInte_Dma = NULL) = 0;

	virtual ~CDma_Base(void)
	{
		if(INTE != NULL) 	delete INTE;
	}
protected:
	void clear_DMA_TCIF() const;
	DMA_Stream_TypeDef* DMAy_Streamx_;
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
	virtual void Destroy_CDma(int DmaCh) const = 0;
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
		pPeriph_->Destroy_CDma(DmaCh);
		kit::destroy(DmaCh);
	}
private:
	const CPeriph* pPeriph_;
};
#endif 
