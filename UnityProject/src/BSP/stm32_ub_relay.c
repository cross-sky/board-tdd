#include "cominc.h"

#define HC1_DS GPIO_Pin_6		//串行数据输入
#define HC1_OE GPIO_Pin_7		//输出有效（低电平）
#define HC1_STCP GPIO_Pin_8		//输出存储器锁存时钟线
#define HC1_SHCP GPIO_Pin_9		//数据输入时钟线　

#define HC1_PINS GPIO_Pin_9|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8
#define VALVE_SUBB_PINS GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15

#define HC1_H(pin) GPIOC->BSRR=pin
#define HC1_L(pin) GPIOC->BRR=pin

const IOControl relaysOut[]={
	{0,  HC1_PINS, GPIOC ,RCC_APB2Periph_GPIOC},
	{0,  VALVE_SUBB_PINS, GPIOB ,RCC_APB2Periph_GPIOB}
};

//电子膨胀阀和继电器对应的输出状态值，hc595统一输出
RelayAndValveDataStruct relayValveData={
	0,0,{0x1000,0x00}
};

//电子膨胀阀对应的通电相数 
const uint16_t tableValve[ValveKindsMax][8]={
	{0x1000,0x1800,	0x0800,	0x0c00,	0x0400,	0x0600,	0x0200,	0x1200},
	{0x8000,0xc000,	0x4000,	0x6000,	0x2000,	0x3000,	0x1000,	0x9000}
};

//继电器bit位置，在relayValveData中
const uint8_t rekayIndex[16]={
	15, 6, 7, 3, 5, 4, 2, 1,
	0, 9, 10,11,12,13,14,15
};

//电子膨胀阀A的bit位置
#define ValveMainABits 0x1e00

//pb12-15
#define ValVeSubBBits 0xf000

//电子膨胀阀当前拍数，0-8
uint8_t tableIndex[ValveKindsMax]={0,0};

void delay1us(void)
{
	uint8_t i=5;
	i--;
}

RelayAndValveDataStruct* vRelay_getDataStruct(void)
{
	return &relayValveData;
}

void vRelaySet(RelayName_t relay, StateType state)
{
	
	if (relay >=RelayMax)
	{
		return;
	}
	if (state == STATE_ON)
	{
		relayValveData.relaysAndValveMainA |= 1<<rekayIndex[relay];
	}else{
		relayValveData.relaysAndValveMainA &= 0<<rekayIndex[relay];
	}
}

////硬件输出.............
void HardValve_runDirect(uint16_t addsOrDecr,uint16_t valveKind)
{
	switch(valveKind)
	{
	case ValveMainA:
		{
			relayValveData.relaysAndValveMainA = (relayValveData.relaysAndValveMainA & (~ValveMainABits)) | tableValve[valveKind][tableIndex[valveKind]];
			break;
		}
	case ValveSubB:
		{
			relayValveData.valveSubB = (relayValveData.valveSubB & 0x07) | tableValve[valveKind][tableIndex[valveKind]];
			break;
		}
	default:break;
	}

	if (addsOrDecr == adds)
	{
		tableIndex[valveKind]++;
	}
	else
	{
		tableIndex[valveKind]--;
	}
	tableIndex[valveKind] &= 0x07;
}

void vHC1DataOut(uint16_t dataOut)
{
	//RelaysAndValveMainA
	uint8_t i;

	HC1_L(HC1_SHCP);
	HC1_L(HC1_STCP);
	for(i=0;i<16;i++)
	{
		if ((dataOut<<i) & 0x8000)
		{
			HC1_H(HC1_DS);
		}
		else
			HC1_L(HC1_DS);

		HC1_L(HC1_SHCP);
		delay1us();

		HC1_H(HC1_SHCP);
		delay1us();
	}

	HC1_H(HC1_STCP);

}

void vValveSubDataOut(uint16_t dataOut)
{
	uint16_t tt;
	tt =  GPIOB->IDR & (~ValVeSubBBits);
	tt |= dataOut;
	GPIOB->ODR = tt;
}

void vRelayInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	uint8_t name;


	//手动开CLK吧，也就几个
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	for (name=0; name<2; name++)
	{
		//如上手动开了，就1个
		//RCC_APB2PeriphClockCmd(controlCd4051[name].clk, ENABLE);
		GPIO_InitStructure.GPIO_Pin = relaysOut[name].pin;
		//这里设置为推挽输出，
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_Init(relaysOut[name].port, &GPIO_InitStructure);
	}

	HC1_L(HC1_OE);
	vHC1DataOut(0);

	//上电初始化
	ValveCalc_valveInit();
}




void vTask4RelayOutProcess(void)
{
	static uint16_t preRelayValveA=0,preValveB=0;
	RelayAndValveDataStruct *data=&relayValveData;

	if (preRelayValveA != data->relaysAndValveMainA)
	{
		vHC1DataOut(data->relaysAndValveMainA);
		preRelayValveA = data->relaysAndValveMainA;
	}

	//...........电子膨胀阀b

	if (preValveB != data->valveSubB)
	{
		vValveSubDataOut(data->valveSubB);
		preValveB = data->valveSubB;
	}

}

void vRelay_command5SetRealy(uint16_t dataRelay)
{
	//1.清除除掉电子膨胀阀的位数
	relayValveData.relaysAndValveMainA &= (ValveMainABits); 
	relayValveData.relaysAndValveMainA |= dataRelay&(~ValveMainABits);
}


/************************************************************************/
/* 电子膨胀阀清0(ul2003为高阻状态，电子膨胀阀是否会跑动一拍？)，为了省电，
下一次启动时，默认阀芯在当前位置，直接走下一拍  

为了防止线圈的温度上升，在电磁阀停止动作时，不能再通电。电流方向：将COM侧作为正极(+)。
初期阀基点在A相励磁状态下，为0脉冲。膨胀阀反向动作时，应加上0.5s左右的励磁，令电机稳定。否则，会引起失步
*/
/************************************************************************/
void vRelay_backValveValue(ValveKinds valveKind)
{
	switch(valveKind)
	{
	case ValveMainA:
		//2.电子膨胀阀值清0
		relayValveData.relaysAndValveMainA &=(~ValveMainABits); 
		break;
	case ValveSubB:
		relayValveData.valveSubB &=(~ValVeSubBBits);
		break;
	default:
		break;
	}
}


