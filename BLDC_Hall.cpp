/**
  ******************************************************************************
  * @file    BLDC_Hall.cpp
  * @author  Chenxx
  * @version V1.0
  * @date    2016-01-30
  * @brief   This file achieve the BLDC_Hall
  ******************************************************************************/
#include "BLDC_Hall.h"
#include "CExti.h"
BLDC_Hall::BLDC_Hall(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	//check if only 3 continuou pins were selected
	uint16_t PinTemp = GPIO_Pin;
	while(1)
	{
		if(PinTemp == 0x7)
			break;
		else if(PinTemp == 0)
			while(1); //error!
		PinTemp >>= 1;
		PIN_POS++;
	}
	Exti_ = new CExti(GPIOx, GPIO_Pin, Exti_->EmptyInit);
	Exti_Init();
}

void BLDC_Hall::Exti_Init()
{
	Exti_->ClkConfig();
	Exti_->PinConfig();
	Exti_->Config(EXTI_Trigger_Rising_Falling);
	
	Exti_->INTE = new CInte_Exti(Exti_);
	Exti_->INTE->Init(1, 1, Exti_->Get_ExtiLine());
}

uint8_t BLDC_Hall::Get_halldata() const
{
	return (Exti_->Get_GPIOx_()->IDR >> PIN_POS)&0x7;
}

//BLDC_Hall Hall_Port(GPIOC, GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12);
//end of file
