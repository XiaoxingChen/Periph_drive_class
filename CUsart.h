/**
  ******************************************************************************
  * @file    CUsart.h
  * @author  Chenxx
  * @version V1.2
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
	CInte_Usart(const CUsart* pUsart);
	virtual void EnableLine(uint32_t USART_IT)
		{USART_ITConfig((USART_TypeDef*)Periphx_, USART_IT, ENABLE);}
	virtual void DisableLine(uint32_t USART_IT)
		{USART_ITConfig((USART_TypeDef*)Periphx_, USART_IT, DISABLE);}
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
	virtual ~CDma_Usart()
	{
		if (INTE != NULL) delete INTE;
	}
	virtual void Init(uint32_t PeriphAddr,
		uint32_t MemAddr,
		uint32_t DMA_DIR,
		uint32_t BufferSize,
		uint32_t PDataSize,	
		uint32_t MDataSize,
		uint32_t DMA_Mode,
		CInte_Dma* new_Inte_Dma);
};

/******************************************************************************
* @brief   This part for CUsart
******************************************************************************/
class CUsart
	:public CPeriph
{
public:
	friend void USART1_Init(void);
	enum DmaChType
	{
		DmaChTxd = USART_DMAReq_Tx,
		DmaChRxd = USART_DMAReq_Rx
	};
	CUsart(USART_TypeDef* USARTx, void(*InitFunc)(void));
	virtual ~CUsart()
	{
		if(DMA != NULL) delete DMA;
		if(INTE != NULL) delete INTE;
	}
	void(* Init)();
	void Config(u32 Baudrate);
	void ClkConfig();
	void PinConfig();
	CDmaKit* DMA;
	CInte_Usart* INTE;
		
	USART_TypeDef* Get_USARTx_(void) const
		{return USARTx_;}
	uint8_t Get_NVIC_IRQChannel_(void) const
		{return NVIC_IRQChannel_;}
	DMA_Channel_TypeDef* DmaChCalc(DmaChType DmaCh) const;
	virtual CDma_Base* Creat_CDma(int DmaCh) const
	{
		/* Periph Config */
		int USART_DMAReq_Ch;
		if(DmaCh == DmaChTxd)
			USART_DMAReq_Ch = USART_DMAReq_Tx;
		else if(DmaCh == DmaChRxd)
			USART_DMAReq_Ch = USART_DMAReq_Rx;
		else
			while(1); //error!
		USART_DMACmd(USARTx_, USART_DMAReq_Ch, ENABLE);
		
		return new CDma_Usart(DmaChCalc((DmaChType)DmaCh));
	}
	virtual void Destroy_CDma(int DmaCh) const
	{
		/* Periph Config */
		int USART_DMAReq_Ch;
		if(DmaCh == DmaChTxd)
			USART_DMAReq_Ch = USART_DMAReq_Tx;
		else if(DmaCh == DmaChRxd)
			USART_DMAReq_Ch = USART_DMAReq_Rx;
		else
			while(1); //error!
		USART_DMACmd(USARTx_, USART_DMAReq_Ch, DISABLE);
		
		delete DMA->handle(DmaCh);
	}
private:
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
extern CUsart COM1;
#endif
/*End of File*/
