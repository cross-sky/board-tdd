#ifndef __STM32_UB_RELAY_H
#define __STM32_UB_RELAY_H

/*
����		Ӳ�� �����Ӧ��bitλ
ѹ����	h01	sq15
��ͨ��	h02	sq6
ѭ����	h03	sq7
�����	h04	sq3
��ˮ��	h05	sq5
��ˮ��	h06	sq4
��ˮ��	h07	sq2
����ȴ�	h08	sq1
���		h09	sq0
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
	uint16_t relaysAndValveMainA;	//relay ��λ�� valveA ��λ(12-15)
	uint16_t valveSubB;
	uint16_t tValveValue[ValveKindsMax];	//��������󻺴�ֵ

}RelayAndValveDataStruct;

void vRelaySet(RelayName_t relay, StateType state);
void HardValve_runDirect(uint16_t addsOrDecr,uint16_t valveKind);

void vRelay_command5SetRealy(uint16_t dataRelay);

void vRelayInit(void);
void vTask4RelayOutProcess(void);

RelayAndValveDataStruct* vRelay_getDataStruct(void);
void vRelay_backValveValue(ValveKinds valveKind);
#endif

