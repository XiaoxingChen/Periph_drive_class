/********************************************************************************
* @file    CSinWaveGtor.cpp
* @author  Chenxx
* @version V1.0
* @date    2016-05-13
* @brief   this file defines the DAC function that can printf with uart.
*	   This is for STM32F1.
********************************************************************************/
#include "CSinWaveGtor.h"
#include <string.h>

#if SINWAVEGTOR_ENABLE_CH1
	uint16_t CSinWaveGtor::Ch1Buf_[2][SINWAVEGTOR_CH1_BUFLEN];
#endif
#if SINWAVEGTOR_ENABLE_CH2
	uint16_t CSinWaveGtor::Ch2Buf_[2][SINWAVEGTOR_CH2_BUFLEN];
#endif
/**
  * @brief  Constructor of CSinWaveGtor
  * @param  None
  * @retval None
  */
	CSinWaveGtor::CSinWaveGtor():
#if SINWAVEGTOR_ENABLE_CH1
	Ch1CurrentBuf_(0),
	Ch1EffectiveBufLen_(SINWAVEGTOR_CH1_BUFLEN),
#endif
#if SINWAVEGTOR_ENABLE_CH2
	Ch2EffectiveBufLen_(SINWAVEGTOR_CH2_BUFLEN),
#endif
	standardSinTab_(DACsinTab),
	waveAmplitude_16b_(0x0FFF),
	waveOffset_11b_(0)
{
	InitAdcGpio();
	InitAdc();
	memset(Ch1Buf_, 4095, SINWAVEGTOR_CH1_BUFLEN);
	memset(&Ch1Buf_[1][0], 2048, SINWAVEGTOR_CH1_BUFLEN);
//	memset(Ch2Buf_, 128, SINWAVEGTOR_CH2_BUFLEN<<1);
}

/**
  * @brief  Initializes the GPIO of Adc
  * @param  None
  * @retval None
  */
void CSinWaveGtor::InitAdcGpio()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Pin = 0; 
	
#if SINWAVEGTOR_ENABLE_CH1
	GPIO_InitStructure.GPIO_Pin |= GPIO_Pin_4; 
#endif
	
#if SINWAVEGTOR_ENABLE_CH2
	GPIO_InitStructure.GPIO_Pin |= GPIO_Pin_5; 
#endif
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief  Initializes Adc
  * @param  None
  * @retval None
  */
void CSinWaveGtor::InitAdc()
{
	/* dac init */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );
	DAC_InitTypeDef DAC_InitType;
	
#ifdef 	SINWAVEGTOR_USE_TIMER6
	DAC_InitType.DAC_Trigger = DAC_Trigger_T6_TRGO; 					//trig by timer6
#elif		SINWAVEGTOR_USE_TIMER7
	DAC_InitType.DAC_Trigger = DAC_Trigger_T7_TRGO; 					//trig by timer7
#endif
	
	DAC_InitType.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
	DAC_InitType.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	
#if	SINWAVEGTOR_ENABLE_CH1
	DAC_Init(DAC_Channel_1,&DAC_InitType); 
	DAC_Cmd(DAC_Channel_1, ENABLE);
	DAC_DMACmd(DAC_Channel_1, ENABLE);  
#endif
	
#if	SINWAVEGTOR_ENABLE_CH2
	DAC_Init(DAC_Channel_2,&DAC_InitType); 
	DAC_Cmd(DAC_Channel_2, ENABLE);
	DAC_DMACmd(DAC_Channel_2, ENABLE);  
#endif
	
	/* timer init */
#ifdef 	SINWAVEGTOR_USE_TIMER6
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
#elif		SINWAVEGTOR_USE_TIMER7
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
#endif
	TIM_PrescalerConfig(SINWAVEGTOR_TIMER, 71, TIM_PSCReloadMode_Update); 
	TIM_SetAutoreload(SINWAVEGTOR_TIMER,10000ul - 1ul);
	TIM_SelectOutputTrigger(SINWAVEGTOR_TIMER, TIM_TRGOSource_Update);
	TIM_Cmd(SINWAVEGTOR_TIMER, DISABLE);	//do not start at first
	
	/* dma init */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	 
	DMA_InitTypeDef DMA_InitStructure;

	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; 
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
#if SINWAVEGTOR_ENABLE_CH1
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(DAC->DHR12R1);
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Ch1Buf_;
	DMA_InitStructure.DMA_BufferSize = SINWAVEGTOR_CH1_BUFLEN;
	
	DMA_DeInit(SINWAVEGTOR_CH1_DMACH);
	DMA_Init(SINWAVEGTOR_CH1_DMACH, &DMA_InitStructure);
	DMA_Cmd(SINWAVEGTOR_CH1_DMACH, ENABLE); 
