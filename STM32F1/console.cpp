/**
  ******************************************************************************
  * @file    console.cpp
  * @author  Chenxx
  * @version V1.0
  * @date    2016-01-29
  * @brief   This file achieve the uart console
  ******************************************************************************/
#include "console.h"
#include "CUsart.h"

console::console(USART_TypeDef* USARTx, uint32_t baudrate)
{
	Usart = new CUsart(USARTx, Usart->EmptyInit);
	Usart_Init(baudrate);
}

void console::Usart_Init(uint32_t baudrate)
{
	Usart->ClkConfig();
	Usart->PinConfig();
	Usart->Config(baudrate);
	
	Usart->DMA = new CDmaKit(Usart);
	Usart->DMA->add(CUsart::DmaChTxd);
	
	Usart->DMA->handle(CUsart::DmaChTxd)->Init((uint32_t)&(COM1.Get_USARTx_()->DR),
		(uint32_t)"a",
		DMA_DIR_PeripheralDST,
		1,
		DMA_MemoryDataSize_Byte,
		DMA_MemoryDataSize_Byte,
		DMA_Mode_Normal,
		NULL);
}
void console::DmaSendArray(u8* array, u16 len)
{
	Usart->DMA->handle(CUsart::DmaChTxd)->Reboot((uint32_t)array, len);
}
void console::printf(const char* str)
{
	DmaSendArray((u8*)str, strlen(str));
}
//void console::Init()
//{
//	Usart = new CUsart(USARTx_, Usart->EmptyInit);
//	Usart_Init();
//}
console console1;
	
//end of file
