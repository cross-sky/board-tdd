#include "cominc.h"

#define HC1_DS GPIO_Pin_6		//������������
#define HC1_OE GPIO_Pin_7		//�����Ч���͵�ƽ��
#define HC1_STCP GPIO_Pin_8		//����洢������ʱ����
#define HC1_SHCP GPIO_Pin_9		//��������ʱ���ߡ�

#define HC1_PINS GPIO_Pin_9|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8
#define VALVE_SUBB_PINS GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15

#define HC1_H(pin) GPIOC->BSRR=pin
#define HC1_L(pin) GPIOC->BRR=pin

const IOControl relaysOut[]={
	{0,  HC1_PINS, GPIOC ,RCC_APB2Periph_GPIOC},
	{0,  VALVE_SUBB_PINS, GPIOB ,RCC_APB2Periph_GPIOB}
};

//�������ͷ��ͼ̵�����Ӧ�����״ֵ̬��hc595ͳһ���
RelayAndValveDataStruct relayValveData={
	0,0,{0x1000,0x00}
};

//�������ͷ���Ӧ��ͨ������ 
const uint16_t tableValve[ValveKindsMax][8]={
	{0x1000,0x1800,	0x0800,	0x0c00,	0x0400,	0x0600,	0x0200,	0x1200},
	{0x8000,0xc000,	0x4000,	0x6000,	0x2000,	0x3000,	0x1000,	0x9000}
};

//�̵���bitλ�ã���relayValveData��
const uint8_t rekayIndex[16]={
	15, 6, 7, 3, 5, 4, 2, 1,
	0, 9, 10,11,12,13,14,15
};

//�������ͷ�A��bitλ��
#define ValveMainABits 0x1e00

//pb12-15
#define ValVeSubBBits 0xf000

//�������ͷ���ǰ������0-8
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

////Ӳ�����.............
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


	//�ֶ���CLK�ɣ�Ҳ�ͼ���
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	for (name=0; name<2; name++)
	{
		//�����ֶ����ˣ���1��
		//RCC_APB2PeriphClockCmd(controlCd4051[name].clk, ENABLE);
		GPIO_InitStructure.GPIO_Pin = relaysOut[name].pin;
		//��������Ϊ���������
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_Init(relaysOut[name].port, &GPIO_InitStructure);
	}

	HC1_L(HC1_OE);
	vHC1DataOut(0);

	//�ϵ��ʼ��
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

	//...........�������ͷ�b

	if (preValveB != data->valveSubB)
	{
		vValveSubDataOut(data->valveSubB);
		preValveB = data->valveSubB;
	}

}

void vRelay_command5SetRealy(uint16_t dataRelay)
{
	//1.��������������ͷ���λ��
	relayValveData.relaysAndValveMainA &= (ValveMainABits); 
	relayValveData.relaysAndValveMainA |= dataRelay&(~ValveMainABits);
}


/************************************************************************/
/* �������ͷ���0(ul2003Ϊ����״̬���������ͷ��Ƿ���ܶ�һ�ģ�)��Ϊ��ʡ�磬
��һ������ʱ��Ĭ�Ϸ�о�ڵ�ǰλ�ã�ֱ������һ��  

Ϊ�˷�ֹ��Ȧ���¶��������ڵ�ŷ�ֹͣ����ʱ��������ͨ�硣�������򣺽�COM����Ϊ����(+)��
���ڷ�������A������״̬�£�Ϊ0���塣���ͷ�������ʱ��Ӧ����0.5s���ҵ����ţ������ȶ������򣬻�����ʧ��
*/
/************************************************************************/
void vRelay_backValveValue(ValveKinds valveKind)
{
	switch(valveKind)
	{
	case ValveMainA:
		//2.�������ͷ�ֵ��0
		relayValveData.relaysAndValveMainA &=(~ValveMainABits); 
		break;
	case ValveSubB:
		relayValveData.valveSubB &=(~ValVeSubBBits);
		break;
	default:
		break;
	}
}