#endif

#if SINWAVEGTOR_ENABLE_CH2
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(DAC->DHR12R2);
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Ch2Buf_;
	DMA_InitStructure.DMA_BufferSize = SINWAVEGTOR_CH2_BUFLEN;
	
	DMA_DeInit(SINWAVEGTOR_CH2_DMACH);
	DMA_Init(SINWAVEGTOR_CH2_DMACH, &DMA_InitStructure);
	DMA_Cmd(SINWAVEGTOR_CH2_DMACH, ENABLE); 
#endif
}

/**
  * @brief  switch adc channel buffer
	* @param  CH: channel number that wants to change
  * @retval None
  */
void CSinWaveGtor::switch_AdcChannelBuf(DMA_Channel_TypeDef * CH)
{
	DMA_Cmd(CH, DISABLE);
	if(CH == SINWAVEGTOR_CH1_DMACH)
	{
#if SINWAVEGTOR_ENABLE_CH1 
		Ch1CurrentBuf_ = !Ch1CurrentBuf_;
		CH->CNDTR = Ch1EffectiveBufLen_;
		CH->CMAR = (uint32_t)&Ch1Buf_[Ch1CurrentBuf_][0];
#endif
	}
	else if(CH == SINWAVEGTOR_CH2_DMACH)
	{
#if SINWAVEGTOR_ENABLE_CH2
		Ch2CurrentBuf_ = !Ch2CurrentBuf_;
		CH->CNDTR = Ch2EffectiveBufLen_;
		CH->CMAR = (uint32_t)&Ch2Buf_[Ch2CurrentBuf_][0];
#endif
	}
	DMA_Cmd(CH, ENABLE); 
}

/**
  * @brief  Stop ADC by disable the trigger timer
	* @param  CH:
	* @param  offset_11b:
	* @param  gain_16b:
	* @param  phase_T_32b:
	* @param  freq_Hz:
  * @retval None
  */
uint16_t update_AdcChannelBuf(DMA_Channel_TypeDef * CH, 
		int16_t offset_11b, 
		uint16_t gain_16b, 
		uint32_t phase_T_32b,
		uint16_t freq_Hz){return 0;}

/**
  * @brief  Stop ADC by disable the trigger timer
	* @param  CH:
  * @retval None
  */
void CSinWaveGtor::update_AdcChannelBuf(DMA_Channel_TypeDef * CH)
{
	uint16_t* targetBuf;
	uint16_t targetBufLen;
	if(CH == SINWAVEGTOR_CH1_DMACH)
	{
	#if SINWAVEGTOR_ENABLE_CH1 
		targetBuf = &Ch1Buf_[0][0];
		targetBufLen = SINWAVEGTOR_CH1_BUFLEN;
	#endif
	}
	else if(CH == SINWAVEGTOR_CH2_DMACH)
	{
	#if SINWAVEGTOR_ENABLE_CH2 
		targetBuf = &Ch2Buf_[0][0];
		targetBufLen = SINWAVEGTOR_CH2_BUFLEN;
	#endif
	}
	int32_t tempValue;
	for(int i = 0; i < targetBufLen; i++)
	{
		tempValue = (int32_t(standardSinTab_[i])*waveAmplitude_16b_>>16) + 0x800 + waveOffset_11b_;
		if(tempValue > 4095) tempValue = 4095;
		else if(tempValue < 0) tempValue = 0;
		
		targetBuf[i] = tempValue;
	}

}

/**
  * @brief  Stop ADC by disable the trigger timer
  * @param  None
  * @retval None
  */
void CSinWaveGtor::set_ConvertPeriod_us(uint16_t period)
{
	stop();
	TIM_SetAutoreload(SINWAVEGTOR_TIMER, period-1);
	start();
}

