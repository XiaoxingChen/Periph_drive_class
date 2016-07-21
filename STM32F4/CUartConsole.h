/********************************************************************************
* @file    CUartConsole.h
* @author  Chenxx
* @version V1.0
* @date    2016-04-21
* @brief   this file defines the Console function that can printf with uart.
*	   This is for STM32F4.
* release information: 
*	2016/07/01: chenxx add noneDMA mode.
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
#define CONSOLE_DMA_MODE		1
//#define CONSOLE_NONEDMA_MODE	1
#define CONSOLE_USE_UART2			1//change here
#define CONSOLE_IOGROUP_A2		1//change here
#define CONSOLE_TX_DMAST			DMA1_Stream6 //change here

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
	void postErr();

#ifdef CONSOLE_NONEDMA_MODE
	void run();
#endif
	
private:
	void InitSciGpio();
	void InitSci();
	/* interrupt flag clear register */
	volatile uint32_t* TXDMA_IFCR_;
	uint32_t TXDMA_IFCR_MASK;
	uint16_t overflowCounter_;

	static char TxBuf_[TXBUF_SIZE];
	char* bufback_ptr_;	//Queue: [front, back)
#ifdef CONSOLE_NONEDMA_MODE
	static char vsnpritfBuf_[TXBUF_SIZE];
	char* buffront_ptr_;//Queue: [front, back)
#endif
};
typedef NormalSingleton<CUartConsole> Console;
#define postErr(msg) printf("Error: %s(%d)-%s(): %s\r\n", __FILE__, __LINE__, __FUNCTION__, msg)
#endif
//end of file
