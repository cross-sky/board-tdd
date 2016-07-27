#ifndef __STM32_UB_ADC_H
#define __STM32_UB_ADC_H
#include "StateType.h"

#define ADC_NeedRunCount	5

typedef enum{
	ADC01_ENVT,		//环温度
	ADC02_WTANK,	//水箱温度
	ADC03_AIN,	//A吸气
	ADC04_AMI,		//A蒸发温度
	ADC05_AOUT,		//A排气温度
	ADC06_WIN,		//进水温度
	ADC07_WOUT,		//出水温度
	ADC8_INTER,	//内部温度
	ADC09_current,	//电流
	ADCMAX,			//最大值
	ADC_USED,
	ADC_UNUSED
}ADCENUM;


void vADCSimpling(void);
ptrState BspAdc_getPtrStateAdc(void);
void vADCsetSingleInputFlag(uint16_t flag);
void vADCInit(void);

void vADCDmaChannel1_ISR(void);

#endif

