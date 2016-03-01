/********************************************************************************
* @file    CPeriph.cpp
* @author  Chenxx
* @version V1.2
* @date    2015-09-05
* @brief   This file defines the class CInte_Base, CDma_Base, CInte_Dma, CPeriph.
********************************************************************************/
#include "CPeriph.h"
#include "stm32f4xx.h"
#include <string.h>

/******************************************************************************
* @brief   This part for CInte_Base
******************************************************************************/

void CInte_Base::NvicConfig(uint8_t PrePriority,uint8_t SubPriority)
{
	NVIC_InitTypeDef NVIC_InitStructure;
													
	NVIC_InitStructure.NVIC_IRQChannel = NVIC_IRQChannel_;	  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PrePriority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
void CInte_Base::Init(uint8_t PrePriority, uint8_t SubPriority)
{
	NvicConfig(PrePriority, SubPriority);
}
void CInte_Base::Init(uint8_t PrePriority, uint8_t SubPriority, uint32_t Periph_IT)
{
	NvicConfig(PrePriority, SubPriority);
	EnableLine(Periph_IT);
}
#if 1
/******************************************************************************
* @brief   This part for CInte_Dma
******************************************************************************/

CInte_Dma::CInte_Dma(const CDma_Base* pDma)
{
	NVIC_IRQChannel_ = pDma->GetNVIC_IRQChannel_();
	Periphx_ = (void*)pDma->GetDMAy_Streamx_();
}
#endif
#if 1
/******************************************************************************
* @brief   This part for CDma_Base
******************************************************************************/
CDma_Base::CDma_Base(DMA_Stream_TypeDef* DMAy_Streamx)
	:INTE(NULL), DMAy_Streamx_(DMAy_Streamx)
{
	uint8_t offset;
	if(((uint32_t)DMAy_Streamx_&0xFFFFFC00) == DMA1_BASE)
	{
		offset = ((uint32_t)DMAy_Streamx_-DMA1_Stream0_BASE)/0x18;
		
		if(offset == 7) 
			NVIC_IRQChannel_ = 47;
		else
			NVIC_IRQChannel_ = (uint16_t)DMA1_Stream0_IRQn + offset;
	}
	else if(((uint32_t)DMAy_Streamx_&0xFFFFFC00) == DMA2_BASE)
	{
		offset = ((uint32_t)DMAy_Streamx_-DMA2_Stream0_BASE)/0x18;
		
		if(offset <= 4)
			NVIC_IRQChannel_ = (uint16_t)DMA2_Stream0_IRQn + offset;
		else
			NVIC_IRQChannel_ = (uint16_t)DMA2_Stream5_IRQn + offset - 5;
		
	}
	else
		while(1);//error
	
	memset(&DMA_InitStructure_, sizeof(DMA_InitStructure_), 0);
	DMA_InitStructure_.DMA_Channel = 0xFFFFFFFF;
}

void CDma_Base::ClkConfig(void)
{
	uint32_t RCC_AHB1Periph;
	if(((uint32_t)DMAy_Streamx_&0xFFFFFF00) == DMA1_BASE)
		RCC_AHB1Periph = RCC_AHB1Periph_DMA1;
	else if(((uint32_t)DMAy_Streamx_&0xFFFFFF00) == DMA2_BASE)
		RCC_AHB1Periph = RCC_AHB1Periph_DMA2;
	else
		while(1);//error
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph, ENABLE);
}

