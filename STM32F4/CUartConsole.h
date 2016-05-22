/********************************************************************************
* @file    CUartConsole.h
* @author  Chenxx
* @version V1.0
* @date    2016-04-21
* @brief   this file defines the Console function that can printf with uart.
*	   This is for STM32F4.
********************************************************************************/
#ifndef _CUARTCONSOLE_H_
#define _CUARTCONSOLE_H_
#include "stm32f4xx.h"
#include "Singleton.h"


/****** UART IOGROUP DMAST reference ******/
//	UARTx		IOGROUP		TX_DMAST

//	UART1 	A9 				DMA2_Stream7
//	UART1 	B6 				DMA2_Stream7

//	UART2 	A2 				DMA1_Stream6
//	UART2 	D5 				DMA1_Stream6

//	UART3 	B10 			DMA1_Stream3
//	UART3 	C10 			DMA1_Stream3
//	UART3 	D8 				DMA1_Stream3

//	UART4 	A0 				DMA1_Stream4
//	UART4 	C10 			DMA1_Stream4

//BSP MACROS
#define CONSOLE_USE_UART1		1//change here
#define CONSOLE_IOGROUP_A9	1//change here
#define CONSOLE_TX_DMAST		DMA2_Stream7 //change here

#ifdef 	CONSOLE_USE_UART1 
	#define 	CONSOLE_UART	USART1	
#elif 	CONSOLE_USE_UART2
	#define 	CONSOLE_UART	USART2	
#elif 	CONSOLE_USE_UART3
	#define 	CONSOLE_UART	USART3
#elif 	CONSOLE_USE_UART4
	#define 	CONSOLE_UART	UART4
#elif 	CONSOLE_USE_UART5
	#define 	CONSOLE_UART	UART5
#endif

#define CONSOLE_DMA				((DMA_TypeDef *)((uint32_t)CONSOLE_TX_DMAST&0xFFFFFC00))

class CUartConsole
{
public:
	CUartConsole();
	~CUartConsole();
	uint8_t send_Array(uint8_t*, uint8_t);
	int printf(const char* fmt, ...) __attribute__((format(printf,2,3)));
	int getch(void);
	enum{TXBUF_SIZE = 100};
	
private:
	void InitSciGpio();
	void InitSci();
	/* interrupt flag clear register */
	volatile uint32_t* TXDMA_IFCR_;
	uint32_t TXDMA_IFCR_MASK;

	static char TxBuf_[TXBUF_SIZE];
	char* bufback_ptr_;
};
typedef NormalSingleton<CUartConsole> Console;
#endif
//end of file
