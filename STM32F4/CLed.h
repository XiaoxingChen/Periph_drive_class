/**
  ******************************************************************************
  * @file    CLed.h
  * @author  Chenxx
  * @version V1.0 
  * @date    2015-09-05
  * @brief   This file the CLed class. For f4xx.
  *	@note		 This version is for STM32F4xx
  ******************************************************************************/
#ifndef __CLED_H
#define __CLED_H

#include "stm32f4xx.h"

#define RCC_AHB1_BSP_LED	RCC_AHB1Periph_GPIOA<< \
				(((uint32_t)BSP_LED_GPIO-(uint32_t)GPIOA)/0x400)
#define BSP_LED_GPIO			GPIOE

#define BSP_LED_START_BIT   (2-1) //0-1

#define BSP_LED1		GPIO_Pin_2
#define BSP_LED2		((uint16_t)0x0000)
#define BSP_LED3		((uint16_t)0x0000)
#define BSP_LED4		((uint16_t)0x0000)
#define BSP_LEDS		(BSP_LED1 | BSP_LED2 | BSP_LED3 | BSP_LED4)  

class CLed
{
	public:
		CLed();
	void On(u8 Led);
	void Off(u8 Led);
	void Toggle(u8 Led);
	private:
		uint8_t Is_On_Set;
		void Set(u8 Led);
		void Reset(u8 Led);
		void CLKConfig(void)
		{
			RCC_AHB1PeriphClockCmd(RCC_AHB1_BSP_LED, ENABLE); 
		}
		void PinConfig(void)
		{
			GPIO_InitTypeDef GPIO_InitStructure;  

			GPIO_InitStructure.GPIO_Pin = BSP_LED1|BSP_LED2|BSP_LED3|BSP_LED4; 
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_Init(BSP_LED_GPIO, &GPIO_InitStructure);	
		}
	
};
extern CLed BspLed;

#endif
