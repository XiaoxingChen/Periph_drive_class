#include "CAdc.h"
#include <string.h>

uint16_t CAdc::ADC_DataBuf[CAdc::ADC_BUF_SIZE];
/**
  * @brief  Constructor of CAdc
  * @param  None
  * @retval None
  */
CAdc::CAdc()
{
	InitAdc();
	add_channel(1, ADC_SampleTime_1Cycles5);
}

/**
  * @brief  Initialize the ADC peripherial
  * @param  None
  * @retval None
  */
void CAdc::InitAdc()
{
	
	/* clock configuration */
	uint32_t RCC_APB2Periph_ADCx;
	#ifdef USE_ADC_1
		RCC_APB2Periph_ADCx = RCC_APB2Periph_ADC1;
	#elif USE_ADC_3
		RCC_APB2Periph_ADCx = RCC_APB2Periph_ADC3;
	#endif
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADCx , ENABLE);
	
	/* ADC Configuration */
	ADC_InitTypeDef ADC_InitStructure; 

	ADC_Cmd(REGULAR_ADC, DISABLE);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; 
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; 
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;	//Trig mode
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(REGULAR_ADC, &ADC_InitStructure); 

	ADC_Cmd(REGULAR_ADC, ENABLE);
	ADC_DMACmd(REGULAR_ADC, ENABLE);				//enable DMA
	ADC_StartCalibration(REGULAR_ADC);
	while(ADC_GetCalibrationStatus(REGULAR_ADC) == SET);
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);	//enable external Trig
	
	/* DMA Clock Config */
	uint32_t RCC_AHB1Periph;
	if(ADC_DMA == DMA1) RCC_AHB1Periph = RCC_AHBPeriph_DMA1;
	else if(ADC_DMA == DMA2) RCC_AHB1Periph = RCC_AHBPeriph_DMA2;
	else while(1); //error
	RCC_AHBPeriphClockCmd(RCC_AHB1Periph, ENABLE);
	
	DMA_InitTypeDef DMA_InitStructure;
	
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(REGULAR_ADC->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_DataBuf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = ADC_BUF_SIZE;
	
	DMA_Cmd(ADC_DMA_CH, DISABLE);
	DMA_Init(ADC_DMA_CH, &DMA_InitStructure);
	DMA_Cmd(ADC_DMA_CH, ENABLE);
	
	/* Timer Config */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	TIM_TimeBaseStructure.TIM_Period = 99;
	TIM_TimeBaseStructure.TIM_Prescaler = 71;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 50;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	
	TIM_Cmd(TIM2, ENABLE);
	TIM_InternalClockConfig(TIM2);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable); 
	TIM_UpdateDisableConfig(TIM2, DISABLE);
}

/**
  * @brief  Add an ADC channel.
	* @param  ADC_Channel_x: Channel that wants to add.
	* @param  ADC_SampleTime: Sample time of this channel
  * @retval None
  */
void CAdc::add_channel(uint8_t ADC_Channel_x, uint8_t ADC_SampleTime)
{
	uint8_t ADC_isOn = ADC_GetFlagStatus(REGULAR_ADC, ADC_FLAG_STRT);
	
	if(ADC_isOn)
		ADC_Cmd(REGULAR_ADC, DISABLE);
	
	uint8_t currentChannelNbr = (REGULAR_ADC->SQR1 >> 20)+1;
	if(currentChannelNbr != 1 || (REGULAR_ADC->SQR3&0xF) != 0) 
		currentChannelNbr++;
	
	ADC_RegularChannelConfig(REGULAR_ADC, ADC_Channel_x, currentChannelNbr, ADC_SampleTime); 
	set_NbrOfChannel(currentChannelNbr);
	PinConfig(ADC_Channel_x);
	ADC_Cmd(REGULAR_ADC, ENABLE);
	
	if(ADC_isOn)
		ADC_Cmd(REGULAR_ADC, ENABLE);
}

/**
  * @brief  Set the number of ADC channels.
	* @param  Nbr: Change the current number to this parameter.
  * @retval None
  */
void CAdc::set_NbrOfChannel(uint8_t Nbr)
{
	if(Nbr > 16) return;
	uint32_t temp = (Nbr-1)<<20;
	REGULAR_ADC->SQR1 &= ~(0xF<<20);
	REGULAR_ADC->SQR1 |= temp;
}

/**
  * @brief  Configuration the GPIO of the ADC channel.
	* @param  ADC_Channel: Channel user wants to add.
  * @retval None
  */
void CAdc::PinConfig(uint8_t ADC_Channel)
{
//	assert_param(IS_ADC_CHANNEL(ADC_Channel));
	uint32_t RCC_APB2Periph;
	uint16_t GPIO_Pin;
	GPIO_TypeDef* GPIOx;
	switch (ADC_Channel)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			GPIOx = GPIOA;
			GPIO_Pin = 1 << ADC_Channel;
			RCC_APB2Periph = RCC_APB2Periph_GPIOA;
			break;
		
		case 8:
		case 9:
			GPIO_Pin = 1 << (ADC_Channel-8);
			GPIOx = GPIOB;
			RCC_APB2Periph = RCC_APB2Periph_GPIOB;
			break;
		
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			GPIO_Pin = 1 << (ADC_Channel-10);
			GPIOx = GPIOC;
			RCC_APB2Periph = RCC_APB2Periph_GPIOC;
			break;
		
		default:
			while(1); //assert failed!				
	}
	GPIO_InitTypeDef GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph|RCC_APB2Periph_AFIO, ENABLE); 
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
	GPIO_Init(GPIOx, &GPIO_InitStructure); 
}

/**
  * @brief  Update Parameters
	* @param  None
  * @retval None
  */
void CAdc::update()
{
	memset(ADC_DataBuf, 0, sizeof(uint16_t)*ADC_BUF_SIZE);
	reboot_dataBufDMA();
}

/**
  * @brief  reboot the DMA working for ADC_DataBuf
	* @param  None
  * @retval None
  */
void CAdc::reboot_dataBufDMA()
{
	DMA_Cmd(ADC_DMA_CH, DISABLE);
	ADC_DMA_CH->CNDTR = ADC_BUF_SIZE;
	DMA_Cmd(ADC_DMA_CH, ENABLE);
}
//end of file
