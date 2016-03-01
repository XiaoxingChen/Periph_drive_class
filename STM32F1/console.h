/**
******************************************************************************
* @file    console.h
* @author  Chenxx
* @version V1.0
* @date    2016-01-29
* @brief   This file achieve the uart console
******************************************************************************/
#ifndef __CONSOLE_H
#define __CONSOLE_H
#include "stm32f10x.h"
class CUsart;
class console
{
public:
	console(USART_TypeDef* USARTx = USART1, uint32_t baudrate = 115200);
	void Usart_Init(uint32_t baudrate);
	void printf(const char* str);
	void DmaSendArray(u8* array, u16 len);
private:
	CUsart* Usart;
};
extern console console1;
#endif
//end of file
