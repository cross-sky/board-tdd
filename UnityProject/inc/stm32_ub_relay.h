#ifndef __STM32_UB_RELAY_H
#define __STM32_UB_RELAY_H

/*
名称		硬件 软件对应的bit位
压缩机	h01	sq15
四通阀	h02	sq6
循环泵	h03	sq7
电加热	h04	sq3
回水阀	h05	sq5
补水阀	h06	sq4
供水泵	h07	sq2
电加热带	h08	sq1
风机		h09	sq0
*/

typedef enum{
	Relay01Compressor=0x00,	//
	Relay02Valve4way,
	Relay03CyclePump,
	Relay04ElecHeater,
	Relay05BackWater,
	Relay06SupplyWater,
	Relay07SupplyPump,
	Relay08Cmhtr,
	Relay09Motor,
	RelayMax
}RelayName_t;

typedef struct RelayAndValveDataStruct{
	uint16_t relaysAndValveMainA;	//relay 低位， valveA 高位(12-15)
	uint16_t valveSubB;
	uint16_t tValveValue[ValveKindsMax];	//步数走完后缓存值

}RelayAndValveDataStruct;

void vRelaySet(RelayName_t relay, StateType state);
void HardValve_runDirect(uint16_t addsOrDecr,uint16_t valveKind);

void vRelay_command5SetRealy(uint16_t dataRelay);

void vRelayInit(void);
void vTask4RelayOutProcess(void);

RelayAndValveDataStruct* vRelay_getDataStruct(void);
void vRelay_backValveValue(ValveKinds valveKind);
#endif

