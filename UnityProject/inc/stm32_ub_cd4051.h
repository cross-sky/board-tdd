#ifndef __STM32_UB_CD4051_H
#define __STM32_UB_CD4051_H

#define CDIN_OPEN 0

typedef enum {
	CDin01_HIGH_PREESURE=0x00,	//��ѹA  ��Ӧ���쳣
	CDin02_LOW_PREESURE,			//��ѹA	��Ӧ���쳣
	CDin03_WATER_OPEN,			//ˮ��
	CDin04,			//��ѹB	��Ӧ���쳣
	CDin05,			//��ѹB	��Ӧ���쳣
	CDin06_HLEVEL,			//ˮ��
	CDin07_MLEVEL,			//��ˮλ
	CDin08_LLEVEL,			//��ˮλ
	CDinMax,
	CDin_EXTRAL_TEMPERATUR110,
	CDunused
}CD4051Input;

typedef enum {
	CDabc=0x00,
	CDControlMAX
}CDControl;

typedef enum {
	CDcom1=0x00,
	CDcomMax
}CDCom;

typedef struct  IOControl
{
	uint8_t 		name;
	uint16_t		pin;
	GPIO_TypeDef*	port;
	uint32_t		clk;
}IOControl;

ptrState BspAdc_getPtrStateCd4051(void);
uint16_t CD4051_IOread(void);
void vCd4051Simpling(void);
void CD4051_startSimple(ptrState s);
void GpioPort_output(GPIO_TypeDef* gpioPort, uint16_t pins, uint16_t dataOut);

void vCd4051Init(void);


/*
#define IOH(ioControl_) (((*ioControl_).port)->BSRR=(*ioControl_).pin)
//#define IOH(ioControl_) ((((IOControl*)ioControl_)->port)->BSRR=((IOControl*)ioControl_)->pin)
#define IOL(ioControl_) (((*ioControl_).port)->BRR=(*ioControl_).pin)

*/

#endif

