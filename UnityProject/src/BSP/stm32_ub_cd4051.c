#include "cominc.h"

#define  ControlCd4051Pins GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8

const IOControl controlCd4051[]={
	{CDabc, ControlCd4051Pins, GPIOB,RCC_APB2Periph_GPIOB}
};

const IOControl comminCd4051[]={
	{CDcom1, GPIO_Pin_9, GPIOB,RCC_APB2Periph_GPIOB}
};

StateStruct stateCd4051={
	STATE_ON,0,CDinMax*ADC_NeedRunCount,200,STATE_DATA_DONE
};

const uint16_t tabCDabc[8]={
		0,	1<<6,2<<6,3<<6,
		4<<6,5<<6,6<<6,7<<6
};

ptrState BspAdc_getPtrStateCd4051(void)
{
	return &stateCd4051;
}

uint16_t CD4051_IOread(void)
{
	//return (GPIOB->IDR & GPIO_Pin_9);
	return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);
	//return 0x01;
}

void CD4051_startSimple(ptrState s)
{
	uint16_t data;
	uint8_t runTimes,cdInX;
	runTimes = State_getRunCount(s)>>3;//求商，需要读取5次相同cdin数据，
	cdInX = State_getRunCount(s)&0x07;//求余，目前读取到cdinX口数据
	data = Inp_getDataCd4051(runTimes);
	data |= CD4051_IOread()<<cdInX;
	Inp_setDataCd4051(data, runTimes);
}

void vCd4051Simpling(void)
{
	uint8_t cdIndex;
	StateStruct *s=&stateCd4051;
	if (State_getDataProcessedFlag(s) == STATE_DATA_UNDONE)
	{
		return;
	}

	if (State_checkStateIsOn(s) ==  FALSE)
	{
		return;
	}

	CD4051_startSimple(s);
		//IO out
	State_addRunCount(s);

	cdIndex = State_getRunCount(s)&0x07;
	GpioPort_output(controlCd4051[CDabc].port,ControlCd4051Pins,tabCDabc[cdIndex]);
}

void vCd4051Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	uint8_t name;

	//手动开CLK吧，也就几个
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	for (name=0; name<CDcomMax; name++)
	{
		//如上手动开了，就1个
		//RCC_APB2PeriphClockCmd(controlCd4051[name].clk, ENABLE);
		GPIO_InitStructure.GPIO_Pin = comminCd4051[name].pin;
		//这里设置为浮空
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(comminCd4051[name].port, &GPIO_InitStructure);
	}

	for (name=0; name<CDControlMAX; name++)
	{
		//如上手动开了，就1个
		//RCC_APB2PeriphClockCmd(controlCd4051[name].clk, ENABLE);
		GPIO_InitStructure.GPIO_Pin = controlCd4051[name].pin;
		//这里设置为开漏输出，已接上拉电阻
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_Init(controlCd4051[name].port, &GPIO_InitStructure);
		//初始时要先设置cd4051打开cdin01对应的in口
	}
	//CDabc set 0
	GpioPort_output(controlCd4051[CDabc].port,ControlCd4051Pins, 0);
}

//offset
void GpioPort_output(GPIO_TypeDef* gpioPort, uint16_t pins, uint16_t dataOut)
{
	uint16_t tdata;
	tdata = gpioPort->IDR & (~pins);
	gpioPort->ODR = tdata | dataOut;
}

