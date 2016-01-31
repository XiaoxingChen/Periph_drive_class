/**
  ******************************************************************************
  * @file    CUsart.cpp
  * @author  Chenxx
  * @version V1.1
  * @date    2015-10-17
  * @brief   This file provides all the Usart functions.
  ******************************************************************************/
#include "stm32f10x.h"
#include "CUsart.h"
#include <string.h>
/******************************************************************************
* @brief   This part for CInte_Usart
******************************************************************************/
CInte_Usart::CInte_Usart(const CUsart* pUsart)
{
	NVIC_IRQChannel_ = pUsart->Get_NVIC_IRQChannel_();
	Periphx_ = (void*)pUsart->Get_USARTx_();
}
/******************************************************************************
* @brief   This part for CDma_Usart
******************************************************************************/
void CDma_Usart::Init(uint32_t PeriphAddr,
							uint32_t MemAddr,
							uint32_t DMA_DIR,
							uint32_t BufferSize,
							uint32_t PDataSize,	
							uint32_t MDataSize,
							uint32_t DMA_Mode,
							CInte_Dma* new_Inte_Dma)
{
	INTE = new_Inte_Dma;
	ClkConfig();
	Config(PeriphAddr,
				MemAddr,
				DMA_DIR,
				BufferSize,
				PDataSize,	
				MDataSize,
				DMA_Mode);
	Enable();
}
/******************************************************************************
* @brief   This part for CUsart
******************************************************************************/
CUsart::CUsart(USART_TypeDef* USARTx, void(*InitFunc)(void)):
	Init(InitFunc),DMA(NULL),INTE(NULL),USARTx_(USARTx)
{
	if(USARTx_ == USART1)
		NVIC_IRQChannel_ = USART1_IRQn;
	else if(USARTx_ == USART2)
		NVIC_IRQChannel_ = USART2_IRQn;
	else if(USARTx_ == USART3)
		NVIC_IRQChannel_ = USART3_IRQn;
	else
		while(1);//error
}
void CUsart::ClkConfig(void)
{
	//			USART1	APB2;	GPIOA	APB2;
	//			USART2	APB1;	GPIOA	APB2;
	//			USART3	APB1;	GPIOB	APB2;
	if (USARTx_ == USART1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	}
	else if(USARTx_ == USART2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	}
	else if(USARTx_ == USART3)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	}
}
void CUsart::PinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;  
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (USARTx_ == USART1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 
		GPIO_Init(GPIOA, &GPIO_InitStructure); 
	}
	else if(USARTx_ == USART2)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	}
	else if(USARTx_ == USART3)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	}
}
void CUsart::Config(u32 Baudrate)
{
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = Baudrate; 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 
	USART_InitStructure.USART_Parity = USART_Parity_No; 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; 
	USART_Init(USARTx_, &USART_InitStructure); 

	USART_Cmd(USARTx_, ENABLE); 
}
DMA_Channel_TypeDef* CUsart::DmaChCalc(DmaChType DmaCh) const
{
	if(USARTx_ == USART1)
	{
		if(DmaCh == DmaChTxd)
			return DMA1_Channel4;
		else if(DmaCh == DmaChRxd)
			return DMA1_Channel5;
		else
			while(1);
	}
	else
		while(1);
}
/******************************************************************************
* @brief   This part for Init Function
******************************************************************************/
void USART1_Init(void)
{
	CUsart& Usartx = COM1;
	Usartx.ClkConfig();
	Usartx.PinConfig();
	Usartx.Config(115200);
	
//	Usartx.DMA = new CDmaKit_Usart(COM1);
	Usartx.DMA = new CDmaKit(&COM1);
	COM1.DMA->add(CUsart::DmaChTxd);

	Usartx.DMA->handle(CUsart::DmaChTxd)->Init((uint32_t)&(COM1.Get_USARTx_()->DR),
					(uint32_t)"a",
					DMA_DIR_PeripheralDST,
					1,
					DMA_MemoryDataSize_Byte,
					DMA_MemoryDataSize_Byte,
					DMA_Mode_Normal,
					new CInte_Dma(Usartx.DMA->handle(CUsart::DmaChTxd)));
	Usartx.DMA->handle(CUsart::DmaChTxd)->INTE->Init(1,1/*,DMA_IT_TC*/);
//	Usartx.INTE = new CInte_Usart(COM1);
//	Usartx.INTE->Init(1,1,USART_IT_RXNE);
}
CUsart COM1(USART1, USART1_Init);
/*End of File*/
