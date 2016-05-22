#ifndef CADC_H_
#define CADC_H_
#include "stm32f10x.h"
#include "singleton.h"

/* ADC_Channel and GPIO map table */
//	ADC1 Channel 		GPIO Port

//	ADC1_Channel0			PA0
//	ADC1_Channel1			PA1
//	ADC1_Channel2			PA2
//	ADC1_Channel3			PA3
//	ADC1_Channel4			PA4
//	ADC1_Channel5			PA5
//	ADC1_Channel6			PA6
//	ADC1_Channel7			PA7

//	ADC1_Channel8			PB0
//	ADC1_Channel9			PB1

//	ADC1_Channel10		PC0
//	ADC1_Channel11		PC1
//	ADC1_Channel12		PC2
//	ADC1_Channel13		PC3
//	ADC1_Channel14		PC4
//	ADC1_Channel15		PC5

#define USE_ADC_1			1	//change USE_ADC_x
#define ADC_TRIG_SRC	ADC_ExternalTrigConv_T4_CC4	//change trig source here

#ifdef USE_ADC_1
	#define REGULAR_ADC 	ADC1
	#define ADC_DMA_CH		DMA1_Channel1
#elif USE_ADC_3
	#define REGULAR_ADC 		ADC3
	#define ADC_DMA_CH		DMA2_Channel3
#else
	#error
#endif

#define ADC_DMA				((DMA_TypeDef *)((uint32_t)ADC_DMA_CH&0xFFFFFC00))

class CAdc
{
public:
	CAdc();
	void add_channel(uint8_t ADC_Channel_x, uint8_t ADC_SampleTime);
	enum{ADC_BUF_SIZE = 50};
	void update();
	
private:
	void InitAdc();
	void set_NbrOfChannel(uint8_t);
	void PinConfig(uint8_t);	//called in add_channel()
	void reboot_dataBufDMA();
	static uint16_t ADC_DataBuf[ADC_BUF_SIZE];
};
typedef NormalSingleton<CAdc> regularAdc;


#endif
//end of file
