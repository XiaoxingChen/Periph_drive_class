/********************************************************************************
* @file    CSinWaveGtor.h
* @author  Chenxx
* @version V1.0
* @date    2016-05-13
* @brief   this file defines the DAC function that can printf with uart.
*	   This is for STM32F1.
********************************************************************************/
#ifndef CSINWAVEGTOR_H
#define CSINWAVEGTOR_H
#include "stm32f10x.h"
#include "Singleton.h"

//BSP MACROS
#define SINWAVEGTOR_ENABLE_CH1		1		//change 1 or 0
#define SINWAVEGTOR_ENABLE_CH2		0		//change 1 or 0
#define SINWAVEGTOR_USE_TIMER6		1		//change TIMERx

#define SINWAVEGTOR_CH1_BUFLEN		1000//change number
#define SINWAVEGTOR_CH2_BUFLEN		1000//change number

#define SINWAVEGTOR_CH1_DMACH			DMA2_Channel3 
#define SINWAVEGTOR_CH2_DMACH			DMA2_Channel4 

#ifdef 	SINWAVEGTOR_USE_TIMER6
	#define SINWAVEGTOR_TIMER				TIM6
#elif		SINWAVEGTOR_USE_TIMER7
	#define SINWAVEGTOR_TIMER				TIM7
#endif

#if (SINWAVEGTOR_ENABLE_CH1 || SINWAVEGTOR_ENABLE_CH2) == 0
#error
#endif

class CSinWaveGtor
{
	public:
	CSinWaveGtor();
	void set_ConvertPeriod_us(uint16_t);
	
	/**
  * @brief  Launch ADC by enable the trigger timer
  * @param  None
  * @retval None
  */
	inline void start() 
	{TIM_Cmd(SINWAVEGTOR_TIMER, ENABLE);}
	/**
  * @brief  Stop ADC by disable the trigger timer
  * @param  None
  * @retval None
  */
	inline void stop()
	{TIM_Cmd(SINWAVEGTOR_TIMER, DISABLE);}
	void update_AdcChannelBuf(DMA_Channel_TypeDef *);
	
	private:
	void InitAdcGpio();
	void InitAdc();
	void switch_AdcChannelBuf(DMA_Channel_TypeDef *);
	uint16_t update_AdcChannelBuf(DMA_Channel_TypeDef *, 
		int16_t offset_11b, 
		uint16_t gain_16b, 
		uint32_t phase_T_32b,
		uint16_t freq_Hz);
	
	
#if SINWAVEGTOR_ENABLE_CH1
	bool Ch1CurrentBuf_;
	static uint16_t Ch1Buf_[2][SINWAVEGTOR_CH1_BUFLEN];
	uint16_t Ch1EffectiveBufLen_;
#endif
	
#if SINWAVEGTOR_ENABLE_CH2
	bool Ch2CurrentBuf_;
	static uint16_t Ch2Buf_[2][SINWAVEGTOR_CH2_BUFLEN];
	uint16_t Ch2EffectiveBufLen_;
#endif
	
	/* this part for single channel amplitude control */
	int16_t* standardSinTab_;
	uint16_t waveAmplitude_16b_;
	int16_t waveOffset_11b_;
};
typedef NormalSingleton<CSinWaveGtor> sinWaveGenerator;
extern int16_t DACsinTab[1000];
#endif
//end of file
