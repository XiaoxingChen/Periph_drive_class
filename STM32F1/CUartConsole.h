/********************************************************************************
* @file    CUartConsole.h
* @author  Chenxx
* @version V1.0
* @date    2016-05-13
* @brief   this file defines the Console function that can printf with uart.
*	   This is for STM32F1.
********************************************************************************/
#ifndef _CUARTCONSOLE_H_
#define _CUARTCONSOLE_H_
#include "stm32f10x.h"
#include "Singleton.h"

//BSP MACROS
#define CONSOLE_USE_UART1		1//change here
#define CONSOLE_IOGROUP_A9	1//change here
#define CONSOLE_TX_DMACH		DMA1_Channel4 //change here

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

#define CONSOLE_DMA				((DMA_TypeDef *)((uint32_t)CONSOLE_TX_DMACH&0xFFFFFC00))

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

	static char TxBuf_[TXBUF_SIZE];
	char* bufback_ptr_;
};
typedef NormalSingleton<CUartConsole> Console;
#endif
//end of file
