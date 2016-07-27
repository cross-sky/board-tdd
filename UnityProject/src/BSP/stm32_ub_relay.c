#include "cominc.h"

RelayAndValveDataStruct relayValveData={
	0,0
};

const uint8_t tableValve[ValveKindsMax][8]={
	{0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8},
	{0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8}
};
uint8_t tableIndex=0;

void vRelaySet(RelayName_t relay, StateType state)
{
	if (relay >=RelayMax)
	{
		return;
	}
	if (state == STATE_ON)
	{
		relayValveData.relaysAndValveMainA |= 1<<relay;
	}else{
		relayValveData.relaysAndValveMainA &= 0<<relay;
	}
}

////硬件输出.............
void HardValve_runDirect(uint16_t addsOrDecr,uint16_t valveKind)
{
	if (addsOrDecr == adds)
	{
		tableIndex++;
	}
	else
	{
		tableIndex--;
	}
	tableIndex &= 0x07;

	switch(valveKind)
	{
	case ValveMainA:
		{
			relayValveData.relaysAndValveMainA = (relayValveData.relaysAndValveMainA & 0x7f) | tableValve[valveKind][tableIndex];
			break;
		}
	case ValveSubB:
		{
			relayValveData.valveSubB = (relayValveData.valveSubB & 0x07) | tableValve[valveKind][tableIndex];
			break;
		}
	default:break;
	}
}


/*
const IOControl relaysOut[]={
	{Relay01Compressor, 10, GPIO_Pin_10, GPIOD,RCC_APB2Periph_GPIOD},		//
	{Relay02Valve4way,	11, GPIO_Pin_11, GPIOD,RCC_APB2Periph_GPIOD},		//
	{Relay03Pump,		12, GPIO_Pin_12, GPIOD,RCC_APB2Periph_GPIOD},		//
	{Relay04ElecHeater, 13, GPIO_Pin_13, GPIOD,RCC_APB2Periph_GPIOD},		//
	{Relay05,			14, GPIO_Pin_14, GPIOD,RCC_APB2Periph_GPIOD},		//
	{Relay06,			0,  GPIO_Pin_0, GPIOD,RCC_APB2Periph_GPIOD},		//
	{Relay07,			1,  GPIO_Pin_1, GPIOD,RCC_APB2Periph_GPIOD},		//
	{Relay08,			2,  GPIO_Pin_2, GPIOD,RCC_APB2Periph_GPIOD},		//
	{Relay09,			3,  GPIO_Pin_3, GPIOD,RCC_APB2Periph_GPIOD},		//
	{Relay10Motor,		9,  GPIO_Pin_9, GPIOD,RCC_APB2Periph_GPIOD},		//
	{Relay11,			8,  GPIO_Pin_8, GPIOD,RCC_APB2Periph_GPIOD}		//
};

//@@@@@@@@1. need init
Event_t eventRelay={
	0,0,0,0
};

void vRelayInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	uint8_t name;

	//手动开CLK吧，也就几个
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

	for (name=0; name<RelayMax; name++)
	{
		//如上手动开了，就1个
		//RCC_APB2PeriphClockCmd(controlCd4051[name].clk, ENABLE);
		GPIO_InitStructure.GPIO_Pin = relaysOut[name].pin;
		//这里设置为推挽输出，
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_Init(relaysOut[name].port, &GPIO_InitStructure);
		IOL(&relaysOut[name]);
	}
}


void vRelaySet(RelayName_t relay, State_t state)
{
	Event_t *e=&eventRelay;
	if (relay >=RelayMax)
	{
		return;
	}
	if (state == STATE_ON)
	{
		e->code |= 1<<relaysOut[relay].name;
	}else{
		e->code &= ~(1<<relaysOut[relay].name);
	}
}

void vRelaySetAll(void)
{
	eventRelay.code=0xffff;
}

void vRelayClearAll(void)
{
	eventRelay.code=0x00;
}

void vTask4RelayOutProcess(void)
{
	uint16_t dat=eventRelay.code;
	uint8_t i;
	static uint16_t preData=0;

	if (preData == dat)
	{
		return;
	}
	preData = dat;

	for(i=0;i<RelayMax;i++)
	{

		if (dat&0x01)
		{
			IOH(&relaysOut[i]);
		}
		else
			IOL(&relaysOut[i]);

		dat>>=1;
	}
	
	
}

*/

