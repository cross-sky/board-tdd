#ifndef __STM32_UB_CD4051_H
#define __STM32_UB_CD4051_H

#define CDIN_OPEN 0

typedef enum {
	CDin01_WATER_OPEN=0x00,	//水流
	CDin02_LOW_PREESURE,	//低压A	对应的异常
	CDin03_HIGH_PREESURE,	//高压A  对应的异常
	CDin04,			//拨码1
	CDin05_LLEVEL,			//低水位
	CDin06,			//拨码2
	CDin07_MLEVEL,			//中水位
	CDin08_HLEVEL,			//高水位
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