void CDma_Base::Config(uint32_t PeriphAddr,
									uint32_t MemAddr,
									uint32_t DMA_DIR,
									uint32_t BufferSize,
									uint32_t PDataSize,	
									uint32_t MDataSize,
									uint32_t DMA_Mode)
{
	DMA_Cmd(DMAy_Streamx_, DISABLE);
	
	if(DMA_InitStructure_.DMA_Channel == 0xFFFFFFFF) while(1); //forget to write DMA_Channel
	DMA_InitStructure_.DMA_PeripheralBaseAddr = PeriphAddr;
	DMA_InitStructure_.DMA_Memory0BaseAddr = MemAddr;
	DMA_InitStructure_.DMA_DIR = DMA_DIR;
	DMA_InitStructure_.DMA_BufferSize = BufferSize;
	DMA_InitStructure_.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure_.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure_.DMA_PeripheralDataSize = PDataSize;
	DMA_InitStructure_.DMA_MemoryDataSize = MDataSize;
	DMA_InitStructure_.DMA_Mode = DMA_Mode;
	DMA_InitStructure_.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure_.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure_.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull; 
	DMA_InitStructure_.DMA_MemoryBurst = DMA_MemoryBurst_Single; 
	DMA_InitStructure_.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

	DMA_Init(DMAy_Streamx_, &DMA_InitStructure_);

}

//
// void Reboot(uint32_t ReloadMemAddr, u16 ReloadBufferSize) 
//
void CDma_Base::Reboot(uint32_t ReloadMemAddr, u16 ReloadBufferSize)
{
	Disable();
	clear_DMA_TCIF();
	if(ReloadBufferSize != 0)
	{
		DMAy_Streamx_->NDTR = ReloadBufferSize;
//		DMA_SetCurrDataCounter(DMAy_Streamx_, ReloadBufferSize);
		DMA_InitStructure_.DMA_BufferSize = ReloadBufferSize;
	}
	else	//ReloadBufferSize == 0, load the size that used last time
	{
		DMAy_Streamx_->NDTR = DMA_InitStructure_.DMA_BufferSize;
	}
	
	if(ReloadMemAddr != NULL)
	{
		DMAy_Streamx_->M0AR = ReloadMemAddr;
		DMA_InitStructure_.DMA_Memory0BaseAddr = ReloadMemAddr;
	}
	else
		DMAy_Streamx_->M0AR = DMA_InitStructure_.DMA_Memory0BaseAddr;
	
	Enable();
}

//
// void Reboot() const
//
void CDma_Base::Reboot() const
{
	Disable();
	clear_DMA_TCIF();
	DMAy_Streamx_->NDTR = DMA_InitStructure_.DMA_BufferSize;
	DMAy_Streamx_->M0AR = DMA_InitStructure_.DMA_Memory0BaseAddr;
	Enable();
}

//
//void Write_DMA_Channel(uint32_t DMA_Channel)
//
void CDma_Base::Write_DMA_Channel(uint32_t DMA_Channel)
{
	DMA_InitStructure_.DMA_Channel = DMA_Channel;
}

//
//
//
#define RESERVED_MASK           (uint32_t)0x0F7D0F7D  
void CDma_Base::clear_DMA_TCIF() const
{
	DMA_TypeDef* DMAy;
	  /* Determine the DMA to which belongs the stream */
  if (DMAy_Streamx_ < DMA2_Stream0) 
		DMAy = DMA1; 
  else 
		DMAy = DMA2; 
	
	uint32_t Streamx = (((uint32_t)DMAy_Streamx_&0xFF) - 0x10)/0x18;	// 0-7

	if(Streamx == 0) DMAy->LIFCR = 1<<5;
	else if(Streamx == 1)	DMAy->LIFCR = 1<<11;
	else if(Streamx == 2) DMAy->LIFCR = 1<<21;
	else if(Streamx == 3) DMAy->LIFCR = 1<<27;
	else if(Streamx == 4) DMAy->HIFCR = 1<<5;
	else if(Streamx == 5) DMAy->HIFCR = 1<<11;
	else if(Streamx == 6) DMAy->HIFCR = 1<<21;
	else if(Streamx == 7) DMAy->HIFCR = 1<<27;
}


#endif
/******************************************************************************
* @brief   This part for CPeriph
******************************************************************************/
void CPeriph::EmptyInit()
{
	while(1); //you should not call an empty initfunc!
}
//end of file
