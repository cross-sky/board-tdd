#ifndef __STM32_UB_ADC_H
#define __STM32_UB_ADC_H
#include "StateType.h"

#define ADC_NeedRunCount	5
#define AirOutTemperMax110 1100		//110��
#define AirOutTemperMax100 1000		//110��
#define EnvirTemper5		50		//5��

typedef enum{
	ADC01_ENVT,		//���¶�
	ADC02_WTANK,	//ˮ���¶�
	ADC03_AIN,	//A����
	ADC04_AMI,		//A�����¶�
	ADC05_AOUT,		//A�����¶�
	ADC06_WIN,		//��ˮ�¶�
	ADC07_WOUT,		//��ˮ�¶�
	ADC8_INTER,	//�ڲ��¶�
	ADC09_current,	//����
	ADCMAX,			//���ֵ
	ADC_USED,
	ADC_UNUSED
}ADCENUM;


void vADCSimpling(void);
ptrState BspAdc_getPtrStateAdc(void);
void vADCsetSingleInputFlag(uint16_t flag);
void vADCInit(void);

void vADCDmaChannel1_ISR(void);

#endif

