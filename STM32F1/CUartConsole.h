/********************************************************************************
* @file    CUartConsole.h
* @author  Chenxx
* @version V1.0
* @date    2016-05-13
* @brief   this file defines the Console function that can printf with uart.
*	   This is for STM32F1.
* release information:
*	2016/07/21 chenxx: add postErr() macro function
********************************************************************************/
#ifndef _CUARTCONSOLE_H_
#define _CUARTCONSOLE_H_
#include "stm32f10x.h"
#include "Singleton.h"

//BSP MACROS
#define CONSOLE_DMA_MODE		1
//#define CONSOLE_NONEDMA_MODE	1
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
	void postErr();
	
	#ifdef CONSOLE_NONEDMA_MODE
	void run();
	#endif
	
private:
	void InitSciGpio();
	void InitSci();

	uint16_t overflowCounter_;

	static char TxBuf_[TXBUF_SIZE];
	char* bufback_ptr_;//Queue: [front, back)
#ifdef CONSOLE_NONEDMA_MODE
	static char vsnpritfBuf_[TXBUF_SIZE];
	char* buffront_ptr_;//Queue: [front, back)
#endif
};
typedef NormalSingleton<CUartConsole> Console;
#define postErr(msg) printf("Error: %s(%d)-%s(): %s\r\n", __FILE__, __LINE__, __FUNCTION__, msg)
#endif
//end of file
