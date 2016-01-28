/**
  ******************************************************************************
  * @file    CUsart.h
  * @author  Chenxx
  * @version V1.1
  * @date    2015-09-05
  * @brief   This file defines the CUsart class.
  ******************************************************************************/
#ifndef __CUSART_HPP
#define __CUSART_HPP

#include "stm32f10x.h"
#include "CPeriph.h"
class CUsart;
class CDmaKit_Usart;
/******************************************************************************
* @brief   This part for CInte_Usart
******************************************************************************/
class CInte_Usart
	:public CInte_Base
{
public:
	CInte_Usart()
	{}
	CInte_Usart(const CUsart& OUsart);
		
	inline void Init(uint8_t PrePriority, uint8_t SubPriority)
	{
		NvicConfig(PrePriority, SubPriority);
	}
	inline void Init(uint8_t PrePriority, uint8_t SubPriority, uint32_t USART_IT)
	{
		NvicConfig(PrePriority, SubPriority);
		EnableLine(USART_IT);
	}
	void EnableLine(uint16_t USART_IT)
		{USART_ITConfig(USART1, USART_IT, ENABLE);}
	void DisableLine(uint16_t USART_IT)
		{USART_ITConfig(USART1, USART_IT, DISABLE);}
private:
	USART_TypeDef* USARTx_;
};
/******************************************************************************
* @brief   This part for CDma_Usart
******************************************************************************/
class CDma_Usart
	:public CDma_Base
{
public:
	/*default constructor*/
	CDma_Usart()
	{}
	/*mostly used constructor*/
	CDma_Usart(DMA_Channel_TypeDef* DMAy_Channelx)
		:CDma_Base(DMAy_Channelx)
	{}
	/*copy constructor*/
	CDma_Usart(const CDma_Usart& other)
		:CDma_Base(other.GetDMAy_Channelx_())		
	{}
	void PeriphConfig(void);
	void Init(uint32_t PeriphAddr,
							uint32_t MemAddr,
							uint32_t DMA_DIR,
							uint32_t BufferSize,
							uint32_t PDataSize,	
							uint32_t MDataSize,
							uint32_t DMA_Mode,
							CInte_Dma* OInte_Dma);
private:
};

/******************************************************************************
* @brief   This part for CUsart
******************************************************************************/
class CUsart
//	:public CPeriph<CInte_Usart, CDmaKit_Usart>
	:public CPeriph<CInte_Usart, CDma_Usart, CUsart>
{
public:
	friend void USART1_Init(void);
	enum DmaChType
	{
		DmaChTxd = USART_DMAReq_Tx,
		DmaChRxd = USART_DMAReq_Rx
	};
	CUsart(USART_TypeDef* USARTx, void(*InitFunc)(void));
	void Config(u32 Baudrate);
	void ClkConfig();
	void PinConfig();
		
	USART_TypeDef* Get_USARTx_(void) const
		{return USARTx_;}
	uint8_t Get_NVIC_IRQChannel_(void) const
		{return NVIC_IRQChannel_;}
	void printf(const char* str)
		{DMASendStr(str);}
	void DMASendArray(u8* array, u16 len);
	DMA_Channel_TypeDef* DmaChCalc(DmaChType DmaCh) const;
private:
	void DMASendStr(const char* str);
	USART_TypeDef* USARTx_;
	uint8_t NVIC_IRQChannel_;
};
/******************************************************************************
* @brief   This part for CDmaKit_Usart
******************************************************************************/
//class CDmaKit_Usart
//	:public kit<CDma_Usart>
//{
//public:		
//	CDmaKit_Usart(const CUsart& OUsart);
//	~CDmaKit_Usart()
//	{}
//	void add(CUsart::DmaChType DmaCh);		
//private:
//	const CUsart& OUsart_;
//};
/******************************************************************************
* @brief   This part for extern declaration
******************************************************************************/
//extern CUsart COM1;
#endif
/*End of File*/
