#ifndef __STM32_UB_CD4051_H
#define __STM32_UB_CD4051_H

#define CDIN_OPEN 0

typedef enum {
	CDin01_HIGH_PREESURE=0x00,	//高压A  对应的异常
	CDin02_LOW_PREESURE,			//低压A	对应的异常
	CDin03_WATER_OPEN,			//水流
	CDin04,			//高压B	对应的异常
	CDin05,			//低压B	对应的异常
	CDin06_HLEVEL,			//水泵
	CDin07_MLEVEL,			//低水位
	CDin08_LLEVEL,			//中水位
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
}CDControl;

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

