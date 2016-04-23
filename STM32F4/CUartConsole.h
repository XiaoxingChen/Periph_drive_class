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
//BSP MACROS
#define CONSOLE_UART				USART1
#define CONSOLE_IOGROUP			GROUP_A9
#define CONSOLE_TX_DMAST		DMA2_Stream7
#define CONSOLE_DMA				((DMA_TypeDef *)((uint32_t)

CONSOLE_TX_DMAST&0xFFFFFC00))

class CUartConsole
{
public:
	enum IOGroup_Type
	{
		GROUP_A2 = 0,
		GROUP_A9,
		GROUP_B10
	};
	CUartConsole();
	~CUartConsole();
	uint8_t send_Array(uint8_t*, uint8_t);
	int printf(const char* fmt, ...) __attribute__((format(printf,2,3)));
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
