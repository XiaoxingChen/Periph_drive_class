/**
  ******************************************************************************
  * @file    CPeriph.cpp
  * @author  Chenxx
  * @version V1.2
  * @date    2015-09-05
  * @brief   This file defines the class CInte_Base, CDma_Base, CInte_Dma, CPeriph.
  ******************************************************************************/
#include "CPeriph.h"
#include "stm32f10x.h"
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
/******************************************************************************
* @brief   This part for CInte_Dma
******************************************************************************/

CInte_Dma::CInte_Dma(const CDma_Base* pDma)
{
	NVIC_IRQChannel_ = pDma->GetNVIC_IRQChannel_();
	Periphx_ = (void*)pDma->GetDMAy_Channelx_();
}
/******************************************************************************
* @brief   This part for CDma_Base
******************************************************************************/
CDma_Base::CDma_Base(DMA_Channel_TypeDef* DMAy_Channelx)
	:INTE(NULL), DMAy_Channelx_(DMAy_Channelx)
{
	
	if(((uint32_t)DMAy_Channelx_&0xFFFFFC00) == DMA1_BASE)
	{
		NVIC_IRQChannel_ = DMA1_Channel1_IRQn+ 
		((uint32_t)DMAy_Channelx_-DMA1_Channel1_BASE)/0x14;
	}
	else if(((uint32_t)DMAy_Channelx_&0xFFFFFC00) == DMA2_BASE)
	{
		NVIC_IRQChannel_ = DMA2_Channel1_IRQn+ 
		((uint32_t)DMAy_Channelx_-DMA2_Channel1_BASE)/0x14;
	}
	else
		while(1);//error
	
	memset(&DMA_InitStructure_, sizeof(DMA_InitStructure_), 0);
}

void CDma_Base::ClkConfig(void)
{
	uint32_t RCC_AHBPeriph;
	
	if(((uint32_t)DMAy_Channelx_&0xFFFFFC00) == DMA1_BASE)
		RCC_AHBPeriph = RCC_AHBPeriph_DMA1;
	else if(((uint32_t)DMAy_Channelx_&0xFFFFFC00) == DMA2_BASE)
		RCC_AHBPeriph = RCC_AHBPeriph_DMA2;
	else
		while(1);//error
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph, ENABLE);
}

void CDma_Base::Config(uint32_t PeriphAddr,
									uint32_t MemAddr,
									uint32_t DMA_DIR,
									uint32_t BufferSize,
									uint32_t PDataSize,	
									uint32_t MDataSize,
									uint32_t DMA_Mode)
{
	DMA_Cmd(DMAy_Channelx_, DISABLE);
	
	DMA_InitStructure_.DMA_PeripheralBaseAddr = PeriphAddr;
	DMA_InitStructure_.DMA_MemoryBaseAddr = MemAddr;
	DMA_InitStructure_.DMA_DIR = DMA_DIR;
	DMA_InitStructure_.DMA_BufferSize = BufferSize;
	DMA_InitStructure_.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure_.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure_.DMA_PeripheralDataSize = PDataSize;
	DMA_InitStructure_.DMA_MemoryDataSize = MDataSize;
	DMA_InitStructure_.DMA_Mode = DMA_Mode;
	DMA_InitStructure_.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure_.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMAy_Channelx_, &DMA_InitStructure_);
	
//	DMA_Cmd(DMA1_Channel2, ENABLE);

}
void CDma_Base::Reboot(uint32_t ReloadMemAddr, u16 ReloadBufferSize)
{
	Disable();
	
	if(ReloadBufferSize != 0)
	{
		DMAy_Channelx_->CNDTR = ReloadBufferSize;
		DMA_InitStructure_.DMA_BufferSize = ReloadBufferSize;
	}
	else	//ReloadBufferSize == 0, load the size that used last time
	{
		DMAy_Channelx_->CNDTR = DMA_InitStructure_.DMA_BufferSize;
	}
	
	
	if(ReloadMemAddr != NULL)
	{
		DMAy_Channelx_->CMAR = ReloadMemAddr;
		DMA_InitStructure_.DMA_MemoryBaseAddr = ReloadMemAddr;
	}
	else
		DMAy_Channelx_->CMAR = DMA_InitStructure_.DMA_MemoryBaseAddr;
	
	Enable();
}
/******************************************************************************
* @brief   This part for CPeriph
******************************************************************************/
void CPeriph::EmptyInit()
{
	while(1); //you should not call an empty initfunc!
}
//end of file