//	stm32 DAC sin table
//	points number:1000
int16_t DACsinTab[1000] = {

	0, 12, 25, 38, 51, 64, 77, 90, 102, 115, 128, 141, 154, 167, 179, 192, 
	205, 218, 231, 243, 256, 269, 282, 294, 307, 320, 333, 345, 358, 371, 383, 396, 
	409, 421, 434, 446, 459, 471, 484, 496, 509, 521, 534, 546, 559, 571, 583, 596, 
	608, 620, 632, 645, 657, 669, 681, 693, 705, 717, 729, 741, 753, 765, 777, 789, 
	801, 813, 825, 836, 848, 860, 871, 883, 895, 906, 918, 929, 941, 952, 963, 975, 
	986, 997, 1009, 1020, 1031, 1042, 1053, 1064, 1075, 1086, 1097, 1108, 1119, 1129, 1140, 1151, 
	1161, 1172, 1182, 1193, 1203, 1214, 1224, 1234, 1245, 1255, 1265, 1275, 1285, 1295, 1305, 1315, 
	1325, 1334, 1344, 1354, 1363, 1373, 1382, 1392, 1401, 1411, 1420, 1429, 1438, 1448, 1457, 1466, 
	1475, 1484, 1492, 1501, 1510, 1519, 1527, 1536, 1544, 1553, 1561, 1569, 1577, 1586, 1594, 1602, 
	1610, 1618, 1626, 1633, 1641, 1649, 1656, 1664, 1671, 1679, 1686, 1693, 1700, 1708, 1715, 1722, 
	1729, 1736, 1742, 1749, 1756, 1762, 1769, 1775, 1782, 1788, 1794, 1800, 1806, 1812, 1818, 1824, 
	1830, 1836, 1841, 1847, 1853, 1858, 1863, 1869, 1874, 1879, 1884, 1889, 1894, 1899, 1904, 1908, 
	1913, 1918, 1922, 1926, 1931, 1935, 1939, 1943, 1947, 1951, 1955, 1959, 1963, 1966, 1970, 1973, 
	1977, 1980, 1983, 1986, 1989, 1992, 1995, 1998, 2001, 2004, 2006, 2009, 2011, 2013, 2016, 2018, 
	2020, 2022, 2024, 2026, 2028, 2030, 2031, 2033, 2034, 2036, 2037, 2038, 2040, 2041, 2042, 2043, 
	2043, 2044, 2045, 2046, 2046, 2046, 2047, 2047, 2047, 2047, 2047, 2047, 2047, 2047, 2047, 2046, 
	2046, 2046, 2045, 2044, 2043, 2043, 2042, 2041, 2040, 2038, 2037, 2036, 2034, 2033, 2031, 2030, 
	2028, 2026, 2024, 2022, 2020, 2018, 2016, 2013, 2011, 2009, 2006, 2004, 2001, 1998, 1995, 1992, 
	1989, 1986, 1983, 1980, 1977, 1973, 1970, 1966, 1963, 1959, 1955, 1951, 1947, 1943, 1939, 1935, 
	1931, 1926, 1922, 1918, 1913, 1908, 1904, 1899, 1894, 1889, 1884, 1879, 1874, 1869, 1863, 1858, 
	1853, 1847, 1841, 1836, 1830, 1824, 1818, 1812, 1806, 1800, 1794, 1788, 1782, 1775, 1769, 1762, 
	1756, 1749, 1742, 1736, 1729, 1722, 1715, 1708, 1700, 1693, 1686, 1679, 1671, 1664, 1656, 1649, 
	1641, 1633, 1626, 1618, 1610, 1602, 1594, 1586, 1577, 1569, 1561, 1553, 1544, 1536, 1527, 1519, 
	1510, 1501, 1492, 1484, 1475, 1466, 1457, 1448, 1438, 1429, 1420, 1411, 1401, 1392, 1382, 1373, 
	1363, 1354, 1344, 1334, 1325, 1315, 1305, 1295, 1285, 1275, 1265, 1255, 1245, 1234, 1224, 1214, 
	1203, 1193, 1182, 1172, 1161, 1151, 1140, 1129, 1119, 1108, 1097, 1086, 1075, 1064, 1053, 1042, 
	1031, 1020, 1009, 997, 986, 975, 963, 952, 941, 929, 918, 906, 895, 883, 871, 860, 
	848, 836, 825, 813, 801, 789, 777, 765, 753, 741, 729, 717, 705, 693, 681, 669, 
	657, 645, 632, 620, 608, 596, 583, 571, 559, 546, 534, 521, 509, 496, 484, 471, 
	459, 446, 434, 421, 409, 396, 383, 371, 358, 345, 333, 320, 307, 294, 282, 269, 
	256, 243, 231, 218, 205, 192, 179, 167, 154, 141, 128, 115, 102, 90, 77, 64, 
	51, 38, 25, 12, 0, -13, -26, -39, -52, -65, -78, -91, -103, -116, -129, -142, 
	-155, -168, -180, -193, -206, -219, -232, -244, -257, -270, -283, -295, -308, -321, -334, -346, 
	-359, -372, -384, -397, -410, -422, -435, -447, -460, -472, -485, -497, -510, -522, -535, -547, 
	-560, -572, -584, -597, -609, -621, -633, -646, -658, -670, -682, -694, -706, -718, -730, -742, 
	-754, -766, -778, -790, -802, -814, -826, -837, -849, -861, -872, -884, -896, -907, -919, -930, 
	-942, -953, -964, -976, -987, -998, -1010, -1021, -1032, -1043, -1054, -1065, -1076, -1087, -1098, -1109, 
	-1119, -1130, -1141, -1152, -1162, -1173, -1183, -1194, -1204, -1215, -1225, -1235, -1246, -1256, -1266, -1276, 
	-1286, -1296, -1306, -1316, -1326, -1335, -1345, -1355, -1364, -1374, -1383, -1393, -1402, -1412, -1421, -1430, 
	-1439, -1449, -1458, -1467, -1476, -1485, -1493, -1502, -1511, -1520, -1528, -1537, -1545, -1554, -1562, -1570, 
	-1578, -1587, -1595, -1603, -1611, -1619, -1627, -1634, -1642, -1650, -1657, -1665, -1672, -1680, -1687, -1694, 
	-1701, -1709, -1716, -1723, -1730, -1737, -1743, -1750, -1757, -1763, -1770, -1776, -1783, -1789, -1795, -1801, 
	-1807, -1813, -1819, -1825, -1831, -1837, -1842, -1848, -1854, -1859, -1864, -1870, -1875, -1880, -1885, -1890, 
	-1895, -1900, -1905, -1909, -1914, -1919, -1923, -1927, -1932, -1936, -1940, -1944, -1948, -1952, -1956, -1960, 
	-1964, -1967, -1971, -1974, -1978, -1981, -1984, -1987, -1990, -1993, -1996, -1999, -2002, -2005, -2007, -2010, 
	-2012, -2014, -2017, -2019, -2021, -2023, -2025, -2027, -2029, -2031, -2032, -2034, -2035, -2037, -2038, -2039, 
	-2041, -2042, -2043, -2044, -2044, -2045, -2046, -2047, -2047, -2047, -2048, -2048, -2048, -2048, -2048, -2048, 
	-2048, -2048, -2048, -2047, -2047, -2047, -2046, -2045, -2044, -2044, -2043, -2042, -2041, -2039, -2038, -2037, 
	-2035, -2034, -2032, -2031, -2029, -2027, -2025, -2023, -2021, -2019, -2017, -2014, -2012, -2010, -2007, -2005, 
	-2002, -1999, -1996, -1993, -1990, -1987, -1984, -1981, -1978, -1974, -1971, -1967, -1964, -1960, -1956, -1952, 
	-1948, -1944, -1940, -1936, -1932, -1927, -1923, -1919, -1914, -1909, -1905, -1900, -1895, -1890, -1885, -1880, 
	-1875, -1870, -1864, -1859, -1854, -1848, -1842, -1837, -1831, -1825, -1819, -1813, -1807, -1801, -1795, -1789, 
	-1783, -1776, -1770, -1763, -1757, -1750, -1743, -1737, -1730, -1723, -1716, -1709, -1701, -1694, -1687, -1680, 
	-1672, -1665, -1657, -1650, -1642, -1634, -1627, -1619, -1611, -1603, -1595, -1587, -1578, -1570, -1562, -1554, 
	-1545, -1537, -1528, -1520, -1511, -1502, -1493, -1485, -1476, -1467, -1458, -1449, -1439, -1430, -1421, -1412, 
	-1402, -1393, -1383, -1374, -1364, -1355, -1345, -1335, -1326, -1316, -1306, -1296, -1286, -1276, -1266, -1256, 
	-1246, -1235, -1225, -1215, -1204, -1194, -1183, -1173, -1162, -1152, -1141, -1130, -1120, -1109, -1098, -1087, 
	-1076, -1065, -1054, -1043, -1032, -1021, -1010, -998, -987, -976, -964, -953, -942, -930, -919, -907, 
	-896, -884, -872, -861, -849, -837, -826, -814, -802, -790, -778, -766, -754, -742, -730, -718, 
	-706, -694, -682, -670, -658, -646, -633, -621, -609, -597, -584, -572, -560, -547, -535, -522, 
	-510, -497, -485, -472, -460, -447, -435, -422, -410, -397, -384, -372, -359, -346, -334, -321, 
	-308, -295, -283, -270, -257, -244, -232, -219, -206, -193, -180, -168, -155, -142, -129, -116, 
	-103, -91, -78, -65, -52, -39, -26, -13, };
//end of file
