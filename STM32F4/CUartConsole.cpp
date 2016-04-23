/********************************************************************************
* @file    CUartConsole.cpp
* @author  Chenxx
* @version V1.0
* @date    2016-04-21
* @brief   this file defines the Console function that can printf with uart.
*	   This is for STM32F4.
********************************************************************************/
#include "CUartConsole.h"

char CUartConsole::TxBuf_[TXBUF_SIZE];

//
//constructor
//
CUartConsole::CUartConsole()
	:bufback_ptr_(TxBuf_)
{
	uint32_t streamx = (((uint32_t)CONSOLE_TX_DMAST&0xFF) - 0x10)/0x18;	// 0-7
	
	if(streamx < 4) TXDMA_IFCR_ = &(CONSOLE_DMA->LIFCR);
	else TXDMA_IFCR_ = &(CONSOLE_DMA->HIFCR);
	switch(streamx % 4)
	{
		case 0:
			TXDMA_IFCR_MASK = 1<<5;
			break;
		case 1:
			TXDMA_IFCR_MASK = 1<<11;
			break;
		case 2:
			TXDMA_IFCR_MASK = 1<<21;
			break;
		case 3:
			TXDMA_IFCR_MASK = 1<<27;
			break;
	}
	
	InitSciGpio();
	InitSci();
}

//
//uint8_t CUartConsole::send_Array(uint8_t* Buf, uint8_t size)
//
uint8_t CUartConsole::send_Array(uint8_t* buf, uint8_t size)
{
//	if(size > SCI_FIFO_LEVEL) size = SCI_FIFO_LEVEL; //is this important?
	
	DMA_Cmd(CONSOLE_TX_DMAST, DISABLE);//data left in buff will be cut
	*TXDMA_IFCR_ = TXDMA_IFCR_MASK;
	CONSOLE_TX_DMAST->NDTR = size;
	CONSOLE_TX_DMAST->M0AR = (uint32_t)buf;
	
	DMA_Cmd(CONSOLE_TX_DMAST, ENABLE);
	return 1;
}

//
//void CUartConsole::InitSciGpio()
//
void CUartConsole::InitSciGpio()
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	uint32_t RCC_AHB1Periph_GPIOx;
	uint8_t GPIO_PinSource_BASE;
	uint8_t GPIO_AF_USARTx;
	GPIO_TypeDef *GPIOx;
	
	if(CONSOLE_IOGROUP == GROUP_A2)
	{
		RCC_AHB1Periph_GPIOx = RCC_AHB1Periph_GPIOA;
		GPIOx = GPIOA;
		GPIO_PinSource_BASE = GPIO_PinSource2;
	}	
	else if(CONSOLE_IOGROUP == GROUP_A9)
	{
		RCC_AHB1Periph_GPIOx = RCC_AHB1Periph_GPIOA;
		GPIOx = GPIOA;
		GPIO_PinSource_BASE = GPIO_PinSource9;
	}
	else if(CONSOLE_IOGROUP == GROUP_B10)
	{
		RCC_AHB1Periph_GPIOx = RCC_AHB1Periph_GPIOB;
		GPIOx = GPIOB;
		GPIO_PinSource_BASE = GPIO_PinSource10;
	}
	else while(1); //undefined!
	
	if(CONSOLE_UART == USART1)	GPIO_AF_USARTx = GPIO_AF_USART1;
	else if(CONSOLE_UART == USART2)	GPIO_AF_USARTx = GPIO_AF_USART2;
	else if(CONSOLE_UART == USART3)	GPIO_AF_USARTx = GPIO_AF_USART3;
	else while(1); //undefined!

	/* open clock of MOSI MISO SCK nCS */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx, ENABLE);
	
	/* Config Pin: MOSI MISO SCK */
	GPIO_PinAFConfig(GPIOx, GPIO_PinSource_BASE, GPIO_AF_USARTx);
	GPIO_PinAFConfig(GPIOx, GPIO_PinSource_BASE + 1, GPIO_AF_USARTx);
	GPIO_PinAFConfig(GPIOx, GPIO_PinSource_BASE + 2, GPIO_AF_USARTx);	
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_0|GPIO_Pin_1)<< GPIO_PinSource_BASE;
	GPIO_Init(GPIOx, &GPIO_InitStructure);
}

//
//void CUsart::InitSci()
//
void CUartConsole::InitSci()
{
	/* init clock of USART */
	if(CONSOLE_UART == USART1) RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	else if(CONSOLE_UART == USART2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	else if(CONSOLE_UART == USART3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	else while(1);
	
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = 115200; 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 
	USART_InitStructure.USART_Parity = USART_Parity_No; 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; 
	USART_Init(CONSOLE_UART, &USART_InitStructure); 

	USART_Cmd(CONSOLE_UART, ENABLE); 
	
	USART_DMACmd(CONSOLE_UART, USART_DMAReq_Tx, ENABLE);
	USART_DMACmd(CONSOLE_UART, USART_DMAReq_Rx, ENABLE);
	
	/* DMA Clock Config */
	uint32_t RCC_AHB1Periph;
	if(CONSOLE_DMA == DMA1) RCC_AHB1Periph = RCC_AHB1Periph_DMA1;
	else if(CONSOLE_DMA == DMA2) RCC_AHB1Periph = RCC_AHB1Periph_DMA2;
	else while(1); //error
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph, ENABLE);
	
	DMA_InitTypeDef DMA_InitStructure;
	
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull; 
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single; 
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_InitStructure.DMA_Channel = DMA_Channel_4; //attention!
	
	/* TX DMA Config */	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(CONSOLE_UART->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)TxBuf_;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = 0;
	
	DMA_Cmd(CONSOLE_TX_DMAST, DISABLE);
	DMA_Init(CONSOLE_TX_DMAST, &DMA_InitStructure);
}

//
//int CUartConsole::printf(const char* fmt, ...)
//
#include <stdarg.h>
#include <stdio.h>
int CUartConsole::printf(const char* fmt, ...)
{
	DMA_Cmd(CONSOLE_TX_DMAST, DISABLE);
	uint8_t dma_ndtr_temp = CONSOLE_TX_DMAST->NDTR;
	if(dma_ndtr_temp == 0) bufback_ptr_ = TxBuf_;
	
	va_list args;
	int n;
	
	va_start(args, fmt);
	n = vsnprintf(bufback_ptr_, TXBUF_SIZE - (bufback_ptr_ - TxBuf_), fmt, args);
	va_end(args);
	
	CONSOLE_TX_DMAST->NDTR += n;
	CONSOLE_TX_DMAST->M0AR = (uint32_t)(bufback_ptr_ - dma_ndtr_temp);
	bufback_ptr_ += n;
	*TXDMA_IFCR_ = TXDMA_IFCR_MASK;
	DMA_Cmd(CONSOLE_TX_DMAST, ENABLE);
	
	return n;
}
//end of file
